#define _HAS_STD_BYTE 0 // Risolve conflitto byte tra C++17 e Windows headers
#define UNICODE         // Assicura che Windows usi le versioni wide delle strutture
#define _UNICODE

#include "VSPScriptGenerator.h"
#include "VSPAeroGenerator.h"
#include "VSPGEOMETRYEXTRACTOR.h"
#include "DELTAXANDDIAMETERS.h"
#include "ControlSurfaceBuilder.h"
#include "ConvLength.h"
#include "EnumLENGTH.h"
#include "ATMOSISA.h"
#include "CONVFILEVSP.h"
#include "READLOADFILE.h"
#include "READPOLARFILE.h"
#include "PLOT.h"
#include "SAVEFILES.h"
#include "WEIGHTS.h"
#include "XMLPARSER.h"
#include "WETTEDAREA.h"
#include "MACCALCULATOR.h"
#include "BUILDAIRCRAFT.h"
#include "COGCALCULATOR.h"
#include "Interpolant2D.h"
#include "EnumAircraftEngineType.h"
#include "EnumEnginePosition.h"
#include "EnumMaterial.h"
#include "EnumUndercarriagePosition.h"
#include "EnumTypeOfWing.h"
#include "COGCALCULATOR.h"
#include "BASEAIRCRAFTDATA.h"
#include "WINGBASEDATA.h"
#include "FUSELAGEBASEDATA.h"
#include "ENGINEBASEDATA.h"
#include "LONGITUDINALSTABILTY.h"
#include "DIRECTIONALSTABILITY.h"
#include "LATERALSTABILITY.h"
#include "SILENTORCOMPONENT.h"
#include "StaticDerivativesExcelWriter.h"
#include <Eigen/Dense>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <list>
#include <format>
#include <cmath>
#include <numeric>
#include <cctype>
#include <string>
#include <iostream>

int main()
{

    // ==================== DETERMINE BUILD DIRECTORY ====================
    // Get the current directory and go to build/
    std::filesystem::path currentDir = std::filesystem::current_path();
    std::filesystem::path buildDir;

    // If already in build/, stay there
    if (currentDir.filename() == "build")
    {
        buildDir = currentDir;
    }
    // Otherwise go to build/
    else if (std::filesystem::exists(currentDir / "build"))
    {
        buildDir = currentDir / "build";
    }
    // Or if you are in another subdirectory, go to ../build
    else if (std::filesystem::exists(currentDir.parent_path() / "build"))
    {
        buildDir = currentDir.parent_path() / "build";
    }
    else
    {
        std::cerr << "ERROR: Cannot find build directory!" << std::endl;
        std::cerr << "Current directory: " << currentDir << std::endl;
        return 1;
    }

    // Change to build directory
    std::filesystem::current_path(buildDir);

    std::cout << "=== VSP Script Generator ===" << std::endl;
    std::cout << "Working directory: " << std::filesystem::current_path() << std::endl;
    std::cout << std::endl;

    // ====================  SCRIPT GENERATION ====================
    std::cout << "\n1. Generating AngelScript script (.vspscript)..." << std::endl;

    VSP::ScriptGenerator script("P2012.vspscript");

    // --- CONTROL SURFACES ---
    VSP::Aircraft ac;
    ac.movables = "faer"; // Flap + Aileron + Elevator + Rudder

    // ac.ver.id = "vertical";
    // ac.ver.mov.type = {'r'};
    // ac.ver.mov.defl = {5.0};
    // ac.ver.isSplittedDeflection = false;

    // --- MAIN WING ---
    VSP::Wing wing;
    wing.id = "wing";
    ac.wing.id = wing.id;
    wing.typeOfWing = TypeOfWing::STRAIGHT_TAPERED;
    wing.useDetailedPanels = true;
    wing.airfoilType = "XS_FIVE_DIGIT";
    wing.idealcl = {0.3, 0.3, 0.3, 0.3};
    wing.camberloc = {0.15, 0.15, 0.15, 0.15};
    wing.thickchord = {0.15, 0.15, 0.15, 0.12};
    wing.twist = {3, -0.1060, -1.4};
    wing.twistloc = {0, 0, 0};
    wing.xloc = 4.568;
    wing.yloc = 0.0;
    wing.zloc = 0.773;
    wing.xrot = 0;
    wing.yrot = 0;
    wing.zrot = 0;

    wing.span = {2.882, 4.118 * 0.5, 4.118 * 0.5};
    wing.ctip = {2.0038, 1.7269, 1.45};
    wing.croot = {2.0038, 2.0038, 1.7269};

    wing.sectessU = {21, 21, 21};
    wing.wtess = 40;

    // Control surfaces
    wing.mov.type = {'f', 'a'};
    wing.mov.eta_inner = {0.10, 0.70};
    wing.mov.eta_outer = {0.70, 1.0};
    wing.mov.cf_c_inner = {0.25, 0.3};
    wing.mov.cf_c_outer = {0.3, 0.3};
    wing.mov.tessellation = {10, 10};
    wing.mov.defl = {30, 0};

    ac.wing.mov.type = wing.mov.type;
    ac.wing.mov.defl = wing.mov.defl;

    wing.sweep = {0, 0, 0};
    wing.sweeploc = {0, 0, 0};
    wing.secsweeploc = {1, 1, 1};
    wing.dihedral = {0, 2.73, 2.73};

    script.makeWing(wing, 2);
    std::cout << "   - Main wing created (with flap and aileron)" << std::endl;

    std::cout << "     - Calculating MAC..." << std::endl;
    std::cout << "MAC: " << wing.MAC << " m" << std::endl;
    std::cout << "yMAC: " << wing.yMAC << " m" << std::endl;
    std::cout << "Total span: " << wing.totalSpan << " m" << std::endl;
    std::cout << "Mean dihedral: " << wing.averageDihedral << " deg" << std::endl;

    //--- HORIZONTAL TAIL ---
    VSP::Wing horizontal;
    horizontal.id = "horizontal";
    ac.hor.id = horizontal.id;
    horizontal.type = "WING";
    horizontal.useDetailedPanels = true;
    horizontal.airfoilType = "XS_FOUR_SERIES";
    horizontal.camber = {0, 0};
    horizontal.camberloc = {0.2, 0.2};
    horizontal.thickchord = {0.12, 0.12};
    horizontal.twist = {0};
    horizontal.twistloc = {0.25};
    horizontal.xloc = 10.70;
    horizontal.yloc = 0.0;
    horizontal.zloc = 0.200;
    horizontal.xrot = 0;
    horizontal.yrot = 0;
    // horizontal.span = {2.622};
    horizontal.span = {2.69309};
    // horizontal.ctip = {0.698};
    horizontal.ctip = {0.89150};
    // horizontal.croot = {1.298};
    horizontal.croot = {1.42319};
    horizontal.sweep = {13.1};
    horizontal.sweeploc = {0};
    horizontal.secsweeploc = {1};
    horizontal.dihedral = {0};
    horizontal.sectessU = {15};
    horizontal.wtess = {25};
    horizontal.mov.type = {'e'};
    horizontal.mov.eta_inner = {0.0};
    horizontal.mov.eta_outer = {1.0};
    horizontal.mov.cf_c_inner = {0.25};
    horizontal.mov.cf_c_outer = {0.3};
    horizontal.mov.tessellation = {10};
    horizontal.mov.defl = {-10};

    ac.hor.mov.type = horizontal.mov.type;
    ac.hor.mov.defl = horizontal.mov.defl;

    script.makeWing(horizontal, 2);
    std::cout << "   - Horizontal tail created (with elevator)" << std::endl;

    std::cout << "     - Calculating MAC..." << std::endl;
    std::cout << "MAC: " << horizontal.MAC << " m" << std::endl;
    std::cout << "yMAC: " << horizontal.yMAC << " m" << std::endl;
    std::cout << "Total span: " << horizontal.totalSpan << " m" << std::endl;

    // --- VERTICAL TAIL ---
    VSP::Wing vertical;
    vertical.id = "vertical";
    ac.ver.id = vertical.id;
    vertical.type = "WING";
    vertical.airfoilType = "XS_FOUR_SERIES";
    vertical.useDetailedPanels = true;
    vertical.camber = {0, 0, 0};
    vertical.camberloc = {0.2, 0.2, 0.2};
    vertical.thickchord = {0.1, 0.1, 0.1};
    vertical.twist = {0, 0};
    vertical.twistloc = {0, 0};
    vertical.xloc = 8.240;
    vertical.yloc = 0.0;
    vertical.zloc = 0.738;
    vertical.xrot = 90;
    vertical.yrot = 0;
    // vertical.zrot = -3.77;
    vertical.zrot = -5.409;
    // vertical.span = {0.372, 1.962};
    // vertical.ctip = {2.412, 0.768};
    // vertical.croot = {3.157, 2.412};
    vertical.span = {0.372, 2.18354};
    vertical.ctip = {1.92778, 0.76914};
    vertical.croot = {3.24261, 1.92778};
    // vertical.sweep = {60, 32.85714};
    vertical.sweep = {73.84152, 30.12277};
    vertical.sweeploc = {0, 0};
    vertical.secsweeploc = {1, 1};
    vertical.dihedral = {0, 0};
    vertical.sectessU = {21};
    vertical.wtess = {41};
    vertical.mov.type = {'r'};
    vertical.mov.eta_inner = {0.0};
    vertical.mov.eta_outer = {1.0};
    vertical.mov.cf_c_inner = {0.25};
    vertical.mov.cf_c_outer = {0.3};
    vertical.mov.tessellation = {10};
    ac.ver.mov.type = vertical.mov.type;
    ac.ver.isSplittedDeflection = false;
    ac.ver.activatedRudder = {1};
    ac.ver.mov.defl = {0};

    script.makeWing(vertical, 0);
    std::cout << "   - Vertical tail created (with rudder)" << std::endl;

    // --- FUSELAGE ---
    VSP::Fuselage fus;
    fus.id = "Fuselage";
    fus.type = "TransportFuse";
    fus.advancedFuselage = false;
    // fus.fuselagePresetName = "AirbusGenericTransportJet";
    fus.customFuselage = true;

    // fus.length         = 37.57;
    // fus.diameter       = 4.14;
    // fus.width          = 3.95010;
    fus.length = 11.7;
    fus.diameter = 1.546;
    // fus.nosemult = 1.902;
    // fus.aftmult = 2.665;
    // fus.nosecenter = -0.217;
    // fus.aftcenter = 0.164;
    // fus.aftwidth = 0.185;
    // fus.aftheight = 0.364;
    // fus.utess = 30;
    // fus.wtess = 15;
    fus.nosemult = 1.945;
    fus.aftmult = 2.665;
    fus.nosecenter = -0.319;
    fus.aftcenter = 0.120;
    fus.aftwidth = 0.185;
    fus.aftheight = 0.256;
    fus.utess = 30;
    fus.wtess = 15;


    script.makeFuselage(fus, wing, horizontal);
    std::cout << "   - Fuselage created" << std::endl;
    std::cout << "     - Calculating tail arm..." << std::endl;
    std::cout << "Tail arm: " << fus.tailArm << " m" << std::endl;

    // // --- NACELLE (engines) ---
    VSP::Nacelle nac;
    nac.id = "nacelle";
    nac.advancedNacelle = true;
    // nac.nacellePresetName = "GeneralTurboFan";
    // nac.nacellePresetName = "GeneralTurboProp_Spec2";
    nac.nacellePresetName = "GeneralTurboProp_Spec1";
    // nac.length = 3.458;
    // nac.diameter = 2.735;
    // nac.xloc = {13.06,13.06};
    // nac.yloc = {5.75,-5.75};
    // nac.zloc = {-1.283,-1.283};
    // nac.length = 2.72821;
    nac.length = 3.361182;
    nac.aDiameter = 0.82918;
    nac.bDiameter = 0.80097;
    nac.xloc = {3.443, 3.443};
    nac.yloc = {2.295, -2.295};
    nac.zloc = {0.820, 0.820};
    nac.xrot = {0.0, 0.0};
    nac.yrot = {0.0, 0.0};
    nac.zrot = {0.0, 0.0};

    VSP::Disk disk;
    disk.id = "disk";
    disk.type = "Disk";
    disk.diameter = {1.950, 1.950};
    disk.xloc = {3.443, 3.443};
    disk.yloc = {2.295, -2.295};
    disk.zloc = {0.820, 0.820};
    disk.xrot = {0.0, 0.0};
    disk.yrot = {0.0, 0.0};
    disk.zrot = {0.0, 0.0};
    disk.utess = {8, 8};
    disk.wtess = {9, 9};

    for (size_t i = 1; i <= nac.xloc.size(); i++)
    {

        script.makeNacelle(nac, i);
        script.MakeDisk(disk, i - 1);
    }

    // nac.type = "STACK";
    // nac.width = 1.0;
    // nac.height = 1.0;
    // nac.chord = 2.5;
    // nac.thickchord = 0.12;
    // nac.xloc = {4.5, 4.5};
    // nac.yloc = {2.8, 2.8};
    // nac.zloc = {-0.5, -0.5};
    // nac.yrot = {0.0, 0.0};
    // nac.utess = 20;
    // nac.wtess = 20;

    // script.makeNacelle(nac, 1);  // Left
    // script.makeNacelle(nac, 2);  // Right
    // std::cout << "   - Nacelle create (left and right)" << std::endl;

    // // --- POD (external tanks) ---
    // VSP::Pod pod;
    // pod.id = "pod";
    // pod.type = "POD";
    // pod.length = {2.5, 2.5};
    // pod.fineratio = {5.0, 5.0};
    // pod.xloc = {6.5, 6.5};
    // pod.yloc = {3.5, 3.5};
    // pod.zloc = {-0.4, -0.4};
    // pod.yrot = {0.0, 0.0};
    // pod.utess = 18;
    // pod.wtess = 18;

    // script.makePod(pod, 1);  // Left
    // script.makePod(pod, 2);  // Right
    // std::cout << "   - External pods created (left and right)" << std::endl;

    // // --- EO/IR SENSOR ---
    // VSP::EOIR eoir;
    // eoir.id = "eoir";
    // eoir.type = "POD";
    // eoir.length = {0.4};
    // eoir.fineratio = {1.8};
    // eoir.xloc = {2.0};
    // eoir.yloc = {0.0};
    // eoir.zloc = {-1.5};
    // eoir.yrot = {0.0};
    // eoir.utess = 15;
    // eoir.wtess = 15;

    // script.makeEOIR(eoir, 1);
    // std::cout << "   - EO/IR sensor created" << std::endl;

    // --- DEGEN GEOM ---
    script.makeDegenGeom("P2012", false);
    std::cout << "   - DegenGeom configured" << std::endl;

    std::cout << "\n✓ AngelScript script generated: P2012.vspscript" << std::endl;

    // ==================== GENERATE VSPAERO FILE ====================
    std::cout << "\n2. Generating VSPAERO file (.vspaero)..." << std::endl;

    VSP::VSPAeroGenerator vspaero("P2012");

    // --- AERODYNAMIC SETTINGS ---
    VSP::AeroSettings settings;

    double altitude = 10000.0;

    ConvLength convlength(Length::FT, Length::M, altitude);

    altitude = convlength.getConvertedValues(); // Convert altitude to meters for VSPAERO

    auto [T, a, P, rho] = Atmosphere::atmosisa(altitude); // Atmosisa like in MATLAB

    double speedOfSound = Atmosphere::ISA::speedOfSound(altitude); // Calculate only the speed of sound
    double mu = Atmosphere::ISA::viscosity(altitude);              // Calculate only the dynamic viscosity

    MACCalculator macCalc;

    double span = 2 * std::accumulate(wing.span.begin(), wing.span.end(), 0.0);

    // .back() e .front() per prendere l'ultimo e il primo elemento di un vettore
    double taperRatio = wing.ctip.back() / wing.croot.front();

    settings.altitude = altitude;                                                                       // Returns the first converted altitude value
    settings.Sref = 25.0;                                                                               // Reference area [m²]
    settings.Cref = macCalc.getMAC(TypeOfWing::STRAIGHT_TAPERED, wing.croot.front(), taperRatio, span); // Reference chord [m]
    settings.Bref = 14.0;                                                                               // Wingspan [m]
    settings.X_cg = wing.xloc + 0.25 * settings.Cref;                                                   // Center of gravity
    settings.Y_cg = 0.0;
    settings.Z_cg = 0.25 * fus.diameter;
    settings.Mach = 0.15;                                                // Mach number
    settings.AoA = {-2, 0, 2};                                           // Angles of attack
    settings.Beta = {0};                                                 // Sideslip angle
    settings.Vinf = settings.Mach * speedOfSound;                        // Velocity [m/s]
    settings.rho = rho;                                                  // Air density [kg/m³]
    settings.ReCref = settings.Vinf * settings.rho * settings.Cref / mu; // Reynolds
    settings.ClMax = 1.7;                                                // Estimated Cl max
    settings.MaxTurningAngle = -1.0;
    settings.Symmetry = "Y"; // Symmetry with respect to the Y plane
    settings.FarDist = -1.0;
    settings.NumWakeNodes = -1;
    settings.WakeIters = 5;
    settings.KT = "N"; // Karman-Tsien correction
    settings.StabilityType = 0;

    vspaero.writeSettings(settings);
    std::cout << "   - Impostazioni aerodinamiche scritte" << std::endl;

    // // --- ROTORI (propellers) ---
    settings.IncludePropToAnlysis = "No";
    // settings.rpm = {2400, 0, 2400};     // Only 1st and 3rd active
    // settings.xloc = {4.0, 0.0, 4.0};
    // settings.yloc = {-2.8, 0.0, 2.8};
    // settings.zloc = {-0.5, 0.0, -0.5};
    // settings.xdir = {1.0, 0.0, 1.0};
    // settings.ydir = {0.0, 0.0, 0.0};
    // settings.zdir = {0.0, 0.0, 0.0};
    // settings.dia = {2.2, 0.0, 2.2};
    // settings.hub = {0.25, 0.0, 0.25};
    // settings.ct = {0.12, 0.0, 0.12};
    // settings.cp = {0.06, 0.0, 0.06};

    vspaero.writeRotors(settings, "P"); // Config = propeller

    auto controls = VSP::ControlSurfaceBuilder::buildControlSurfaces(ac, settings.Symmetry);
    vspaero.writeControlSurfaces(controls);
    std::cout << "   - Control surfaces configured (" << controls.size() << " groups)" << std::endl;

    for (const auto &ctrl : controls)
    {
        std::cout << "     * " << ctrl.name << ": " << ctrl.surfaces.size()
                  << " surfaces, deflection = " << ctrl.deflection << "°" << std::endl;
    }

    vspaero.writeFooter(settings);
    std::cout << "\n ✓ VSPAERO file generated: MyAircraft_DegenGeom.vspaero" << std::endl;
    // // close file .VSPAERO mandatory
    vspaero.close();

    // // ==================== CURRENT DIRECTORY CHECK ====================
    // std::cout << "Current directory: " << std::filesystem::current_path() << std::endl;
    try
    {
        // ==================== VSPScript execution ====================

        std::string exeVSPScript = "vspscript.exe -script P2012.vspscript";
        std::cout << "\nExecuting: " << exeVSPScript << std::endl;

        int retVSPScript = system(exeVSPScript.c_str());

        if (retVSPScript != 0 && !std::filesystem::exists("P2012.vsp3"))
        {
            std::cerr << "VSPScript: execution failed" << std::endl;
            return 1;
        }

        // ==================== OPTIONAL: RUN VSPAERO ANALYSIS ====================
        // Uncomment the following lines to run VSPAero analysis
        // std::string exeVSPAero = "vspaero.exe -omp 4 MyAircraft_DegenGeom";
        // std::cout << "\nExecuting: " << exeVSPAero << std::endl;
        // int ret = system(exeVSPAero.c_str());
        // if (ret != 0)
        // {
        //     std::cerr << "VSPAERO: execution failed" << std::endl;
        //     return 1;
        // }
    }

    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    // ==================== OPTIONAL: LOAD AND PLOT AERODYNAMIC DATA ====================
    // Uncomment to load and visualize VSPAero results

    // VSPAero::LODLoader loader("MyAircraft_DegenGeom.lod");
    // Plot plotter("Y [m]", "Cl", "Cl distribution along the span");

    // for (size_t i = 0; i < settings.AoA.size(); i++)
    // {
    //     auto case0 = loader.loadCase(settings.AoA[i]);
    //
    //     std::cout << "\n--- WING (AoA = " << settings.AoA[i] << " deg) ---" << std::endl;
    //     std::cout << "Sections: " << case0.wingSections.size() << std::endl;
    //     std::cout << "  Yavg [m]   Cl      Cd      Cmz" << std::endl;
    //     std::cout << "  ------------------------------------" << std::endl;
    //
    //     std::vector<double> Yavg, Cl;
    //
    //     for (const auto &sec : case0.wingSections)
    //     {
    //         std::cout << std::fixed << std::setprecision(4)
    //                   << "  " << std::setw(8) << sec.Yavg
    //                   << " " << std::setw(7) << sec.Cl
    //                   << " " << std::setw(7) << sec.Cd
    //                   << " " << std::setw(7) << sec.Cmz << std::endl;
    //
    //         Yavg.push_back(sec.Yavg);
    //         Cl.push_back(sec.Cl);
    //     }
    //
    //     std::vector<std::string> colors = {"red", "blue", "green"};
    //     std::string color = colors[i % colors.size()];
    //     std::ostringstream oss;
    //     oss << "AoA = " << std::fixed << std::setprecision(3) << settings.AoA[i] << "°";
    //
    //     plotter.addData(Yavg, Cl, oss.str(), "lines", "1", "1", "", "", color);
    // }
    //
    // plotter.show();

    // ==================== OPTIONAL: READ POLAR DATA ====================
    // Uncomment to read and display polar data

    // VSPPolar::PolarReader reader;
    // reader.readFile("MyAircraft_DegenGeom.polar");
    // const auto &polarData = reader.getData();
    // const auto &headers = reader.getHeaders();
    //
    // std::cout << "\n--- POLAR DATA ---" << std::endl;
    // for (const auto &point : polarData)
    // {
    //     std::cout << "AoA: " << point.AoA
    //               << " | CL: " << point.CL
    //               << " | CDtot: " << point.CDtot << std::endl;
    // }

    // try
    // {
    //     // ==================== WETTED AREA CALCULATION ====================
    //     WETTEDAREA::WettedArea wa("GetGeomOfAircraft.vspscript");

    //     // Calculate wetted area for all components (fuselage, nacelles, boom if present)
    //     wa.getAllGeoms("P2012", "GetGeomOfAircraft.vspscript");

    //     // Access and display results
    //     const auto &results = wa.getWettedAreaResults();

    //     std::cout << "\n=== WETTED AREA RESULTS ===" << std::endl;
    //     std::cout << "WET_FUSE_AREA: " << results.WET_FUSE_AREA << " m²" << std::endl;
    //     std::cout << "WET_NAC_AREA: " << results.WET_NAC_AREA << " m²" << std::endl;
    //     std::cout << "WET_BOOM_AREA: " << results.WET_BOOM_AREA << " m²" << std::endl;

    //     // Convert wing ID to uppercase for display
    //     std::string wingIdUpper = wing.id;
    //     std::transform(wingIdUpper.begin(), wingIdUpper.end(), wingIdUpper.begin(),
    //                    [](unsigned char c)
    //                    { return std::toupper(c); });

    //     std::cout << "WET_" << wingIdUpper << "_AREA: " << results.WET_AREA << " m²" << std::endl;

    //     // ==================== WEIGHT ESTIMATION ====================
    //     Weight weight;

    //     // Calculate dive speed
    //     double diveSpeed = 1.25 * settings.Mach * speedOfSound;

    //     // Calculate horizontal tail MAC and position
    //     double taperRatioHor = horizontal.ctip[0] / horizontal.croot[0];
    //     double horMAC = 0.67 * horizontal.croot[0] * ((1 + taperRatioHor + std::pow(taperRatioHor, 2)) / (1 + taperRatioHor));
    //     double yMACHor = 2 * horizontal.span[0] / 6 * (1 + 2 * taperRatioHor) / (1 + taperRatioHor);
    //     double deltaXLEHor = yMACHor * tan(horizontal.sweep[0] / 57.3);
    //     double xMACHor = horizontal.xloc + deltaXLEHor + 0.25 * horMAC;

    //     // Calculate tail arm (distance between wing and tail aerodynamic centers)
    //     double tailArm = xMACHor - (wing.xloc + 0.25 * settings.Cref);

    //     // // Estimate fuselage weight using Torenbeek method
    //     // double weightFuselage = weight.fuselgeWeightTorenbeekMaterial(
    //     //     diveSpeed, tailArm, fus.diameter, fus.diameter, results.WET_FUSE_AREA,
    //     //     false, EnginePosition::WING_MOUNTED, UndercarriagePosition::BODY_MOUNTED,
    //     //     2.5, false, Material::ALLUMINIUM, Material::ALLUMINIUM, Material::ALLUMINIUM);

    //     // std::cout << "\n=== FUSELAGE WEIGHT ESTIMATION ===" << std::endl;
    //     // std::cout << "Fuselage Weight (Torenbeek method): " << weightFuselage << " kg" << std::endl;
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Error: " << e.what() << std::endl;
    //     return 1;
    // }

    // ==================== COG CALCULATION ====================

    BuildAircraft builder("P2012", settings);

    builder.buildAircraft();

    AircraftBuildData data = builder.getBuildData();

    COG::COGCalculator calcCenterOfGravity("P2012",
                                           data.commonData,
                                           data.wingData,
                                           data.fuselageData,
                                           data.engineData,
                                           wing,
                                           horizontal,
                                           vertical,
                                           fus,
                                           nac);

    calcCenterOfGravity.getWeights();
    calcCenterOfGravity.calculateCOGAircraft();

    COG::Weights weightData = calcCenterOfGravity.getWeightsData();
    COG::COGDATA cogData = calcCenterOfGravity.getCOGData();
    COG::COGDATA cogComponents = calcCenterOfGravity.getCOGComponentsData();

    // double OEW = weightData.totalAircraftWeight -  weightData.payloadWeight - weightData.fuelWeight;
    // double emptyWeight = OEW - 0.05*weightData.fuelWeight - weightData.crewWeight;
    // double structuralMass = weightData.wingWeight + weightData.horizontalWeight +
    //                         weightData.verticalWeight + weightData.fuselageWeight +
    //                         weightData.landingGearWeight + weightData.controlSurfacesWeight;

    // double manifacturerEmptyWeight = structuralMass + weightData.propulsionGroupWeight + weightData.avionicGroupWeight +
    //                                  weightData.electricalGroupWeight + weightData.hydraulicAndPneumaticWeight +
    //                                  weightData.furnishingsAndEquipmentWeight + weightData.airConditioningAndAntiIceWeight;

    // std::cout << "\n=== BALANCE AND WEIGHTS DATA ===" << std::endl;
    // // std::cout << "Fuselage Weight: " << weightData.fuselageWeight << " kg" << std::endl;
    // std::cout << "Total aircraft weight: " << weightData.totalAircraftWeight << " kg" << std::endl;
    // std::cout << "Payload weight: " << weightData.payloadWeight << " kg" << std::endl;
    // std::cout << "Fuel weight: " << weightData.fuelWeight << " kg" << std::endl;
    // std::cout << "Crew weight: " << weightData.crewWeight << " kg" << std::endl;
    // std::cout << "Operating Empty Weight (OEW): " << OEW << " kg" << std::endl;
    // std::cout << "Empty Weight: " << emptyWeight << " kg" << std::endl;
    // std::cout << "Structural mass: " << structuralMass << " kg" << std::endl;
    // std::cout << "Manufacturer Empty Weight: " << manifacturerEmptyWeight << " kg" << std::endl;
    // std::cout << "Xcg: " << cogData.xCG << " m" << std::endl;
    // std::cout << "Ycg: " << cogData.yCG << " m" << std::endl;
    // std::cout << "Zcg: " << cogData.zCG << " m" << std::endl;

    // ====================== XML PARSER TEST ====================

    // XMLUtil::XMLParser parser (std::filesystem::current_path().string() + "/P2012_weightsSettings.xml");

    // BaseWeightData baseData;

    // baseData.setWTO(parser.getValue<double>("myXMLDataToWeights/generalData/maximumTakeOffWeight"));
    // // baseData.setPayloadWeight(720);
    // // baseData.setCrewWeight(76.51);
    // // baseData.setFuelWeight(694);
    // // baseData.setDiveSpeed(111);
    // // baseData.

    LONGITUDINAL_STABILITY::LongitudinalStabilityCalculator stabilityCalc(builder,
                                                                          cogData,
                                                                          ac,
                                                                          settings,
                                                                          wing,
                                                                          horizontal,
                                                                          fus,
                                                                          nac,
                                                                          disk);

    stabilityCalc.calculateLongitudinalStability();

    // AR = 4.0
    std::vector<double> x_AR4 = {1.987690, 1.882740, 1.774640, 1.618870, 1.488560, 1.392180,
                                 1.254540, 1.178300, 1.091490, 1.023750, 0.960280, 0.882022,
                                 0.805917, 0.738358, 0.668714, 0.618095, 0.563305, 0.516943,
                                 0.464390, 0.422469, 0.387092, 0.355955, 0.331126, 0.302226,
                                 0.283739, 0.271578, 0.261451};
    std::vector<double> y_AR4 = {0.050866, 0.048525, 0.049383, 0.053791, 0.061171, 0.069339,
                                 0.087354, 0.096420, 0.109800, 0.120915, 0.136225, 0.157999,
                                 0.183987, 0.218368, 0.256997, 0.289128, 0.329754, 0.363968,
                                 0.419383, 0.478945, 0.561724, 0.644469, 0.720818, 0.818352,
                                 0.892536, 0.962438, 1.021750};

    // AR = 6.0
    std::vector<double> x_AR6 = {1.990130, 1.913810, 1.848090, 1.792470, 1.708250, 1.624030,
                                 1.538210, 1.474690, 1.400020, 1.326960, 1.255480, 1.191990,
                                 1.112620, 1.036470, 0.949220, 0.874730, 0.816087, 0.762289,
                                 0.721171, 0.664194, 0.610485, 0.577303, 0.539452, 0.492178,
                                 0.444966, 0.410536, 0.377657, 0.343454, 0.318751, 0.291008,
                                 0.271138, 0.254182};
    std::vector<double> y_AR6 = {0.092095, 0.090585, 0.088992, 0.092077, 0.097504, 0.102932,
                                 0.106786, 0.115222, 0.122160, 0.130673, 0.139172, 0.152368,
                                 0.167276, 0.186918, 0.211407, 0.240555, 0.263232, 0.295388,
                                 0.322685, 0.354867, 0.398129, 0.423776, 0.462152, 0.514882,
                                 0.575543, 0.644036, 0.707757, 0.804805, 0.897018, 1.006710,
                                 1.106810, 1.173580};

    // AR = 9.0
    std::vector<double> x_AR9 = {1.978750, 1.913580, 1.842040, 1.762560, 1.662440, 1.551230,
                                 1.447980, 1.351100, 1.247900, 1.166990, 1.094010, 1.011530,
                                 0.929097, 0.851491, 0.778604, 0.701086, 0.639517, 0.577934,
                                 0.525979, 0.482010, 0.434975, 0.403750, 0.370997, 0.347771,
                                 0.318248, 0.295022, 0.275038, 0.256643};
    std::vector<double> y_AR9 = {0.127089, 0.127606, 0.128173, 0.130390, 0.134357, 0.144758,
                                 0.155097, 0.168557, 0.185241, 0.206507, 0.226124, 0.250576,
                                 0.279787, 0.316892, 0.347613, 0.395823, 0.450253, 0.503096,
                                 0.566968, 0.635536, 0.718408, 0.790048, 0.869633, 0.947555,
                                 1.033460, 1.111380, 1.197210, 1.283030};

    // AR = 12.0
    std::vector<double> x_AR12 = {1.985290, 1.910570, 1.808870, 1.689720, 1.592820, 1.473690,
                                  1.384750, 1.295880, 1.199070, 1.116580, 1.051570, 0.988131,
                                  0.919988, 0.851845, 0.780598, 0.723658, 0.668384, 0.613199,
                                  0.554859, 0.502967, 0.462216, 0.426157, 0.396660, 0.367087,
                                  0.336013, 0.314452, 0.285081, 0.272894, 0.254563};
    std::vector<double> y_AR12 = {0.149248, 0.149840, 0.155407, 0.165871, 0.176159, 0.189796,
                                  0.201607, 0.221350, 0.242742, 0.265607, 0.286748, 0.306289,
                                  0.333800, 0.361311, 0.398366, 0.435307, 0.481754, 0.539306,
                                  0.600056, 0.671860, 0.745162, 0.808908, 0.897986, 0.977546,
                                  1.068220, 1.155650, 1.260590, 1.327320, 1.421070};

    // Initialize interpolator with power regression method
    Interpolant2D upwashInterpolator(1, RegressionMethod::POWER);

    // Add all curves
    upwashInterpolator.addCurve(4.0, x_AR4, y_AR4);
    upwashInterpolator.addCurve(6.0, x_AR6, y_AR6);
    upwashInterpolator.addCurve(9.0, x_AR9, y_AR9);
    upwashInterpolator.addCurve(12.0, x_AR12, y_AR12);

    double downWashGradientOnWingDueToCanard = upwashInterpolator.interpolate(0.8, 9.0);

    std::cout << "========================================" << std::endl;
    std::cout << "  AIRCRAFT GEOMETRY EXTRACTION TOOL" << std::endl;
    std::cout << "========================================\n"
              << std::endl;

    // ==================== STEP 1: Estrai tutte le geometrie ====================
    std::cout << "[STEP 1] Extracting all aircraft geometries..." << std::endl;

    VSPGEOMTRYEXTRACTOR::GeometryExtractor geomExtractor;
    geomExtractor.extractAllGeoms("P2012", "P2012_AllGeoms.vspscript");

    VSPGEOMTRYEXTRACTOR::AircraftGeometryData allGeomData = geomExtractor.getGeometryData();

    

    std::cout << "✓ Found " << allGeomData.allGeoms.size() << " geometries\n"
              << std::endl;

    // ==================== STEP 2: Estrai diametri fusoliera ====================
    std::cout << "[STEP 2] Extracting fuselage diameters and positions..." << std::endl;

    VSPGEOMTRYEXTRACTOR::DiametersExtractor fuseExtractor;

    VSPGEOMTRYEXTRACTOR::FuselageDiametersAndXStation fuseData = fuseExtractor.extractFuselageDiameters(
        "P2012",
        allGeomData,
        fus.length);

    VSPGEOMTRYEXTRACTOR::DiametersExtractor nacelleExctractor;
    VSPGEOMTRYEXTRACTOR::NacelleDiametersAndXStation nacelleData = nacelleExctractor.extractNacelleDiameters(
        "P2012",
        allGeomData,
        nac.length);

    // ==================== FUSELAGE SECTION DATA ====================
    std::cout << "=== Fuselage Sections Data ===" << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    std::cout << std::left << std::setw(12) << "Section"
              << std::setw(18) << "Diameter (m)"
              << std::setw(18) << "X Position (m)"
              << "% Length" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    for (size_t i = 0; i < fuseData.allFuselageWidth.size(); i++)
    {
        double percentLength = (fuseData.xStation[i] / fus.length) * 100.0;

        std::cout << std::left << std::setw(12) << i
                  << std::setw(18) << fuseData.allFuselageWidth[i]
                  << std::setw(18) << fuseData.xStation[i]
                  << std::setprecision(1) << percentLength << "%" << std::endl;
    }

    // ==================== NACELLE SECTION DATA ====================
    std::cout << "\n=== Nacelle Sections Data ===" << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    std::cout << std::left << std::setw(12) << "Section"
              << std::setw(18) << "Diameter (m)"
              << std::setw(18) << "X Position (m)"
              << "% Length" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    for (size_t i = 0; i < nacelleData.allNacelleWidth.size(); i++)
    {
        double percentLength = (nacelleData.xStation[i] / nac.length) * 100.0;

        std::cout << std::left << std::setw(12) << i
                  << std::setw(18) << nacelleData.allNacelleWidth[i]
                  << std::setw(18) << nacelleData.xStation[i]
                  << std::setprecision(1) << percentLength << "%" << std::endl;
    }

    // ==================== LONGITUDINAL DERIVATIVES TEST ====================

    std::cout << "\n=== LONGITUDINAL AIRCRAFT DERIVATIVES TEST ===" << std::endl;
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "deltaCmDeltaAlphaAircraft = " << stabilityCalc.getTotalAircrfatLongitudinalDerivatives().deltaCmDeltaAlphaAircraft << std::endl;
    std::cout << "deltaCmDeltaClAircraft = " << stabilityCalc.getTotalAircrfatLongitudinalDerivatives().deltaCmDeltaClAircraft << std::endl;

    std::cout << "\n=== LONGITUDINAL COMPONENTS DERIVATIVES TEST ===" << std::endl;
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "deltaCmDeltaAlphaWing = " << stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent().deltaCmDeltaAlphaWing << std::endl;
    std::cout << "deltaCmDeltaAlphaHorizontalTail = " << stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent().deltaCmDeltaAlphaHorizontalTail << std::endl;
    std::cout << "deltaCmDeltaAlphaFuselage = " << stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent().deltaCmDeltaAlphaFuselage << std::endl;
    std::cout << "deltaCmDeltaAlphaNacelle = " << stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent().deltaCmDeltaAlphaNacelle << std::endl;
    std::cout << "deltaCmDeltaClWing = " << stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent().deltaCmClWingContribution << std::endl;
    std::cout << "deltaCmDeltaClHorizontalTail = " << stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent().deltaCmClHorizontalTailContribution << std::endl;
    std::cout << "deltaCmDeltaClFuselage = " << stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent().deltaCmClFuselageContribution << std::endl;
    std::cout << "deltaCmDeltaClNacelle = " << stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent().deltaCmClNacelleContribution << std::endl;

    // ==================== DIRECTIONAL STABILITY TEST ====================

    DIRECTIONAL_STABILITY::DirectionalStabilityCalculator directionalStabilityCalc(builder,
                                                                                   cogData,
                                                                                   settings,
                                                                                   wing,
                                                                                   horizontal,
                                                                                   fus,
                                                                                   nac,
                                                                                   vertical);

    directionalStabilityCalc.calculateDirectionalStabilityDerivatives();


    // // ==================== TEST SILENTOR COMPONENT ====================

    // SILENTORCOMPONENT::SilentorComponent silentor("P2012");

    // silentor.GetGeometryWithSilentComponent("P2012_silent_componets.vspscript", ac, allGeomData, {"wing","TransportFuse"});

    // silentor.executeAnalysis(settings);

    // std::vector<double> liftCoefficienttWingFuselage = silentor.getAerodynamicCoefficients().liftCoefficient;


    // ==================== LATERAL STABILITY TEST ====================

    LATERAL_STABILITY::LateralStabilityCalculator lateralStabilityCalc(builder,
                                                                       ac,
                                                                       cogData,
                                                                       settings,
                                                                       wing,
                                                                       horizontal,
                                                                       vertical,
                                                                       fus,
                                                                       nac,
                                                                       directionalStabilityCalc.getSingleComponentsDerivativesSideForce());

    lateralStabilityCalc.calculateLateralStabilityDerivatives();


    // ==================== TEST EXCEL WRITER====================

    StaticDerivativeExcelWriter excelWriter;

    excelWriter.writeDerivativesToExcel("TestExcelDerivatives.xlsx",
                                        "P2012",
                                        settings,
                                        stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent(),
                                        directionalStabilityCalc.getSingleComponentsDerivativesSideForce(),
                                        directionalStabilityCalc.getSingleComponentsDerivativesYaw(),
                                        lateralStabilityCalc.getComponentsLateralStabilityDerivativesRoll(),
                                        stabilityCalc.getTotalAircrfatLongitudinalDerivatives(),
                                        directionalStabilityCalc.getAircraftDirectionalDerivatives(),
                                        lateralStabilityCalc.getAircraftLateralStabilityDerivativesRoll());





    // ==================== TEST TRIM CONDITION ==========================

    double liftCoefficientAtZeroAoA  = stabilityCalc.getLongitudinalAerodynamicCoefficients().liftCoefficientAtZeroAoA;
    double pitchingMomentAtZeroAoA = stabilityCalc.getLongitudinalAerodynamicCoefficients().pitchingMomentAtZeroAoA;

    double aircraftLiftSlope = stabilityCalc.getTotalAircrfatLongitudinalDerivatives().liftSlopeAircraft;
    double pitchingMomentSlopeAircraft = stabilityCalc.getTotalAircrfatLongitudinalDerivatives().deltaCmDeltaAlphaAircraft;

    double deltaCLdeltaHorIncidence = stabilityCalc.getTotalAircrfatLongitudinalDerivatives().deltaCLDeltaIncidenceHorizontalTail;
    double deltaCmdeltaHorIncidence = stabilityCalc.getTotalAircrfatLongitudinalDerivatives().deltaCmDeltaIncidenceHorizontalTail;

    double deltaCLDeltaHorDeflection = stabilityCalc.getTotalAircrfatLongitudinalDerivatives().deltaCLDeltaElevatorDeflection;
    double elevetorControlPower = stabilityCalc.getTotalAircrfatLongitudinalDerivatives().deltaCmDeltaElevatorDeflection;




    Eigen::MatrixXd matrixA(2, 2);
    matrixA << pitchingMomentSlopeAircraft, elevetorControlPower,
               aircraftLiftSlope, deltaCLDeltaHorDeflection;

    Eigen::VectorXd vectorB(2);
    vectorB << -(pitchingMomentAtZeroAoA + deltaCmdeltaHorIncidence*horizontal.yrot), 
                 -(liftCoefficientAtZeroAoA + deltaCLdeltaHorIncidence*horizontal.yrot);

    Eigen::VectorXd trimSolution = matrixA.colPivHouseholderQr().solve(vectorB);

    std::cout << "\n=== TRIM CONDITION SOLUTION ===" << std::endl;
    std::cout << "Trim angle of attack: " << trimSolution(0) << " degrees" << std::endl;
    std::cout << "Trim elevator deflection: " << trimSolution(1) << " degrees" << std::endl;

    double deltaH = 0.0;
    double vectorLength = 0.0;
    double numberOfIntervalsForChart = 100;
    std::vector<double> alphaAngle = {trimSolution(0),10.0};
    double minVale = *std::min_element(alphaAngle.begin(), alphaAngle.end());
    double maxVale = *std::max_element(alphaAngle.begin(), alphaAngle.end());

    std::vector<double> amplifiedAlphaValuesVector;

    std::vector<double> deltaE;
    std::vector<double> CLe;
    // Uso std::max_element/std::min_element per ottenere gli iteratori al massimo e minimo di xValues,
    // li dereferenzio con * per accedere ai valori double e ne faccio la differenza per ricavare l’intervallo complessivo
    vectorLength = *std::max_element(alphaAngle.begin(), alphaAngle.end()) - *std::min_element(alphaAngle.begin(), alphaAngle.end());

    deltaH = vectorLength / numberOfIntervalsForChart;

    for (double i = minVale; i <= maxVale; i += deltaH)
    {

        amplifiedAlphaValuesVector.emplace_back(i); // Uso l'indice i così le coppie (xLinear, yLinear) corrispondono ai punti della retta nel dominio originale.

        deltaE.push_back(-((pitchingMomentAtZeroAoA + deltaCmdeltaHorIncidence*horizontal.yrot + pitchingMomentSlopeAircraft*i) / (elevetorControlPower)));
        CLe.push_back(liftCoefficientAtZeroAoA + aircraftLiftSlope*i + deltaCLdeltaHorIncidence*horizontal.yrot + deltaCLDeltaHorDeflection*deltaE.back());
 
    }

    // ========================================================================
    //     Optional: Plot downwash angle vs angle of attack (currently commented)
    //     ========================================================================
    Plot plot(CLe, deltaE,
              "CL (-)",
              "Elevator deflection (deg)",
              "Trim - condition",
              "deltaE vs CLe",
              "lines", "1", "1", "", "", "blue");

    // ==================== SAVE OUTPUT FILES ====================
    SaveFiles saveFiles;

    std::cout << "\n=== SAVING FILES ===" << std::endl;
    saveFiles.saveFiles("MyAircraft_test_folder", "P2012");
    std::cout << "✓ Files saved successfully" << std::endl;

    return 0;
}

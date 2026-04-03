#define _HAS_STD_BYTE 0 // Risolve conflitto byte tra C++17 e Windows headers
#define UNICODE         // Assicura che Windows usi le versioni wide delle strutture
#define _UNICODE

#include <vtkAOSDataArrayTemplate.h>
template class vtkAOSDataArrayTemplate<int>;
template class vtkAOSDataArrayTemplate<long long>;

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
#include "DerivativesExcelWriter.h"
#include "MomentOfInertiaCalculator.h"
#include "ChordCalculator.h"
#include "DegenGeomParser.h"
#include "Plotter3D.h"
#include "FuelMassCalculatorBreguet.h"
#include "AircraftAlphaZeroAngleCalculator.h"
#include "CDCalculator.h"
#include "PropellerEfficiencyCalculator.h"
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

    std::string aircraftName = "P2012";
    // ====================  SCRIPT GENERATION ====================

    SaveFiles saveFiles;

    saveFiles.captureExecutionTimestamp(); // Capture the timestamp before script generation

    std::cout << "\n1. Generating AngelScript script (.vspscript)..." << std::endl;

    VSP::ScriptGenerator script(aircraftName + ".vspscript");

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
    wing.mov.defl = {0, 0};

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
    horizontal.mov.defl = {0};

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
    vertical.mov.defl = {0};
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
    script.makeDegenGeom(aircraftName, false);
    std::cout << "   - DegenGeom configured" << std::endl;

    std::cout << "\n✓ AngelScript script generated: " << aircraftName << ".vspscript" << std::endl;

    // ==================== GENERATE VSPAERO FILE ====================
    std::cout << "\n2. Generating VSPAERO file (.vspaero)..." << std::endl;

    VSP::VSPAeroGenerator vspaero(aircraftName);

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
    settings.Mach = 0.27;                                                // Mach number
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

        std::string exeVSPScript = "vspscript.exe -script " + aircraftName + ".vspscript";
        std::cout << "\nExecuting: " << exeVSPScript << std::endl;

        int retVSPScript = system(exeVSPScript.c_str());

        if (retVSPScript != 0 && !std::filesystem::exists(aircraftName + ".vsp3"))
        {
            std::cerr << "VSPScript: execution failed" << std::endl;
            return 1;
        }

        // ==================== OPTIONAL: RUN VSPAERO ANALYSIS ====================
        // Uncomment the following lines to run VSPAero analysis
        // std::string exeVSPAero = "vspaero.exe -omp 4 " + aircraftName + "_DegenGeom";
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

    // ==================== 3D - AIRCRAFT REPRESENTATION TEST ====================

    // Leggi il file DegenGeom
    DegenGeomReader reader(aircraftName + "_DegenGeom.csv");
    auto components = reader.read();

    // Crea il plotter
    AircraftPlotter plotter(aircraftName);
    plotter.setResolution(2560, 1440);
    plotter.setBackground(12, 116, 228);
    std::map<std::string, std::array<double, 3>> colorMap = {
        {wing.id, {0, 0, 255}},
        {horizontal.id, {0, 255, 0}},
        {vertical.id, {255, 0, 255}},
        {fus.id, {180, 180, 180}},
        {nac.id, {100, 200, 255}}, // matcha nacelle_1 e nacelle_2
        {disk.id, {80, 80, 80}}};

    for (const auto &surf : components)
    {
        plotter.addComponentWithColorMap(surf, colorMap);
    }

    // // Mostra la finestra 3D interattiva
    // plotter.show();

    // plotter.saveAllViews(std::filesystem::current_path().string());

    // ==================== COG CALCULATION AND DRAG CALCULATION ====================

    BuildAircraft builder("P2012", settings);

    builder.buildAircraft();

    AircraftBuildData data = builder.getBuildData();

    COG::COGCalculator calcCenterOfGravity("P2012",
                                           data.commonData,
                                           builder,
                                           data.wingData,
                                           data.fuselageData,
                                           data.engineData,
                                           ac,
                                           wing,
                                           horizontal,
                                           vertical,
                                           fus,
                                           nac);

    // std::vector<double> liftCoefficients;
    std::vector<double> dragCoefficients;
    // std::vector<double> efficiencyAircraft;

    // CalculateAircrfatAlphaZeroLiftAngle alphaZeroLiftAngleCalc(builder, ac, settings, wing, horizontal, vertical, fus, nac);

    // double aircrfatalphaZeroLiftAngle = alphaZeroLiftAngleCalc.getAircraftAlphaZeroLiftAngle();

    // for (double AoA = aircrfatalphaZeroLiftAngle; AoA <= 10; AoA += 1.0)
    // {

    //     dragCoefficients.push_back(cdCalc.getCDTotalAircraft(AoA, settings.Mach, settings.altitude, "CRUISE"));
    //     liftCoefficients.push_back(cdCalc.getLiftCoefficient());

    //     efficiencyAircraft.push_back(liftCoefficients.back() / dragCoefficients.back());
    // }

    // Plot plotDragPolar(dragCoefficients, liftCoefficients,
    //                    "CD (-)",
    //                    "CL (-)",
    //                    "Drag Polar",
    //                    "DragP, M = 0.27, Alt = 10000 ft",
    //                    "lines", "1", "1", "", "", "blue");

    // Plot plotEfficinency(liftCoefficients, efficiencyAircraft,
    //                      "CL (-)",
    //                      "L/D (-)",
    //                      "Efficiency",
    //                      "L/D, M = 0.25, Alt = 10000 ft",
    //                      "lines", "1", "1", "", "", "orange");

    // =========== POWER CALCULATION AND PLOT POWER VS TAS ===========

    RestoreSettings settingsRestorer; // Create an instance of RestoreSettings to automatically restore settings when going out of scope

    settingsRestorer.setSavePrevoiusSettings(settings);

    double criticalAltitude = builder.getEngineData().getCriticalAltitude();

    ConvLength convLength(Length::FT, Length::M, criticalAltitude);

    criticalAltitude = convLength.getConvertedValues(); // Convert critical altitude to meters for fuel calculation

    double availablePower = 0.0;
    double throttleSetting = 1.0; // Assuming 75% throttle for cruise to evaluate max speed
    double etaPropeller = 0.8;    // Estimated propeller efficiency

    std::vector<double> machNumbers = {0.2, 0.25, 0.27, 0.3, 0.35};                     // Different Mach numbers to evaluate
    std::vector<double> altitudeEvaluations = {0.0, 5000.0, 10000.0, 15000.0, 20000.0}; // Different altitudes to evaluate

    ConvLength convAltitudesToEvaluate(Length::FT, Length::M, altitudeEvaluations);

    altitudeEvaluations = convAltitudesToEvaluate.getConvertedValues(); // Convert altitudes to meters for power calculation

    std::vector<double> requiredPower;
    std::vector<double> availablePowerVec;
    // std::vector<double> TAS;
    std::vector<double> ROC;

    Eigen::MatrixXd powerRequiredMatrix(machNumbers.size(), altitudeEvaluations.size()); // Matrix to store power values for different Mach numbers and altitudes
    Eigen::MatrixXd powerAvailableMatrix(machNumbers.size(), altitudeEvaluations.size()); // Matrix to store power values for different Mach numbers and altitudes
    Eigen::MatrixXd TAS(machNumbers.size(), altitudeEvaluations.size()); // Matrix to store true airspeed values for different Mach numbers and altitudes
   
    double propellerEfficiency = 0.0;

    for (size_t i = 0; i < altitudeEvaluations.size(); i++)
    {

        double muViscosity = Atmosphere::ISA::viscosity(altitudeEvaluations[i]);                                                                    // Calculate viscosity at sea level for power calculation
        auto [temperatureToEvaluate, speedOfSoundToEvaluate, pressureToEvaluate, densityToEvaluate] = Atmosphere::atmosisa(altitudeEvaluations[i]); // Atmosisa like in MATLAB
        std::cout << "\n--- Evaluating at altitude: " << altitudeEvaluations[i] << " m ---" << std::endl;

       
        
        for (size_t j = 0; j < machNumbers.size(); j++)
        {

            settings.Mach = machNumbers[j];
            settings.rho = densityToEvaluate; // Update air density for current altitude
            settings.ReCref = settings.Mach * speedOfSoundToEvaluate * settings.Cref * settings.rho / muViscosity; // Update Reynolds number for current Mach
            
            PropellerEfficiencyCalculator propEfficiencyCalculator(builder,
                                                                   wing,
                                                                   horizontal,
                                                                   fus,
                                                                   disk,
                                                                   nac,
                                                                   machNumbers[j] * speedOfSoundToEvaluate,
                                                                   40.0,
                                                                   altitudeEvaluations[i]);

            propellerEfficiency = propEfficiencyCalculator.getPropellerEfficiency();

            if (altitudeEvaluations[i] > criticalAltitude)
            {
                // Power in kW
                availablePower = 0.7457 * builder.getEngineData().getNumberOfEngines() * builder.getEngineData().getBSHP() * throttleSetting * propellerEfficiency * (1.0 - (altitudeEvaluations[i] - criticalAltitude) * 0.1 / 1e3);
            }

            else
            {
                // Power in kW
                availablePower = 0.7457 * builder.getEngineData().getNumberOfEngines() * builder.getEngineData().getBSHP() * throttleSetting * propellerEfficiency;
            }

            CDCalculator cdCalc(aircraftName, builder, ac, settings, wing, horizontal, vertical, fus, nac);

            dragCoefficients.push_back(cdCalc.getCDTotalAircraft(2, machNumbers[j], altitudeEvaluations[i], "CRUISE"));

            double tempRequiredPower = 0.5 * settings.rho * std::pow(machNumbers[j] * speedOfSoundToEvaluate, 3) * settings.Sref * dragCoefficients.back() / 1000.0; // Power in kW

            powerRequiredMatrix(i,j) = tempRequiredPower;
            powerAvailableMatrix(i,j) = availablePower;


            TAS(i,j) = machNumbers[j] * speedOfSoundToEvaluate;

           
            // ROC.push_back(1000.0 * (availablePowerVec.back() - requiredPower.back()) / (9.81 * builder.getCommonData().getWTO())); // Rate of climb in m/s

            // ConvVel rocConverter(Speed::M_TO_S, Speed::FT_TO_MIN, ROC.back()); // Converter for rate of climb from m/s to ft/min

            // ROC.back() = rocConverter.getConvertedValues(); // Update ROC with converted value in ft/min
        }
    }

    Plot plotPower("TAS(m/s)", "Power(kW)", "Power vs TAS");
    // Colori per la potenza RICHIESTA (toni freddi/scuri)
    std::vector<std::string> colorsPowerRequired = {
        "#0072BD", // blu MATLAB
        "#7E2F8E", // viola
        "#005F73", // petrolio
        "#0A9396", // ciano scuro
        "#001219"  // blu notte
    };

    // Colori per la potenza DISPONIBILE (toni caldi)
    std::vector<std::string> colorsPowerAvailable = {
        "#D95319", // arancione MATLAB
        "#EDB120", // giallo ocra
        "#A2142F", // rosso scuro MATLAB
        "#77AC30", // verde MATLAB
        "#E76F51"  // corallo
    };

    for (size_t n = 0; n < altitudeEvaluations.size(); n++)
    {
         
         plotPower.addData(TAS.row(n), powerRequiredMatrix.row(n), "Required Power", "lines", "1", "1", "", "", colorsPowerRequired[n]);
         plotPower.addData(TAS.row(n), powerAvailableMatrix.row(n), "Available Power", "lines", "1", "1", "", "", colorsPowerAvailable[n]);
      
    }
   
    

    plotPower.show();

    // Plot rocPlot(TAS, ROC,
    //              "TAS (m/s)",
    //              "Rate of Climb (ft/min)",
    //              "Rate of Climb vs TAS",
    //              "ROC, M = 0.2-0.35, Alt = 10000 ft",
    //              "lines", "1", "1", "", "", "blue");

    settings = settingsRestorer.getSettingsToRestore(); // Restore previous settings after power calculation and plotting

    // ==================== SAVE OUTPUT FILES ====================

    std::cout << "\n=== SAVING FILES ===" << std::endl;
    saveFiles.saveFiles(aircraftName + "_folder", aircraftName);
    std::cout << "✓ Files saved successfully" << std::endl;
    // ==================== FUEL CALCULATION ====================

    // // ==================== SAVE OUTPUT FILES ====================
    // SaveFiles saveFiles;

    // std::cout << "\n=== SAVING FILES ===" << std::endl;
    // saveFiles.saveFiles("MyAircraft_test_folder", aircraftName);
    // std::cout << "✓ Files saved successfully" << std::endl;

    return 0;
}

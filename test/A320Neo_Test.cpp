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

    std::string aircraftName = "A320Neo";

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

    VSP::ScriptGenerator script(aircraftName + ".vspscript");

    // --- CONTROL SURFACES ---
    VSP::Aircraft ac;
    ac.movables = "sfaer"; // Flap + Aileron + Elevator + Rudder

    // ac.ver.id = "vertical";
    // ac.ver.mov.type = {'r'};
    // ac.ver.mov.defl = {5.0};
    // ac.ver.isSplittedDeflection = false;

    // --- MAIN WING ---
    VSP::Wing wing;
    wing.id = "wing";
    ac.wing.id = wing.id;
    wing.typeOfWing = TypeOfWing::CRANKED;
    wing.useDetailedPanels = true;
    wing.airfoilType = "XS_FILE_AIRFOIL";
    wing.affile = {"airfoil/SC_02_0714.dat",
                   "airfoil/SC_02_0714.dat",
                   "airfoil/SC_02_0712.dat",
                   "airfoil/SC_02_0712.dat",
                   "airfoil/SC_02_0712.dat"};
    wing.camber = {0.2, 0.2, 0.2, 0.2, 0.2};
    wing.camberloc = {0.4, 0.4, 0.4, 0.4, 0.4};
    wing.twist = {0, 0,  0,-3.5};
    wing.twistloc = {0, 0, 0, 0};
    // wing.idealcl = {0.3, 0.3, 0.3, 0.3};
    wing.thickchord = {0.14, 0.14, 0.12, 0.12, 0.12};
    wing.xloc = 11.765;
    wing.yloc = 0.0;
    wing.zloc = -0.570;
    wing.xrot = 0;
    wing.yrot = 0;
    wing.zrot = 0;

    wing.span = {6.39327, 10.64270, 0.37267, 2.44506};
    wing.ctip = {3.779, 1.143, 0.884, 0.382};
    wing.croot = {7.25, 3.779, 1.143, 0.884};

    wing.kinkStation = 6.39327; 
    wing.taperInbord = wing.ctip.front() / wing.croot.front();

    wing.sectessU = {15, 15, 15, 15};
    wing.wtess = 25;

    // Control surfaces
    wing.mov.type = {'s', 's', 's', 's', 's', 'f', 'f', 'a'};
    wing.mov.eta_inner = {0.113480, 0.315809, 0.466194, 0.598461, 0.723475, 0.10, 0.324019, 0.693484};
    wing.mov.eta_outer = {0.272845, 0.464194, 0.598461, 0.721475, 0.828508, 0.320019, 0.688484, 0.831076};
    wing.mov.cf_c_inner = {0.110063, 0.14, 0.14, 0.14, 0.14, 0.20, 0.20, 0.286600};
    wing.mov.cf_c_outer = wing.mov.cf_c_inner;
    wing.mov.tessellation = {10, 10, 10, 10, 10, 10, 10, 10};
    wing.mov.defl = {0, 0, 0, 0, 0, 0, 0, 0};

    ac.wing.mov.type = wing.mov.type;
    ac.wing.mov.defl = wing.mov.defl;

    wing.sweep = {28.31, 28.31, 55.34, 39.62};
    wing.sweeploc = {0, 0, 0, 0};
    wing.secsweeploc = {1, 1, 1, 1};
    wing.dihedral = {6.0, 6.0, 24.5, 75.37};

    wing.blending = true;

    wing.blend.InLEMode = {0,0,0,4,0};
    wing.blend.InTEMode = {0,0,0,5,0};
    wing.blend.OutLEMode = {0,0,2,0,0};
    wing.blend.OutTEMode = {0,0,0,0,0};
    wing.blend.InLESweep = {0,0,0,0,0};
    wing.blend.OutLESweep = {0,0,0,0,0};
    wing.blend.InLEDihedral = {0,0,0,0,0};
    wing.blend.OutLEDihedral = {0,0,0,0,0};
    wing.blend.InTESweep = {0,0,0,0,0};
    wing.blend.OutTESweep = {0,0,0,0,0};
    wing.blend.InTEDihedral = {0,0,0,0,0};
    wing.blend.OutTEDihedral = {0,0,0,0,0};
    wing.blend.InLEStrength = {1,1,1,0.75,1};
    wing.blend.InTEStrength = {1,1,1,0.75,1};
    wing.blend.OutLEStrength = {1,1,0.75,1,1};
    wing.blend.OutTEStrength = {1,1,1,1,1};

    script.makeWing(wing, 2,  wing.typeOfWing);
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
    horizontal.camber = {0.2, 0.2};
    horizontal.camberloc = {0.4, 0.4};
    horizontal.thickchord = {0.12, 0.12};
    horizontal.twist = {0,0};
    horizontal.twistloc = {0,0};
    horizontal.xloc = 31.77;
    horizontal.yloc = 0.0;
    horizontal.zloc = 1.089;
    horizontal.xrot = 0;
    horizontal.yrot = 0;
    // horizontal.span = {2.622};
    horizontal.span = {5.97783};
    // horizontal.ctip = {0.698};
    horizontal.ctip = {1.318};
    // horizontal.croot = {1.298};

    horizontal.capTreatment = true;
    horizontal.croot = {4.0, 1.318};
    horizontal.capLength = {0.31468};
    horizontal.capOffset = {0.0};
    horizontal.capStrength = {1.0};
    horizontal.capType = {"Round"};

    horizontal.sweep = {33.040};
    horizontal.sweeploc = {0};
    horizontal.secsweeploc = {1};
    horizontal.dihedral = {8.4};
    horizontal.sectessU = {15};
    horizontal.wtess = {25};
    horizontal.mov.type = {'e'};
    horizontal.mov.eta_inner = {0.190068};
    horizontal.mov.eta_outer = {1.0};
    horizontal.mov.cf_c_inner = {0.28};
    horizontal.mov.cf_c_outer = {0.28};
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
    vertical.camber = {0, 0, 0, 0, 0};
    vertical.camberloc = {0.2, 0.2, 0.2, 0.2, 0.2};
    vertical.thickchord = {0.12, 0.12, 0.12, 0.12, 0.12};
    vertical.twist = {0, 0, 0, 0};
    vertical.twistloc = {0, 0, 0, 0};
    vertical.xloc = 28.304;
    vertical.zloc = 2.787;
    vertical.yloc = 0.0;

    vertical.xrot = 90;
    vertical.yrot = 0;
    vertical.zrot = -2.5;

    vertical.span = {0.469, 0.734, 4.20030, 0.46670};
    vertical.ctip = {5.404, 4.66825, 1.996, 1.615};
    vertical.croot = {7.40267, 5.404, 4.66825, 1.996};
    vertical.sweep = {77.37, 50.560, 40.050, 46.190};
    vertical.sweeploc = {0, 0, 0, 0};
    vertical.secsweeploc = {1, 1, 1, 1};
    vertical.dihedral = {0, 0, 0, 0};

    vertical.blending = true;

    vertical.blend.InLEMode = {0, 4, 0, 0, 0};
    vertical.blend.InTEMode = {0, 0, 0, 0, 0};
    vertical.blend.OutLEMode = {0, 0, 0, 2, 0};
    vertical.blend.OutTEMode = {0, 0, 0, 0, 0};
    vertical.blend.InLESweep = {0, 0, 0, 0, 0};
    vertical.blend.OutLESweep = {0, 0, 0, 0, 0};
    vertical.blend.InLEDihedral = {0, 0, 0, 0, 0};
    vertical.blend.OutLEDihedral = {0, 0, 0, 0, 0};
    vertical.blend.InTESweep = {0, 0, 0, 0, 0};
    vertical.blend.OutTESweep = {0, 0, 0, 0, 0};
    vertical.blend.InTEDihedral = {0, 0, 0, 0, 0};
    vertical.blend.OutTEDihedral = {0, 0, 0, 0, 0};
    vertical.blend.InLEStrength = {1, 0.4, 1, 1, 1};
    vertical.blend.InTEStrength = {1, 1, 1, 1, 1};
    vertical.blend.OutLEStrength = {1, 1, 1, 1.30, 1};
    vertical.blend.OutTEStrength = {1, 1, 1, 1, 1};
    // vertical.sweep = {60, 32.85714};
    // vertical.sweep = {73.84152, 30.12277};
    // vertical.sweeploc = {0, 0};
    // vertical.secsweeploc = {1, 1};
    // vertical.dihedral = {0, 0};
    vertical.sectessU = {15, 15, 15, 15};
    vertical.wtess = {25};
    vertical.mov.type = {'r'};
    vertical.mov.eta_inner = {0.141176};
    vertical.mov.eta_outer = {1.0};
    vertical.mov.cf_c_inner = {0.367212};
    vertical.mov.cf_c_outer = {0.367212};
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
    // fus.type = "TransportFuse";
    fus.advancedFuselage = true;
    fus.fuselagePresetName = "AirbusGenericTransportJet";
    fus.customFuselage = false;

    // fus.length         = 37.57;
    // fus.diameter       = 4.14;
    // fus.width          = 3.95010;
    fus.length = 37.57;
    fus.diameter = 4.141;
    fus.width = 3.95010;

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
    nac.nacellePresetName = "GeneralTurbofan";
    // nac.nacellePresetName = "GeneralTurboProp_Spec2";
    // nac.nacellePresetName = "GeneralTurboProp_Spec1";
    // nac.length = 3.458;
    // nac.diameter = 2.735;
    // nac.xloc = {13.06,13.06};
    // nac.yloc = {5.75,-5.75};
    // nac.zloc = {-1.283,-1.283};
    // nac.length = 2.72821;
    nac.length = 3.458;
    nac.diameter = 2.735;
    nac.xloc = {13.006,13.006};
    nac.yloc = {5.75, -5.75};
    nac.zloc = {-1.283, -1.283};
    nac.xrot = {0.0, 0.0};
    nac.yrot = {0.0, 0.0};
    nac.zrot = {0.0, 0.0};

    // VSP::Disk disk;
    // disk.id = "disk";
    // disk.type = "Disk";
    // disk.diameter = {1.950, 1.950};
    // disk.xloc = {3.443, 3.443};
    // disk.yloc = {2.295, -2.295};
    // disk.zloc = {0.820, 0.820};
    // disk.xrot = {0.0, 0.0};
    // disk.yrot = {0.0, 0.0};
    // disk.zrot = {0.0, 0.0};
    // disk.utess = {8, 8};
    // disk.wtess = {9, 9};

    for (size_t i = 1; i <= nac.xloc.size(); i++)
    {

        script.makeNacelle(nac, i);
        // script.MakeDisk(disk, i - 1);
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

    double altitude = 25000.0;

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
    settings.Sref = wing.planformArea;                                                                               // Reference area [m²]
    settings.Cref = wing.MAC; // Reference chord [m]
    settings.Bref = wing.totalProjectedSpan;                                                                               // Wingspan [m]
    settings.X_cg = wing.xloc + 0.25 * settings.Cref;                                                   // Center of gravity
    settings.Y_cg = 0.0;
    settings.Z_cg = 0.25 * fus.diameter;
    settings.Mach = 0.55;                                                // Mach number
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
        {nac.id, {100, 200, 255}}}; // matcha nacelle_1 e nacelle_2
        // {&disk.id, {80, 80, 80}}};

    for (const auto &surf : components)
    {
        plotter.addComponentWithColorMap(surf, colorMap);
    }

    // Mostra la finestra 3D interattiva
    plotter.show();

    plotter.saveAllViews(std::filesystem::current_path().string());

    // ==================== COG CALCULATION ====================

    BuildAircraft builder(aircraftName, settings);

    builder.buildAircraft();

    AircraftBuildData data = builder.getBuildData();

    COG::COGCalculator calcCenterOfGravity(aircraftName,
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

    // ================= CHORD INTERPOLATOR TEST ====================

    std::vector<double> etaStaionVec = {0.0, 0.25, 0.40, 0.5, 0.75, 1.0};
    std::vector<double> chordEvaluated;

    for (const auto &eta : etaStaionVec)
    {
        chordEvaluated.push_back(ChordCalculator(wing).getChordAtEtaStation(eta, wing.typeOfWing));
    }

    Plot plotChordDistrubution(etaStaionVec, chordEvaluated,
                               "Eta station (-)",
                               "Chord (m)",
                               "Chord distribution",
                               "Distribution",
                               "linespoints", "2", "1", "7", "1", "orange");

    // ================= AIRCRAFT GEOMETRY EXTRACTION TEST ====================

    std::cout << "========================================" << std::endl;
    std::cout << "  AIRCRAFT GEOMETRY EXTRACTION TOOL" << std::endl;
    std::cout << "========================================\n"
              << std::endl;

    // ==================== STEP 1: Estrai tutte le geometrie ====================
    std::cout << "[STEP 1] Extracting all aircraft geometries..." << std::endl;

    VSPGEOMTRYEXTRACTOR::GeometryExtractor geomExtractor;
    geomExtractor.extractAllGeoms(aircraftName, aircraftName + "_AllGeoms.vspscript");

    VSPGEOMTRYEXTRACTOR::AircraftGeometryData allGeomData = geomExtractor.getGeometryData();

    std::cout << "✓ Found " << allGeomData.allGeoms.size() << " geometries\n"
              << std::endl;

    // ==================== STEP 2: Estrai diametri fusoliera ====================
    std::cout << "[STEP 2] Extracting fuselage diameters and positions..." << std::endl;

    VSPGEOMTRYEXTRACTOR::DiametersExtractor fuseExtractor;

    VSPGEOMTRYEXTRACTOR::FuselageDiametersAndXStation fuseData = fuseExtractor.extractFuselageDiameters(
        aircraftName,
        allGeomData,
        fus.length);

    VSPGEOMTRYEXTRACTOR::DiametersExtractor nacelleExctractor;
    VSPGEOMTRYEXTRACTOR::NacelleDiametersAndXStation nacelleData = nacelleExctractor.extractNacelleDiameters(
        aircraftName,
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

    LONGITUDINAL_STABILITY::LongitudinalStabilityCalculator stabilityCalc(builder,
                                                                          cogData,
                                                                          ac,
                                                                          settings,
                                                                          wing,
                                                                          horizontal,
                                                                          fus,
                                                                          nac);

    stabilityCalc.calculateLongitudinalStability();

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

    // ==================== TEST EXCEL WRITER ====================

    DerivativeExcelWriter excelWriter;

    // Case 1

    excelWriter.writeDerivativesToExcel("TestExcelDerivatives.xlsx",
                                        aircraftName,
                                        settings,
                                        stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent(),
                                        directionalStabilityCalc.getSingleComponentsDerivativesSideForce(),
                                        directionalStabilityCalc.getSingleComponentsDerivativesYaw(),
                                        lateralStabilityCalc.getComponentsLateralStabilityDerivativesRoll(),
                                        stabilityCalc.getTotalAircrfatLongitudinalDerivatives(),
                                        stabilityCalc.getTotalAircrfatLongitudinalDynamicDerivatives(),
                                        directionalStabilityCalc.getAircraftDirectionalDerivatives(),
                                        lateralStabilityCalc.getAircraftLateralStabilityDerivativesRoll());

    // Case 2

    // excelWriter.writeDerivativesToExcel("TestExcelDerivatives.xlsx",
    //                                     aircraftName,
    //                                     settings,
    //                                     stabilityCalc.getLongitudinalStabilityDerivativesToSingleComponent(),
    //                                     {},
    //                                     {},
    //                                     {},
    //                                     stabilityCalc.getTotalAircrfatLongitudinalDerivatives(),
    //                                     stabilityCalc.getTotalAircrfatLongitudinalDynamicDerivatives(),
    //                                     {},
    //                                     {}
    //                                     );

    // ==================== TEST TRIM CONDITION ==========================

    double liftCoefficientAtZeroAoA = stabilityCalc.getLongitudinalAerodynamicCoefficients().liftCoefficientAtZeroAoA;
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
    vectorB << -(pitchingMomentAtZeroAoA + deltaCmdeltaHorIncidence * horizontal.yrot),
        -(liftCoefficientAtZeroAoA + deltaCLdeltaHorIncidence * horizontal.yrot);

    Eigen::VectorXd trimSolution = matrixA.colPivHouseholderQr().solve(vectorB);

    std::cout << "\n=== TRIM CONDITION SOLUTION ===" << std::endl;
    std::cout << "Trim angle of attack: " << trimSolution(0) << " degrees" << std::endl;
    std::cout << "Trim elevator deflection: " << trimSolution(1) << " degrees" << std::endl;

    double deltaH = 0.0;
    double vectorLength = 0.0;
    double numberOfIntervalsForChart = 100;
    std::vector<double> alphaAngle = {trimSolution(0), 10.0};
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

        deltaE.push_back(-((pitchingMomentAtZeroAoA + deltaCmdeltaHorIncidence * horizontal.yrot + pitchingMomentSlopeAircraft * i) / (elevetorControlPower)));
        CLe.push_back(liftCoefficientAtZeroAoA + aircraftLiftSlope * i + deltaCLdeltaHorIncidence * horizontal.yrot + deltaCLDeltaHorDeflection * deltaE.back());
    }

    // ========================================================================
    //     TRIM CONDITION PLOT
    // ========================================================================

    Plot plot(CLe, deltaE,
              "CL (-)",
              "Elevator deflection (deg)",
              "Trim - condition",
              "deltaE vs CLe",
              "lines", "1", "1", "", "", "blue");

    // ==================== MOMENT OF INERTIA TEST ====================

    MomentOfInertia momentOfInertiaCalculator(builder);

    MomentOfInertia::Result inertiaResult = momentOfInertiaCalculator.compute(wing, fus);

    std::cout << "\n=== MOMENT OF INERTIA RESULTS ===" << std::endl;
    std::cout << "Ixx: " << inertiaResult.Ixx << " kg·m²" << std::endl;
    std::cout << "Iyy: " << inertiaResult.Iyy << " kg·m²" << std::endl;
    std::cout << "Izz: " << inertiaResult.Izz << " kg·m²" << std::endl;

    // ==================== SHORT PERIOD TEST ====================

    double WTO = data.commonData.getWTO();

    double dynamicPressure = 0.5 * settings.rho * std::pow(settings.Vinf, 2);

    double momentDerivativeWithRespectToQ = (settings.rho * settings.Vinf * settings.Sref *
                                             std::pow(settings.Cref, 2) * 57.3 * stabilityCalc.getTotalAircrfatLongitudinalDynamicDerivatives().deltaCmDeltaPitchSpeed) /
                                            (4 * inertiaResult.Iyy);

    double momentDerivativeWithRespectToAlpha = (dynamicPressure * settings.Sref * settings.Cref *
                                                 57.3 * stabilityCalc.getTotalAircrfatLongitudinalDerivatives().deltaCmDeltaAlphaAircraft) /
                                                inertiaResult.Iyy;

    double zitaSP = -momentDerivativeWithRespectToQ / (2 * std::sqrt(-momentDerivativeWithRespectToAlpha));
    double omegaNSP = std::sqrt(-momentDerivativeWithRespectToAlpha);

    double actualDamping = 2 * zitaSP * omegaNSP * WTO;

    double stiffness = std::pow(omegaNSP, 2) * WTO;

    ODE45 ode45( // CTAD → ODE45<vector<double>>
        [WTO, actualDamping, stiffness](double /*t*/, const std::vector<double> &y) -> std::vector<double>
        {
            return {
                y[1],                                                    // x'
                -(actualDamping / WTO) * y[1] - (stiffness / WTO) * y[0] // x''
            };
        },
        0.0,                   // t0
        20.0,                  // tf
        std::vector{0.0, 0.1}, // y0 = {deltaAlpha, deltaQ}  <- vector<double> → deduce State
        0.01                   // dt
    );

    // Estrai le colonne dal risultato
    const auto &ySolutions = ode45.getY(); // vector<vector<double>>

    // Estrai x(t) e x_dot(t) come vettori separati
    std::vector<double> x_vec, xdot_vec;
    x_vec.reserve(ySolutions.size());
    xdot_vec.reserve(ySolutions.size());

    for (const auto &row : ySolutions)
    {
        x_vec.push_back(57.3 * row[0]);    // AoA vartion (deg)
        xdot_vec.push_back(57.3 * row[1]); // Pitch rate (deg/s)
    }

    Plot plotPitchRate(ode45.getT(), xdot_vec,
                       "Time (s)",
                       "Pitch rate (deg/s)",
                       "Short period response",
                       "SP response",
                       "lines", "1", "1", "", "", "blue");

    Plot plotAoA(ode45.getT(), x_vec,
                 "Time (s)",
                 "AOA (deg)",
                 "Short period response",
                 "SP response",
                 "lines", "1", "1", "", "", "blue");

    // ==================== SAVE OUTPUT FILES ====================
    SaveFiles saveFiles;

    std::cout << "\n=== SAVING FILES ===" << std::endl;
    saveFiles.saveFiles(aircraftName + "_folder", aircraftName);
    std::cout << "✓ Files saved successfully" << std::endl;

    return 0;
}

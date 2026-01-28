#define _HAS_STD_BYTE 0 // Risolve conflitto byte tra C++17 e Windows headers
#define UNICODE         // Assicura che Windows usi le versioni wide delle strutture
#define _UNICODE

#include "VSPScriptGenerator.h"
#include "VSPAeroGenerator.h"
#include "ControlSurfaceBuilder.h"
#include "ConvLength.h"
#include "EnumLENGTH.h"
#include "ATMOSISA.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include "CONVFILEVSP.h"
#include "READLOADFILE.h"
#include "READPOLARFILE.h"
#include "PLOT.h"
#include <list>
#include <format>

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

    VSP::ScriptGenerator script("MyAircraft.vspscript");

    // --- CONTROL SURFACES ---
    VSP::Aircraft ac;
    ac.movables = "faer"; // Flap + Aileron + Elevator + Rudder

    // ac.ver.id = "vertical";
    // ac.ver.mov.type = {'r'};
    // ac.ver.mov.defl = {5.0};
    // ac.ver.isSplittedDeflection = false;

    // --- FUSELAGE ---
    VSP::Fuselage fus;
    fus.id = "fuselage";
    fus.type = "TransportFuse";
    fus.advancedFuselage = false;
    // fus.fuselagePresetName = "AirbusGenericTransportJet";
    fus.customFuselage = true;

    // fus.length         = 37.57;
    // fus.diameter       = 4.14;
    // fus.width          = 3.95010;
    fus.length = 11.7;
    fus.diameter = 1.546;
    fus.nosemult = 1.902;
    fus.aftmult = 2.665;
    fus.nosecenter = -0.217;
    fus.aftcenter = 0.164;
    fus.aftwidth = 0.411;
    fus.aftheight = 0.364;
    fus.utess = 30;
    fus.wtess = 15;

    script.makeFuselage(fus);
    std::cout << "   - Fuselage created" << std::endl;

    // --- MAIN WING ---
    VSP::Wing wing;
    wing.id = "wing";
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

    //--- HORIZONTAL TAIL ---
    VSP::Wing horizontal;
    horizontal.id = "horizontal";
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
    horizontal.span = {2.622};
    horizontal.ctip = {0.698};
    horizontal.croot = {1.298};
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

    // --- VERTICAL TAIL ---
    VSP::Wing vertical;
    vertical.id = "vertical";
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
    vertical.zrot = -3.77;
    vertical.span = {0.372, 1.962};
    vertical.ctip = {2.412, 0.768};
    vertical.croot = {3.157, 2.412};
    vertical.sweep = {60, 32.85714};
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

    // // // --- NACELLE (engines) ---
    // VSP::Nacelle nac;
    // nac.id = "nacelle";
    // nac.advancedNacelle = true;
    // nac.nacellePresetName = "GeneralTurbofan";
    // nac.length = 3.458;
    // nac.diameter = 2.735;
    // nac.xloc = {13.06,13.06};
    // nac.yloc = {5.75,-5.75};
    // nac.zloc = {-1.283,-1.283};

    // for (size_t i = 1; i <= nac.xloc.size(); i++) {

    //     script.makeNacelle(nac,i);
    // }

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
    script.makeDegenGeom("MyAircraft", false);
    std::cout << "   - DegenGeom configured" << std::endl;

    std::cout << "\n✓ AngelScript script generated: MyAircraft.vspscript" << std::endl;

    // ==================== GENERATE VSPAERO FILE ====================
    std::cout << "\n2. Generating VSPAERO file (.vspaero)..." << std::endl;

    VSP::VSPAeroGenerator vspaero("MyAircraft");

    // --- AERODYNAMIC SETTINGS ---
    VSP::AeroSettings settings;

    double altitude = 10000.0;

    auto [T, a, P, rho] = Atmosphere::atmosisa(altitude); // Atmosisa like in MATLAB

    double speedOfSound = Atmosphere::ISA::speedOfSound(altitude); // Calculate only the speed of sound
    double mu = Atmosphere::ISA::viscosity(altitude);              // Calculate only the dynamic viscosity

    std::vector<double> altitudeToVSP;
    altitudeToVSP.push_back(altitude);

    ConvLength convlength(Length::FT, Length::M, altitudeToVSP);

    settings.altitude = convlength.getConvertedValues().at(0); // Returns the first converted altitude value
    settings.Sref = 25.0;                                      // Reference area [m²]
    settings.Cref = 1.8592;                                    // Reference chord [m]
    settings.Bref = 14.0;                                      // Wingspan [m]
    settings.X_cg = wing.xloc + 0.25 * settings.Cref;          // Center of gravity
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

        std::string exeVSPScript = "vspscript.exe -script MyAircraft.vspscript";
        std::cout << "\nExecuting: " << exeVSPScript << std::endl;

        int retVSPScript = system(exeVSPScript.c_str());

        if (retVSPScript != 0 && !std::filesystem::exists("MyAircraft.vsp3"))
        {
            std::cerr << "VSPScript: execution failed" << std::endl;
            return 1;
        }

        //==================== VSPAERO launch command ====================
        std::string exeVSPAero = "vspaero.exe -omp 4 MyAircraft_DegenGeom";
        std::cout << "\nExecuting: " << exeVSPAero << std::endl;

        // Esegui direttamente, output visibile nello stesso terminale
        int ret = system(exeVSPAero.c_str());
        if (ret != 0)
        {
            std::cerr << "VSPAERO: execution failed" << std::endl;
            return 1;
        }
    }

    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    // ====================  READING LOAD FILE SECTION ====================

    VSPAero::LODLoader loader("MyAircraft_DegenGeom.lod");
    Plot<> plotter("Y [m]", "Cl","Cl distribution along the span");
    
    for (size_t i = 0; i < settings.AoA.size(); i++)
    {
        auto case0 = loader.loadCase(settings.AoA[i]); // Case 0 = Wing

        // ==================== MAIN WING ====================
        std::cout << "\n--- WING (Main Wing) ---" << std::endl;
        std::cout << "Sections: " << case0.wingSections.size() << std::endl;
        std::cout << "  Yavg [m]   Cl      Cd      Cmz" << std::endl;
        std::cout << "  ------------------------------------" << std::endl;

        std::cout << "\nAngle of Attack: " << settings.AoA[i] << " degrees" << std::endl;

        // Temporary vectors for plotting
        std::vector<double> Yavg;
        std::vector<double> Cl;

        for (const auto &sec : case0.wingSections)
        {
            std::cout << std::fixed << std::setprecision(4)
                      << "  " << std::setw(8) << sec.Yavg
                      << " " << std::setw(7) << sec.Cl
                      << " " << std::setw(7) << sec.Cd
                      << " " << std::setw(7) << sec.Cmz
                      << std::endl;

            
            Yavg.emplace_back(sec.Yavg);
            Cl.emplace_back(sec.Cl);
        }

        // Aggiungi questa curva al plot con colori diversi
        std::vector<std::string> colors = {"red", "blue", "green"};
        std::string color = colors[i % colors.size()];

        std::ostringstream oss;
        // Set the precision to 3 decimal places for AoA
        oss << "AoA = " << std::fixed << std::setprecision(3) << settings.AoA[i] << "°";
        std::string legend = oss.str();

        plotter.addData(Yavg, Cl, legend, "lines",
                        "1", "1.0", "", "", color);
    }

    // Show the plot with all AoA curves
    plotter.show();

    // ==================== READING POLAR FILE SECTION ====================

    VSPPolar::PolarReader reader;

    reader.readFile("MyAircraft_DegenGeom.polar");

    // polarData è una reference costante al vettore interno dei dati letti dal PolarReader,
    // così si evita la copia e si accede direttamente ai dati.
    const auto &polarData = reader.getData();

    const auto &headers = reader.getHeaders();

    std::cout << "\n--- POLAR DATA ---" << std::endl;

    for (const auto &point : polarData)
    {
        std::cout << "AoA: " << point.AoA
                  << " | CL: " << point.CL
                  << " | CDtot: " << point.CDtot
                  << std::endl;
    }

    std::cout << "\n--- HEADERS ---" << std::endl;

    for (const auto &header : headers)
    {
        std::cout << header << " ";
    }


    

    return 0;
}
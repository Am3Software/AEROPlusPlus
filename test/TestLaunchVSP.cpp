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

 int main() {
 

        // ==================== Verifica directory corrente ====================
        std::cout << "Current directory: " << std::filesystem::current_path() << std::endl;

        // ==================== DETERMINA LA DIRECTORY BUILD ====================
        // Ottieni la directory corrente e vai in build/
        std::filesystem::path currentDir = std::filesystem::current_path();
        std::filesystem::path buildDir;

        // Se sei già in build/, resta lì
        if (currentDir.filename() == "build")
        {
            buildDir = currentDir;
        }
        // Altrimenti vai in build/
        else if (std::filesystem::exists(currentDir / "build"))
        {
            buildDir = currentDir / "build";
        }
        // Oppure sei in un'altra sottodirectory, vai in ../build
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

        // ==================== VSPScript execution ====================
        // Gli eseguibili sono qui (build/), i file sono in ../
        std::string exeVSPScript = "vspscript.exe -script MyAircraft.vspscript";
        std::cout << "\nExecuting: " << exeVSPScript << std::endl;

        // Apri una nuova finestra DOS visibile
        std::string cmdVSPScript = "start cmd.exe /K \"" + exeVSPScript + "\"";
        int retVSPScript = system(cmdVSPScript.c_str());
        if (retVSPScript != 0)
        {
            std::cerr << "VSPScript: esecuzione fallita" << std::endl;
            return 1;
        }

        //==================== VSPAERO launch command ====================

        std::string cmd =
            "cmd /C \"cd /d " + buildDir.string() + " && vspaero -omp 4 MyAircraft_DegenGeom\"";

        int ret = system(cmd.c_str());

        if (ret != 0) {
            std::cerr << "VSPAERO: esecuzione fallita" << std::endl;
            return 1;
        }
    }
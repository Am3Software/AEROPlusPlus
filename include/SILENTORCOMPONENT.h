#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <array>
#include <memory>
#include <iostream>
#include "VSPGEOMETRYEXTRACTOR.h"
#include "VSPAeroGenerator.h"
#include "VSPScriptGenerator.h"
#include "READPOLARFILE.h"
#include "ControlSurfaceBuilder.h"
#include "EQUALSIGNORECASE.h"

namespace SILENTORCOMPONENT
{
    struct AerodynamicCoefficients
    {
        std::vector<double> liftCoefficient;
        std::vector<double> pitchingMomentCoefficient;
    };

    class SilentorComponent
    {

    private:
        VSPGEOMTRYEXTRACTOR::AircraftGeometryData geometryData;
        SILENTORCOMPONENT::AerodynamicCoefficients aeroCoeffs;
        VSP::Aircraft silentorAC;
        std::string nameOfAircrfat;
        std::filesystem::path baseDir = std::filesystem::current_path();
        std::string typeOfGeom;
        std::string specifiedComponentToCalculateWettedArea;

        std::ofstream file;
        std::string parentFolder;
        std::string filenameVspScript;

        std::string movablesDetected;

        std::filesystem::path sourceAircraftFilePath;
        std::filesystem::path copyAircraftFilePath;

        // // Memorizza i dati delle geometrie
        // AircrfatIDGeom geometryData;

        // // Memorizza i risultati della wetted area
        // WettedAreaResults wettedResults;

        inline void writeComment(const std::string &comment)
        {
            file << "// " << comment << "\r\n\r\n";
        }

        inline void writeCommand(const std::string &command)
        {
            file << command << "\r\n";
        }

        inline void writeUpdate()
        {
            file << "Update();\r\n\r\n";
        }

        inline std::string replaceBackslash(const std::string &str)
        {
            std::string result;
            for (char c : str)
            {
                if (c == '\\')
                {
                    result += "\\\\";
                }
                else
                {
                    result += c;
                }
            }
            return result;
        }

        // inline int executeCommand(const std::string &command, int chooseLauncher, std::string vspExecutable = "")
        // {
        //     if (chooseLauncher == 1)
        //     {
        //         int retVSPScript = system(command.c_str());
        //         if (retVSPScript != 0)
        //         {
        //             std::cerr << "VSPScript: execution failed" << std::endl;
        //             return 1;
        //         }
        //         return 0;
        //     }
        //     else if (chooseLauncher == 2)
        //     {
        //         int retVSPAero = system(command.c_str());
        //         if (retVSPAero != 0)
        //         {
        //             std::cerr << "VSPAERO: execution failed" << std::endl;
        //             return 1;
        //         }
        //         return 0;
        //     }
        //     else
        //     {
        //         std::cerr << "executeCommand: invalid chooseLauncher value" << std::endl;
        //         return -1;
        //     }
        // }

        int executeCommand(const std::string &command, int chooseLauncher, std::string vspExecutable = "")
        {
            // Salva e forza il working directory corretto
            std::filesystem::path originalPath = std::filesystem::current_path();
            std::filesystem::current_path(parentFolder); // <- forza CWD alla cartella del progetto

            int ret = -1;
            if (chooseLauncher == 1)
            {
                ret = system(command.c_str());
                if (ret != 0)
                    std::cerr << "[ERROR] VSPScript failed (ret=" << ret << "): " << command << std::endl;
            }
            else if (chooseLauncher == 2)
            {
                ret = system(command.c_str());
                if (ret != 0)
                    std::cerr << "[ERROR] VSPAERO failed (ret=" << ret << "): " << command << std::endl;
            }
            else
            {
                std::cerr << "[ERROR] executeCommand: invalid chooseLauncher" << std::endl;
            }

            std::filesystem::current_path(originalPath); // ripristina
            return (ret == 0) ? 0 : 1;
        }

    public:
        /// @brief Constructor for SilentorComponent class.
        /// @param nameOfAircraft The name of the aircrfat
        /// @param filename The name of the .vspscript file to generate and execute.
        /// @param parentFolderPath The path to the parent folder (optional).
        SilentorComponent(const std::string &nameOfAircraft,
                          const std::string &filename,
                          const std::string &parentFolderPath = "")
        {

            this->nameOfAircrfat = nameOfAircraft;
            this->filenameVspScript = filename;
            this->parentFolder = parentFolderPath;

            if (parentFolderPath.empty())
            {
                parentFolder = baseDir.string();

                sourceAircraftFilePath = baseDir / (nameOfAircraft + ".vsp3");
                copyAircraftFilePath = baseDir / (nameOfAircraft + "_copy.vsp3");

                if (std::filesystem::exists(copyAircraftFilePath))
                {

                    std::filesystem::remove(copyAircraftFilePath);
                }

                std::filesystem::copy_file(sourceAircraftFilePath, copyAircraftFilePath);
            }
            else
            {
                parentFolder = parentFolderPath;

                sourceAircraftFilePath = std::filesystem::path(parentFolder) / (nameOfAircraft + ".vsp3");
                copyAircraftFilePath = std::filesystem::path(parentFolder) / (nameOfAircraft + "_copy.vsp3");

                if (std::filesystem::exists(copyAircraftFilePath))
                {

                    std::filesystem::remove(copyAircraftFilePath);
                }

                std::filesystem::copy_file(sourceAircraftFilePath, copyAircraftFilePath);
            }
        }

        ~SilentorComponent()
        {

            // Remove the genreted files
            // Remove Copy Aircrfat

            if (file.is_open())
            {
                file.flush();
                file.close();
            }

            try {

            // if (std::filesystem::exists(copyAircraftFilePath))
            // {
            //     std::filesystem::remove(copyAircraftFilePath);
            // }
            // std::filesystem::remove(std::filesystem::path(parentFolder) / (nameOfAircrfat + "_copy.vsp3"));

            // // Remove VSP Script
            // std::filesystem::remove(std::filesystem::path(parentFolder) / filenameVspScript);

            for (const auto &entry : std::filesystem::directory_iterator(parentFolder))
            {
                const std::string filename = entry.path().filename().string();
                const std::string prefix = nameOfAircrfat + "_copy";

                if (filename.rfind(prefix, 0) == 0) // starts_with prefix
                {
                    std::filesystem::remove(entry.path());
                }
            }
           
            }
            catch (const std::exception &e)
            {
                // Mai rilanciare dal distruttore — solo log
                std::cerr << "[WARNING] ~SilentorComponent: cleanup failed: " << e.what() << std::endl;
            }
        }

        /// @brief Silent the disired component and execute the degenerate geometry
        /// @param aircrfatName The name of the aircraft.
        /// @param filename The name of the file with the exstension .vspscript.
        /// @param parentFolderPath The path to the parent folder (optional).
        void GetGeometryWithThisComponent( const VSP::Aircraft &ac,  VSPGEOMTRYEXTRACTOR::AircraftGeometryData &geometryData,
                                          const std::vector<std::string> &nameOfComponent = {}, const std::string &parentFolderPath = "")
        {
            if (parentFolderPath.empty()) {
                parentFolder = std::filesystem::current_path().string();
            }
            else {
                parentFolder = parentFolderPath;
            }

            // 2. Poi chiudi eventuale file aperto
            if (file.is_open()){
                file.close();
            }

            // 3. Poi apri con path ASSOLUTO
            std::filesystem::path scriptPath = std::filesystem::path(parentFolder) / filenameVspScript;
            file.open(scriptPath);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open: " + scriptPath.string());
            }

            file << "void main(){\r\n";
            writeComment("Calculate Silentor Component Geometry");

            // Carica il file aircraft
            std::string aircrfatLoaded = parentFolder + "\\" + nameOfAircrfat + "_copy.vsp3";
            std::string aircraftCwdEscaped = replaceBackslash(aircrfatLoaded);

            writeComment("Load the VSP3 file");
            writeCommand("string fnamePreset = \"" + aircraftCwdEscaped + "\";");
            writeCommand("ReadVSPFile(fnamePreset);");
            writeUpdate();

            file << "\r\n";

            // Case for the input of a specific component name

            std::map<std::string, bool> componentVisibility;

            this->silentorAC = VSP::Aircraft{}; // reset prima di popolarlo
            // Loop esterno su tutte le geometrie
            for (const auto &geom : geometryData.allGeoms)
            {
                // Cerca se il componente è nella lista dei componenti da mostrare
                bool found = std::find(nameOfComponent.begin(), nameOfComponent.end(), geom.nameOfComponent) != nameOfComponent.end();

                if (found)
                {
                    writeCommand("SetSetFlag (\"" + geom.id + "\", SET_SHOWN, true);");
                    file << "\r\n";
                    componentVisibility[geom.nameOfComponent] = true;

                    if (equalsIgnoreCase(geom.nameOfComponent, ac.wing.id))
                    {
                        silentorAC.wing = ac.wing;
                        // Aggiungi 'f' una sola volta se presente
                        if (std::find(ac.wing.mov.type.begin(), ac.wing.mov.type.end(), 'f') != ac.wing.mov.type.end())
                        {
                            movablesDetected += 'f';
                        }

                        // Aggiungi 's' una sola volta se presente
                        if (std::find(ac.wing.mov.type.begin(), ac.wing.mov.type.end(), 's') != ac.wing.mov.type.end())
                        {
                            movablesDetected += 's';
                        }

                        // Aggiungi 'a' una sola volta se presente
                        if (std::find(ac.wing.mov.type.begin(), ac.wing.mov.type.end(), 'a') != ac.wing.mov.type.end())
                        {
                            movablesDetected += 'a';
                        }
                    }
                    else if (equalsIgnoreCase(geom.nameOfComponent, ac.canard.id))
                    {
                        silentorAC.canard = ac.canard;
                        movablesDetected += silentorAC.canard.mov.type.front();
                    }
                    else if (equalsIgnoreCase(geom.nameOfComponent, ac.hor.id))
                    {
                        silentorAC.hor = ac.hor;
                        movablesDetected += silentorAC.hor.mov.type.front();
                    }
                    else if (equalsIgnoreCase(geom.nameOfComponent, ac.ver.id))
                    {
                        silentorAC.ver = ac.ver;
                        movablesDetected += silentorAC.ver.mov.type.front();
                    }

                    silentorAC.movables = movablesDetected;
                }
                else
                {

                    if (geom.id.size() > nameOfComponent.size())
                    {
                        writeCommand("SetSetFlag (\"" + geom.id + "\", SET_SHOWN, false);");
                        file << "\r\n";
                        componentVisibility[geom.nameOfComponent] = false;
                    }

                    else
                    {

                        continue;
                    }
                }
            }

            writeCommand("SetComputationFileName(DEGEN_GEOM_CSV_TYPE,\"" + nameOfAircrfat + "_copy_DegenGeom.csv\");");
            writeCommand("ComputeDegenGeom(SET_SHOWN,DEGEN_GEOM_CSV_TYPE);");
            file << "\r\n";

            writeCommand("SetComputationFileName(DEGEN_GEOM_M_TYPE,\"" + nameOfAircrfat + "_copy_DegenGeom.m\");");
            writeCommand("ComputeDegenGeom(SET_SHOWN,DEGEN_GEOM_M_TYPE);");
            file << "\r\n";

            file << "}\r\n";

            file.flush();
            file.close();
            


            executeCommand("vspscript.exe -script " + filenameVspScript, 1);
        }

        // Esegue lo script di wetted area e cattura i risultati

        /// @brief Executes the wetted area script and captures the results from the terminal output.
        /// @param nameOfTheFileToWetArea Name of the script file that calculates the analysis - no one extension
        /// @param vspExecutable The VSP executable to use (mandatory is "vspscript.exe").
        void executeAnalysis(VSP::AeroSettings settings,
                             const std::string &vspExecutable = "vspaero.exe")
        {

            aeroCoeffs.liftCoefficient.clear();

            VSP::VSPAeroGenerator vspaero(nameOfAircrfat + "_copy");

            vspaero.writeSettings(settings);

            vspaero.writeRotors(settings, "P"); // Config = propeller

            auto controls = VSP::ControlSurfaceBuilder::buildControlSurfaces(this->silentorAC, settings.Symmetry);
            vspaero.writeControlSurfaces(controls);

            vspaero.writeFooter(settings);
            std::cout << "\n ✓ VSPAERO file generated: MyAircraft_DegenGeom.vspaero" << std::endl;
            // // close file .VSPAERO mandatory
            // vspaero.close();

            // // Costruisci il comando
            // std::string command = vspExecutable + " -omp 4 " + nameOfAircrfat + "_copy_DegenGeom";

            // // Esegui l'analisi
            // executeCommand(command,2);

            // In executeAnalysis
            vspaero.close();

            std::filesystem::path degenGeomPath = std::filesystem::path(parentFolder) / (nameOfAircrfat + "_copy_DegenGeom");
            std::string command = vspExecutable + " -omp 4 \"" + degenGeomPath.string() + "\"";
            executeCommand(command, 2);

            VSPPolar::PolarReader reader;
            reader.readFile(nameOfAircrfat + "_copy_DegenGeom.polar");
            const auto &polarData = reader.getData();
            const auto &headers = reader.getHeaders();

            // std::cout << "\n--- POLAR DATA ---" << std::endl;
            for (const auto &point : polarData)
            {
                aeroCoeffs.liftCoefficient.push_back(point.CL);
                aeroCoeffs.pitchingMomentCoefficient.push_back(point.CMm);
            }

            
        }

        // Getters results

        SILENTORCOMPONENT::AerodynamicCoefficients getAerodynamicCoefficients() const
        {
            return aeroCoeffs;
        }
    };
} // namespace WETTEDAREA
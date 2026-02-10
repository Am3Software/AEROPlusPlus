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

namespace WETTEDAREA
{
    struct GeomInfo
    {
        std::string id;              // ID da OpenVSP
        std::string name;            // Type name da OpenVSP (GetGeomTypeName)
        std::string nameOfComponent; // Nome componente da OpenVSP (GetGeomName)
    };

    struct AircrfatIDGeom
    {
        std::vector<GeomInfo> allGeoms; // TUTTI gli elementi catturati
    };

    // Struttura per salvare i risultati della wetted area
    struct WettedAreaResults
    {
        double WET_FUSE_AREA = 0.0;
        double WET_NAC_AREA = 0.0;
        double WET_BOOM_AREA = 0.0;
        double WET_AREA = 0.0;
    };

    class WettedArea
    {

    private:
        std::string nameOfAircrfat;
        std::filesystem::path baseDir = std::filesystem::current_path();
        std::string typeOfGeom;
        std::string specifiedComponentToCalculateWettedArea;

        std::ofstream file;
        std::string parentFolder;

        // Memorizza i dati delle geometrie
        AircrfatIDGeom geometryData;

        // Memorizza i risultati della wetted area
        WettedAreaResults wettedResults;

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

        // Funzione per eseguire un comando e catturare l'output
        inline std::string executeCommand(const std::string &command)
        {
            std::string result;

#ifdef _WIN32
            // Windows
            std::array<char, 128> buffer;
            std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);

            if (!pipe)
            {
                throw std::runtime_error("popen() failed!");
            }

            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
            {
                result += buffer.data();
            }
#else
            // Linux/Mac
            std::array<char, 128> buffer;
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);

            if (!pipe)
            {
                throw std::runtime_error("popen() failed!");
            }

            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
            {
                result += buffer.data();
            }
#endif

            return result;
        }

        // Funzione per parsare l'output e popolare la struttura
        inline void parseGeomOutput(const std::string &output)
        {
            geometryData = AircrfatIDGeom(); // Reset

            std::istringstream stream(output);
            std::string line;

            while (std::getline(stream, line))
            {
                // Salta linee vuote
                if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos)
                    continue;

                GeomInfo geom;
                std::istringstream lineStream(line);

                // Formato CSV: name,id,nameOfComponent
                std::getline(lineStream, geom.name, ',');
                std::getline(lineStream, geom.id, ',');
                std::getline(lineStream, geom.nameOfComponent);

                // Pulisci solo caratteri speciali, mantieni tutto il resto
                auto cleanString = [](std::string &str)
                {
                    str.erase(std::remove_if(str.begin(), str.end(),
                                             [](char c)
                                             { return c == '\r' || c == '\n' || c == '"'; }),
                              str.end());
                    // Trim spazi iniziali e finali
                    size_t start = str.find_first_not_of(" \t");
                    size_t end = str.find_last_not_of(" \t");
                    if (start != std::string::npos && end != std::string::npos)
                        str = str.substr(start, end - start + 1);
                };

                cleanString(geom.name);
                cleanString(geom.id);
                cleanString(geom.nameOfComponent);

                // Aggiungi alla lista se i dati sono validi
                if (!geom.name.empty() && !geom.id.empty() && !geom.nameOfComponent.empty())
                {
                    geometryData.allGeoms.push_back(geom);
                }
            }
        }

        // Funzione per parsare i risultati della wetted area
        inline void parseWettedAreaResults(const std::string &output)
        {
            wettedResults = WettedAreaResults(); // Reset a 0

            std::istringstream stream(output);
            std::string line;

            while (std::getline(stream, line))
            {
                // Cerca "WET_FUSE_AREA: <valore>"
                if (line.find("WET_FUSE_AREA:") != std::string::npos)
                {
                    size_t pos = line.find(":");
                    if (pos != std::string::npos)
                    {
                        std::string valueStr = line.substr(pos + 1);
                        // Rimuovi spazi
                        valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
                        try
                        {
                            wettedResults.WET_FUSE_AREA = std::stod(valueStr);
                        }
                        catch (...)
                        {
                            wettedResults.WET_FUSE_AREA = 0.0;
                        }
                    }
                }
                // Cerca "WET_NAC_AREA: <valore>"
                else if (line.find("WET_NAC_AREA:") != std::string::npos)
                {
                    size_t pos = line.find(":");
                    if (pos != std::string::npos)
                    {
                        std::string valueStr = line.substr(pos + 1);
                        valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
                        try
                        {
                            wettedResults.WET_NAC_AREA = std::stod(valueStr);
                        }
                        catch (...)
                        {
                            wettedResults.WET_NAC_AREA = 0.0;
                        }
                    }
                }
                // Cerca "WET_BOOM_AREA: <valore>"
                else if (line.find("WET_BOOM_AREA:") != std::string::npos)
                {
                    size_t pos = line.find(":");
                    if (pos != std::string::npos)
                    {
                        std::string valueStr = line.substr(pos + 1);
                        valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
                        try
                        {
                            wettedResults.WET_BOOM_AREA = std::stod(valueStr);
                        }
                        catch (...)
                        {
                            wettedResults.WET_BOOM_AREA = 0.0;
                        }
                    }
                }

                else
                {
                    // Cerca "WET_AREA: <valore>"
                    if (line.find("WET_AREA:") != std::string::npos)
                    {
                        size_t pos = line.find(":");
                        if (pos != std::string::npos)
                        {
                            std::string valueStr = line.substr(pos + 1);
                            valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
                            try
                            {
                                wettedResults.WET_AREA = std::stod(valueStr);
                            }
                            catch (...)
                            {
                                wettedResults.WET_AREA = 0.0;
                            }
                        }
                    }
                }
            }
        }

    public:
        /// @brief Constructor for WettedArea class.
        /// @param filename The name of the file to open with the exstension .vspscript.
        /// @param parentFolderPath The path to the parent folder (optional).
        inline WettedArea(const std::string &filename, const std::string &parentFolderPath = "")
        {
            file.open(filename);
            if (!file.is_open())
            {
                throw std::runtime_error("Cannot open file: " + filename);
            }

            if (parentFolderPath.empty())
            {
                parentFolder = baseDir.string();
            }
            else
            {
                parentFolder = parentFolderPath;
            }
            file << "void main(){\r\n";
        }

        inline ~WettedArea()
        {
            if (file.is_open())
            {
                file.close();
            }
        }

        // Genera lo script che stampa i dati in formato CSV

        /// @brief Generates a script that prints geometry data such as, id, name and nameOfComponents.
        /// @param nameOfAircraft The name of the aircraft.
        /// @param filename The name of the file with the exstension .vspscript.
        /// @param parentFolderPath The path to the parent folder (optional).
        inline void getAllGeoms(std::string nameOfAircraft, const std::string &filename,
                                bool isDefaultCase = true, const std::string &parentFolderPath = "")
        {
            if (file.is_open())
            {
                file.close();
            }

            file.open(filename);
            if (!file.is_open())
            {
                throw std::runtime_error("Cannot open file: " + filename);
            }

            if (parentFolderPath.empty())
            {
                parentFolder = std::filesystem::current_path().string();
            }
            else
            {
                parentFolder = parentFolderPath;
            }

            file << "void main(){\r\n";

            std::string aircraftLoaded = parentFolder + "\\" + nameOfAircraft + ".vsp3";
            std::string aircraftCwdEscaped = replaceBackslash(aircraftLoaded);

            writeComment("Load the VSP3 file");
            writeCommand("string fnamePreset = \"" + aircraftCwdEscaped + "\";");
            file << "\r\n";

            writeCommand("ReadVSPFile(fnamePreset);");
            writeUpdate();
            file << "\r\n";

            writeComment("Print geometry information in CSV format: name,id,nameOfComponent");
            writeCommand("array<string> geom_ids = FindGeoms();");
            file << "\r\n";

            writeCommand("for (uint i = 0; i < geom_ids.length(); i++)");
            writeCommand("{");
            writeCommand("    string name = GetGeomTypeName(geom_ids[i]);");
            writeCommand("    string id = geom_ids[i];");
            writeCommand("    string nameOfComponent = GetGeomName(geom_ids[i]);");
            file << "\r\n";

            // Stampa in formato CSV
            writeCommand("    Print(name + \",\" + id + \",\" + nameOfComponent + \"\\n\");");
            file << "\r\n";

            writeCommand("}");
            file << "\r\n";

            writeCommand("}");

            file.close();

            if (isDefaultCase)
            {
                // Executes the script and captures the geometry data
                WETTEDAREA::WettedArea::executeAndCaptureGeomIds(filename);

                // Calculate Wetted Area
                WETTEDAREA::WettedArea::CalculateWettedArea(nameOfAircraft, nameOfAircraft + "_WettedArea.vspscript");

                // Executes the script and captures the wetted area results
                WETTEDAREA::WettedArea::executeAndCaptureWettedArea(nameOfAircraft + "_WettedArea.vspscript");
            }
        }

        // Esegue lo script e cattura automaticamente i dati dal terminale

        /// @brief Executes the script and automatically captures the geometry data from the terminal output.
        /// @param nameOfTheFile The name of the script file to execute.
        /// @param vspExecutable The VSP executable to use (mandatory is "vspscript.exe").
        inline void executeAndCaptureGeomIds(const std::string &nameOfTheFile,
                                             const std::string &vspExecutable = "vspscript.exe")
        {
            // Costruisci il comando
            std::string command = vspExecutable + " -script \"" + nameOfTheFile + "\"";

            // Esegui e cattura l'output
            std::string output = executeCommand(command);

            // Parsa l'output
            parseGeomOutput(output);
        }

        // Getter per accedere ai dati

        /// @brief Gets the captured geometry data.
        /// @return The AircrfatIDGeom structure containing all captured geometry data.
        inline const AircrfatIDGeom &getGeometryData() const
        {
            return geometryData;
        }

        // Getter per accedere ai risultati della wetted area

        /// @brief Gets the wetted area results.
        /// @return The WettedAreaResults structure containing the wetted area results.
        inline const WettedAreaResults &getWettedAreaResults() const
        {
            return wettedResults;
        }

        // Calcola Wetted Area - qui hai accesso a TUTTI i dati catturati

        /// @brief Calculates the wetted area using the captured geometry data and generates a script to compute it.
        /// @param aircrfatName The name of the aircraft.
        /// @param filename The name of the file with the exstension .vspscript.
        /// @param parentFolderPath The path to the parent folder (optional).
        inline void CalculateWettedArea(const std::string &aircrfatName, const std::string &filename,
                                        const std::string &nameOfComponent = "", const std::string &parentFolderPath = "")
        {
            this->nameOfAircrfat = aircrfatName;

            if (file.is_open())
            {
                file.close();
            }

            file.open(filename);
            if (!file.is_open())
            {
                throw std::runtime_error("Cannot open file: " + filename);
            }

            if (parentFolderPath.empty())
            {
                parentFolder = std::filesystem::current_path().string();
            }
            else
            {
                parentFolder = parentFolderPath;
            }

            file << "void main(){\r\n";
            writeComment("Calculate Wetted Area");

            // Carica il file aircraft
            std::string aircrfatLoaded = parentFolder + "\\" + aircrfatName + ".vsp3";
            std::string aircraftCwdEscaped = replaceBackslash(aircrfatLoaded);

            writeComment("Load the VSP3 file");
            writeCommand("string fnamePreset = \"" + aircraftCwdEscaped + "\";");
            writeCommand("ReadVSPFile(fnamePreset);");
            writeUpdate();

            file << "\r\n";

            // Hide all geometries except the fuselage, nacelle and boom (if present)

            if (nameOfComponent.empty())
            {
                for (const auto &geom : geometryData.allGeoms)
                {

                    if (geom.nameOfComponent != "fuselage" && geom.nameOfComponent != "TransportFuse" &&
                        geom.nameOfComponent.substr(0, 3) != "nac" && geom.nameOfComponent.substr(0, 4) != "boom")
                    {
                        writeCommand("SetSetFlag (\"" + geom.id + "\", SET_SHOWN, false);");
                    }
                }

                file << "\r\n";

                writeCommand("string mesh_id = ComputeCompGeom( SET_SHOWN, false, 0 );");
                writeCommand("string comp_res_id = FindLatestResultsID( \"Comp_Geom\" );");
                writeCommand("array<double> @double_arr = GetDoubleResults( comp_res_id, \"Wet_Area\" );");
                file << "\r\n";

                // Trova gli indici corretti per ogni tipo di geometria
                int fuseIndex = -1;
                int nacIndex = -1;
                int boomIndex = -1;
                int currentIndex = 0;

                for (const auto &geom : geometryData.allGeoms)
                {
                    if (geom.nameOfComponent == "fuselage" || geom.nameOfComponent == "TransportFuse")
                    {
                        if (fuseIndex == -1)
                            fuseIndex = currentIndex;
                        currentIndex++;
                    }
                    else if (geom.nameOfComponent.substr(0, 3) == "nac")
                    {
                        if (nacIndex == -1)
                            nacIndex = currentIndex;
                        currentIndex++;
                    }
                    else if (geom.nameOfComponent.substr(0, 4) == "boom")
                    {
                        if (boomIndex == -1)
                            boomIndex = currentIndex;
                        currentIndex++;
                    }
                }

                // Stampa i risultati solo se l'indice è valido
                if (fuseIndex >= 0)
                {
                    writeCommand("Print(\"WET_FUSE_AREA: \" + double_arr[" + std::to_string(fuseIndex) + "] + \"\\n\");");
                }

                if (nacIndex >= 0)
                {
                    writeCommand("Print(\"WET_NAC_AREA: \" + double_arr[" + std::to_string(nacIndex) + "] + \"\\n\");");
                }

                if (boomIndex >= 0)
                {
                    writeCommand("Print(\"WET_BOOM_AREA: \" + double_arr[" + std::to_string(boomIndex) + "] + \"\\n\");");
                }
            }
            // Case for the input of a specific component name
            else
            {

                file << "\r\n";

                for (const auto &geom : geometryData.allGeoms)
                {

                    if (geom.nameOfComponent != nameOfComponent)
                    {
                        writeCommand("SetSetFlag (\"" + geom.id + "\", SET_SHOWN, false);");
                        file << "\r\n";
                    }
                }

                writeCommand("string mesh_id = ComputeCompGeom( SET_SHOWN, false, 0 );");
                writeCommand("string comp_res_id = FindLatestResultsID( \"Comp_Geom\" );");
                writeCommand("array<double> @double_arr = GetDoubleResults( comp_res_id, \"Wet_Area\" );");
                file << "\r\n";

                // Trova gli indici corretti per ogni tipo di geometria
                int componentIndex = -1;
                int currentIndex = 0;

                for (const auto &geom : geometryData.allGeoms)
                {
                    if (geom.nameOfComponent == nameOfComponent)
                    {
                        if (componentIndex == -1)
                            componentIndex = currentIndex;
                        currentIndex++;
                    }
                }

                // Stampa i risultati solo se l'indice è valido
                if (componentIndex >= 0)
                {
                    writeCommand("Print(\"WET_AREA: \" + double_arr[" + std::to_string(componentIndex) + "] + \"\\n\");");
                }
            }

            file << "}\r\n";

            file.close();
        }

        // Esegue lo script di wetted area e cattura i risultati

        /// @brief Executes the wetted area script and captures the results from the terminal output.
        /// @param nameOfTheFileToWetArea Name of the script file that calculates wetted area with the exstension .vspscript.
        /// @param vspExecutable The VSP executable to use (mandatory is "vspscript.exe").
        inline void executeAndCaptureWettedArea(const std::string &nameOfTheFileToWetArea,
                                                const std::string &vspExecutable = "vspscript.exe")
        {
            // Costruisci il comando
            std::string command = vspExecutable + " -script \"" + nameOfTheFileToWetArea + "\"";

            // Esegui e cattura l'output
            std::string output = executeCommand(command);

            // Parsa i risultati
            parseWettedAreaResults(output);
        }
    };
} // namespace WETTEDAREA
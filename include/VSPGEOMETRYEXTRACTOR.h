#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <array>
#include <memory>
#include <sstream>
#include <algorithm>
#include <filesystem>


namespace VSPGEOMTRYEXTRACTOR
{
    struct GeomInfo
    {
        std::string id;              // ID da OpenVSP
        std::string name;            // Type name da OpenVSP (GetGeomTypeName)
        std::string nameOfComponent; // Nome componente da OpenVSP (GetGeomName)

       
    };

    struct AircraftGeometryData
    {
        std::vector<GeomInfo> allGeoms; // TUTTI gli elementi catturati
        std::vector<std::string> idGeom;
        std::vector<std::string> nameGeom;
        std::vector<std::string> nameOfComponentGeom;

    };

    class GeometryExtractor
    {
    private:
        std::ofstream file;
        std::string parentFolder;
        AircraftGeometryData geometryData;

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

        inline std::string executeCommand(const std::string &command)
        {
            std::string result;

#ifdef _WIN32
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

        inline void parseGeomOutput(const std::string &output)
        {
            geometryData = AircraftGeometryData(); // Reset

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
                    geometryData.idGeom.push_back(geom.id);
                    geometryData.nameGeom.push_back(geom.name);
                    geometryData.nameOfComponentGeom.push_back(geom.nameOfComponent);
                }
            }
        }

    public:
        /// @brief Constructor
        /// @param parentFolderPath The path to the parent folder (optional).
        inline GeometryExtractor(const std::string &parentFolderPath = "")
        {
            if (parentFolderPath.empty())
            {
                parentFolder = std::filesystem::current_path().string();
            }
            else
            {
                parentFolder = parentFolderPath;
            }
        }

        inline ~GeometryExtractor()
        {
            if (file.is_open())
            {
                file.close();
            }
        }

        /// @brief Generates a script that prints geometry data such as id, name and nameOfComponents.
        /// @param nameOfAircraft The name of the aircraft.
        /// @param filename The name of the file with the extension .vspscript.
        inline void extractAllGeoms(const std::string &nameOfAircraft, const std::string &filename)
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

            // Executes the script and captures the geometry data
            executeAndCaptureGeomIds(filename);
        }

        /// @brief Executes the script and automatically captures the geometry data from the terminal output.
        /// @param nameOfTheFile The name of the script file to execute.
        /// @param vspExecutable The VSP executable to use (default is "vspscript.exe").
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

        /// @brief Gets the captured geometry data.
        /// @return The AircraftGeometryData structure containing all captured geometry data.
        inline const AircraftGeometryData &getGeometryData() const
        {
            return geometryData;
        }
    };

} // namespace VSP
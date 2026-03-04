#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <regex>
#include <sstream>
#include <algorithm>
#include <numeric>
#include "VSPGEOMETRYEXTRACTOR.h"

namespace VSPGEOMTRYEXTRACTOR
{

    struct FuselageDiametersAndXStation
    {
        std::vector<double> allFuselageWidth;
        std::vector<double> allFuselageHeight;
        std::vector<double> xStation;
    };

    struct NacelleDiametersAndXStation
    {
        std::vector<double> allNacelleWidth;
        std::vector<double> allNacelleHeights;
        std::vector<double> xStation;
    };

    class DiametersExtractor
    {
    private:
        std::ofstream file;
        std::string parentFolder;

        inline void writeComment(const std::string &comment)
        {
            file << "// " << comment << "\r\n\r\n";
        }

        inline void writeCommand(const std::string &command)
        {
            file << command << "\r\n";
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
            FILE *pipe = _popen(command.c_str(), "r");
#else
            FILE *pipe = popen(command.c_str(), "r");
#endif

            if (!pipe)
            {
                throw std::runtime_error("Failed to execute command: " + command);
            }

            char buffer[256];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                result += buffer;
            }

#ifdef _WIN32
            _pclose(pipe);
#else
            pclose(pipe);
#endif

            return result;
        }

        inline void createDiametersScript(
            const std::string &scriptFilename,
            const std::string &nameOfAircraft,
            const std::string &idCompomnent,
            const std::string &typeOfComponent,
            bool isCustomFuselage)
        {
            std::ofstream scriptFile(scriptFilename);

            if (!scriptFile.is_open())
            {
                throw std::runtime_error("Cannot create script file: " + scriptFilename);
            }

            std::ofstream tempFile;
            tempFile.swap(file);
            file.swap(scriptFile);

            std::string cwd = parentFolder;
            std::string cwdEscaped = replaceBackslash(cwd);

            file << "void main() {\r\n\r\n";

            writeCommand("string fnamePreset = \"" + cwdEscaped + "\\\\" + nameOfAircraft + ".vsp3\";");
            file << "\r\n";
            writeCommand("ReadVSPFile(fnamePreset);");
            file << "\r\n";

            if (typeOfComponent == "Fuselage")
            {
                writeFuselageTypeScript(idCompomnent);
            }
            else if (typeOfComponent == "Stack")
            {
                writeStackTypeScript(idCompomnent);
            }
            else if (isCustomFuselage)
            {
                writeCustomFuselageScript(idCompomnent);
            }

            file << "}\r\n";

            scriptFile.swap(file);
            file.swap(tempFile);
            scriptFile.close();
        }

        inline void writeFuselageTypeScript(const std::string &fuselageID)
        {
            writeComment("Fuselage type - using XLocPercent");

            writeCommand("string xsec_surf = GetXSecSurf(\"" + fuselageID + "\", 0);");
            file << "\r\n";

            writeCommand("int num_xsecs = GetNumXSec(xsec_surf);");
            file << "\r\n";

            writeCommand("array<double> widthXSecValue(num_xsecs);");
            file << "\r\n";

            writeCommand("array<double> heightXSecValue(num_xsecs);");
            file << "\r\n";

            writeCommand("array<double> deltaXSecValue(num_xsecs);");
            file << "\r\n";

            writeCommand("string xsec_width = \"\";");
            file << "\r\n";

            writeCommand("string xsec_height = \"\";");
            file << "\r\n";

            writeCommand("string xsec_deltaX = \"\";");
            file << "\r\n";

            writeCommand("for (int ii = 0; ii <num_xsecs; ii++) {");
            file << "\r\n";

            writeCommand("xsec_width = GetXSec( xsec_surf, ii );");
            file << "\r\n";

            writeCommand("xsec_height = GetXSec( xsec_surf, ii );");
            file << "\r\n";

            writeCommand("widthXSecValue[ii] = GetXSecWidth( xsec_width );");
            file << "\r\n";

            writeCommand("heightXSecValue[ii] = GetXSecHeight( xsec_height );");
            file << "\r\n";

            writeCommand("xsec_deltaX =  GetXSecParm( xsec_width, \"XLocPercent\" );");
            file << "\r\n";

            writeCommand("deltaXSecValue[ii] = GetParmVal( xsec_deltaX );");
            file << "\r\n";

            writeCommand("}");
            file << "\r\n";

            writeCommand("Print(\"NumberOfSections:\" + num_xsecs );");
            file << "\r\n";

            writeCommand("for (int j = 0; j < num_xsecs; ++j) {");
            file << "\r\n";

            writeCommand("Print(\"width_\" + j + \": \" + widthXSecValue[j]);");
            file << "\r\n";

            writeCommand("Print(\"height_\" + j + \": \" + heightXSecValue[j]);");
            file << "\r\n";

            writeCommand("Print(\"deltaX_\" + j + \": \" + deltaXSecValue[j]);");
            file << "\r\n";

            writeCommand("}");
            file << "\r\n";
        }

        inline void writeStackTypeScript(const std::string &componentID)
        {
            writeComment("Stack type - using XDelta cumulative");

            writeCommand("string xsec_surf = GetXSecSurf(\"" + componentID + "\", 0);");
            file << "\r\n";

            writeCommand("int num_xsecs = GetNumXSec(xsec_surf);");
            file << "\r\n";

            writeCommand("array<double> widthXSecValue(num_xsecs);");
            file << "\r\n";

             writeCommand("array<double> heightXSecValue(num_xsecs);");
            file << "\r\n";

            writeCommand("array<double> deltaXSecValue(num_xsecs);");
            file << "\r\n";

            writeCommand("string xsec_width = \"\";");
            file << "\r\n";

            writeCommand("string xsec_height = \"\";");
            file << "\r\n";

            writeCommand("string xsec_deltaX = \"\";");
            file << "\r\n";

            writeCommand("for (int ii = 0; ii <num_xsecs; ii++) {");
            file << "\r\n";

            writeCommand("xsec_width = GetXSec( xsec_surf, ii );");
            file << "\r\n";

            writeCommand("xsec_height = GetXSec( xsec_surf, ii );");
            file << "\r\n";

            writeCommand("widthXSecValue[ii] = GetXSecWidth( xsec_width );");
            file << "\r\n";

            writeCommand("heightXSecValue[ii] = GetXSecHeight( xsec_height );");
            file << "\r\n";

            writeCommand("xsec_deltaX =  GetXSecParm( xsec_width, \"XDelta\" );");
            file << "\r\n";

            writeCommand("if(ii == 0){");
            file << "\r\n";

            writeCommand("deltaXSecValue[ii] = GetParmVal( xsec_deltaX );");
            file << "\r\n";

            writeCommand("}");
            file << "\r\n";

            writeCommand("else{");
            file << "\r\n";

            writeCommand("deltaXSecValue[ii] = deltaXSecValue[ii-1]+GetParmVal( xsec_deltaX );");
            file << "\r\n";

            writeCommand("}");
            file << "\r\n";

            writeCommand("}");
            file << "\r\n";

            writeCommand("Print(\"NumberOfSections:\" + num_xsecs );");
            file << "\r\n";

            writeCommand("for (int j = 0; j < num_xsecs; ++j) {");
            file << "\r\n";

            writeCommand("Print(\"width_\" + j + \": \" + widthXSecValue[j]);");
            file << "\r\n";

            writeCommand("Print(\"height_\" + j + \": \" + heightXSecValue[j]);");
            file << "\r\n";

            writeCommand("Print(\"deltaX_\" + j + \": \" + deltaXSecValue[j]);");
            file << "\r\n";

            writeCommand("}");
            file << "\r\n";
        }

        inline void writeCustomFuselageScript(const std::string &fuselageID)
        {
            writeComment("Custom fuselage type");

            writeCommand("string xsec_surf = GetXSecSurf(\"" + fuselageID + "\", 0);");
            file << "\r\n";

            writeCommand("string fuseLengthID = FindParm(\"" + fuselageID + "\",\"Length\",\"Design\");");
            file << "\r\n";

            writeCommand("string fuseDiamID = FindParm(\"" + fuselageID + "\",\"Diameter\",\"Design\");");
            file << "\r\n";

            writeCommand("string noseMultID = FindParm(\"" + fuselageID + "\",\"NoseMult\",\"Design\");");
            file << "\r\n";

            writeCommand("string aftMultID = FindParm(\"" + fuselageID + "\",\"AftMult\",\"Design\");");
            file << "\r\n";

            writeCommand("string aftWidthtID = FindParm(\"" + fuselageID + "\",\"AftWidth\",\"Design\");");
            file << "\r\n";

            writeCommand("double fuselageLength =  GetParmVal( fuseLengthID );");
            file << "\r\n";

            writeCommand("double fuseDiam       =  GetParmVal( fuseDiamID );");
            file << "\r\n";

            writeCommand("double noseLength     =  fuseDiam*GetParmVal( noseMultID );");
            file << "\r\n";

            writeCommand("double tailLength     =  fuseDiam*GetParmVal( aftMultID );");
            file << "\r\n";

            writeCommand("double cabinLength    =  fuselageLength-(noseLength+tailLength);");
            file << "\r\n";

            writeCommand("double tailDiam       =  GetParmVal( aftWidthtID );");
            file << "\r\n";

            writeCommand("Print(\"FUSE_LEN:\" + fuselageLength );");
            file << "\r\n";

            writeCommand("Print(\"FUSE_DIAM:\" + fuseDiam );");
            file << "\r\n";

            writeCommand("Print(\"NOSE_LEN:\" + noseLength );");
            file << "\r\n";

            writeCommand("Print(\"TAIL_LEN:\" + tailLength );");
            file << "\r\n";

            writeCommand("Print(\"CABIN_LEN:\" + cabinLength );");
            file << "\r\n";

            writeCommand("Print(\"TAIL_DIAM:\" + tailDiam );");
            file << "\r\n";
        }

        inline FuselageDiametersAndXStation parseFuselageDiametersOutput(
            const std::string &output,
            const std::string &fuselageType,
            bool isCustomFuselage,
            double fuseLength)
        {
            FuselageDiametersAndXStation result;

            std::regex numSections_regex(R"(NumberOfSections:\s*(-?\d+(?:\.\d+)?))");
            std::regex width_regex(R"(width_\d+:\s*(-?\d+(?:\.\d+)?))");
            std::regex height_regex(R"(height_\d+:\s*(-?\d+(?:\.\d+)?))");
            std::regex deltaX_regex(R"(deltaX_\d+:\s*(-?\d+(?:\.\d+)?))");

            std::smatch match;
            int numSections = 0;

            if (std::regex_search(output, match, numSections_regex))
            {
                numSections = std::stoi(match[1].str());
            }

            if (numSections > 0 && fuselageType != "Custom")
            {
                // Fuselage width vector
                auto widths_begin = std::sregex_iterator(output.begin(), output.end(), width_regex);
                auto widths_end = std::sregex_iterator();

                for (std::sregex_iterator i = widths_begin; i != widths_end; ++i)
                {
                    std::smatch m = *i;
                    result.allFuselageWidth.push_back(std::stod(m[1].str()));
                }

                // Fuselage height vector
                auto heights_begin = std::sregex_iterator(output.begin(), output.end(), height_regex);
                auto heights_end = std::sregex_iterator();

                for (std::sregex_iterator i = heights_begin; i != heights_end; ++i)
                {
                    std::smatch m = *i;
                    result.allFuselageHeight.push_back(std::stod(m[1].str()));
                }

                auto deltaX_begin = std::sregex_iterator(output.begin(), output.end(), deltaX_regex);
                auto deltaX_end = std::sregex_iterator();

                bool isFuselageType = (fuselageType == "Fuselage");

                for (std::sregex_iterator i = deltaX_begin; i != deltaX_end; ++i)
                {
                    std::smatch m = *i;
                    double value = std::stod(m[1].str());

                    if (isFuselageType && !isCustomFuselage)
                    {
                        value *= fuseLength;
                    }

                    result.xStation.push_back(value);
                }
            }

            else
            {

                if (isCustomFuselage)
                {
                    result = interpolateCustomFuselage(output);
                }
            }

            return result;
        }

        inline NacelleDiametersAndXStation parseNacelleDiametersOutput(
            const std::string &output,
            const std::string &nacelleType,
            double fuseLength)
        {
            NacelleDiametersAndXStation result;

            std::regex numSections_regex(R"(NumberOfSections:\s*(-?\d+(?:\.\d+)?))");
            std::regex width_regex(R"(width_\d+:\s*(-?\d+(?:\.\d+)?))");
            std::regex height_regex(R"(height_\d+:\s*(-?\d+(?:\.\d+)?))");
            std::regex deltaX_regex(R"(deltaX_\d+:\s*(-?\d+(?:\.\d+)?))");

            std::smatch match;
            int numSections = 0;

            if (std::regex_search(output, match, numSections_regex))
            {
                numSections = std::stoi(match[1].str());
            }

            if (numSections > 0 && nacelleType != "Custom")
            {

                // Nacelle widthes
                auto widths_begin = std::sregex_iterator(output.begin(), output.end(), width_regex);
                auto widths_end = std::sregex_iterator();

                for (std::sregex_iterator i = widths_begin; i != widths_end; ++i)
                {
                    std::smatch m = *i;
                    result.allNacelleWidth.push_back(std::stod(m[1].str()));
                }

                // Nacelle heights
                auto heights_begin = std::sregex_iterator(output.begin(), output.end(), height_regex);
                auto heights_end = std::sregex_iterator();

                for (std::sregex_iterator i = heights_begin; i != heights_end; ++i)
                {
                    std::smatch m = *i;
                    result.allNacelleHeights.push_back(std::stod(m[1].str()));
                }

                auto deltaX_begin = std::sregex_iterator(output.begin(), output.end(), deltaX_regex);
                auto deltaX_end = std::sregex_iterator();

                bool isNacelleType = (nacelleType != "Stack");
                std::vector<double> tempValueContainer;

                for (std::sregex_iterator i = deltaX_begin; i != deltaX_end; ++i)
                {
                    std::smatch m = *i;
                    double value = std::stod(m[1].str());

                    tempValueContainer.push_back(value);

                    if (isNacelleType)
                    {
                        value *= fuseLength;
                    }

                    result.xStation.push_back(value);
                }
            }

            return result;
        }

        inline FuselageDiametersAndXStation interpolateCustomFuselage(const std::string &output)
        {
            FuselageDiametersAndXStation result;

            std::regex fuseLenMatch(R"(FUSE_LEN:\s*(-?\d+(?:\.\d+)?))");
            std::regex fuseDiamMatch(R"(FUSE_DIAM:\s*(-?\d+(?:\.\d+)?))");
            std::regex noseLenMatch(R"(NOSE_LEN:\s*(-?\d+(?:\.\d+)?))");
            std::regex tailLenMatch(R"(TAIL_LEN:\s*(-?\d+(?:\.\d+)?))");
            std::regex cabinLenMatch(R"(CABIN_LEN:\s*(-?\d+(?:\.\d+)?))");
            std::regex tailDiamMatch(R"(TAIL_DIAM:\s*(-?\d+(?:\.\d+)?))");

            std::smatch match;
            double fuseLenVal = 0, fuseDiamVal = 0, noseLenVal = 0,
                   tailLenVal = 0, cabinLenVal = 0, tailDiamVal = 0;

            if (std::regex_search(output, match, fuseLenMatch))
                fuseLenVal = std::stod(match[1].str());
            if (std::regex_search(output, match, fuseDiamMatch))
                fuseDiamVal = std::stod(match[1].str());
            if (std::regex_search(output, match, noseLenMatch))
                noseLenVal = std::stod(match[1].str());
            if (std::regex_search(output, match, tailLenMatch))
                tailLenVal = std::stod(match[1].str());
            if (std::regex_search(output, match, cabinLenMatch))
                cabinLenVal = std::stod(match[1].str());
            if (std::regex_search(output, match, tailDiamMatch))
                tailDiamVal = std::stod(match[1].str());

            std::vector<double> noseDiam, deltaXNose;
            for (int i = 1; i <= 10; i++)
            {
                double t = i * 0.1;
                double x = t * noseLenVal;
                double diam = fuseDiamVal * t;

                noseDiam.push_back(diam);
                deltaXNose.push_back(x);
            }

            std::vector<double> tailDiam, deltaXTail;
            double tailStart = cabinLenVal + noseLenVal;
            double tailEnd = fuseLenVal;

            for (int i = 0; i < 10; i++)
            {
                double t = i / 9.0;
                double x = tailStart + t * (tailEnd - tailStart);
                double diam = fuseDiamVal + t * (tailDiamVal - fuseDiamVal);

                tailDiam.push_back(diam);
                deltaXTail.push_back(x);
            }

            result.allFuselageWidth.push_back(0.0);
            result.xStation.push_back(0.0);

            result.allFuselageWidth.insert(result.allFuselageWidth.end(),
                                               noseDiam.begin(), noseDiam.end());
            result.xStation.insert(result.xStation.end(), deltaXNose.begin(), deltaXNose.end());

            result.allFuselageWidth.insert(result.allFuselageWidth.end(),
                                               tailDiam.begin(), tailDiam.end());
            result.xStation.insert(result.xStation.end(), deltaXTail.begin(), deltaXTail.end());

            return result;
        }

    public:
        inline DiametersExtractor(const std::string &parentFolderPath = "")
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

        inline ~DiametersExtractor()
        {
            if (file.is_open())
            {
                file.close();
            }
        }

        inline FuselageDiametersAndXStation extractFuselageDiameters(
            const std::string &nameOfAircraft,
            const AircraftGeometryData &geomData,
            double fuseLength)
        {
            std::string fuselageID;
            std::string fuselageType;
            std::string fuselageNameOfComponent;
            bool isCustomFuselage = false;

            for (const auto &geom : geomData.allGeoms)
            {
                std::string lowerName = geom.nameOfComponent;
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

                if (lowerName == "fuselage" || lowerName == "transportfuse")
                {
                    fuselageID = geom.id;
                    fuselageType = geom.name;
                    fuselageNameOfComponent = geom.nameOfComponent;

                    if (fuselageType != "Fuselage" && fuselageType != "Stack")
                    {
                        isCustomFuselage = true;
                    }

                    break;
                }
            }

            if (fuselageID.empty())
            {
                throw std::runtime_error("Fuselage not found in geometry data");
            }

            std::string scriptFilename = nameOfAircraft + "_GetDiametersAndDeltaXToFuselage.vspscript";
            createDiametersScript(scriptFilename, nameOfAircraft, fuselageID, fuselageType, isCustomFuselage);

            std::string command = "vspscript.exe -script " + scriptFilename;
            std::string output = executeCommand(command);

            FuselageDiametersAndXStation result = parseFuselageDiametersOutput(output, fuselageType, isCustomFuselage, fuseLength);

            std::filesystem::remove(scriptFilename);

            return result;
        }


        inline NacelleDiametersAndXStation extractNacelleDiameters(
            const std::string &nameOfAircraft,
            const AircraftGeometryData &geomData,
            double nacelleLength)
        {
            std::string nacelleID;
            std::string nacelleType;
            std::string nacelleNameOfComponent;
            bool isCustomNacelle = false;

            for (const auto &geom : geomData.allGeoms)
            {
                std::string lowerName = geom.nameOfComponent;
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

                if (lowerName.substr(0, 3) == "nac")
                {
                    nacelleID = geom.id;
                    nacelleType = geom.name;
                    nacelleNameOfComponent = geom.nameOfComponent;

                    if (nacelleType != "Stack")
                    {
                        isCustomNacelle = true;
                    }

                    break;
                }
            }

            if (nacelleID.empty())
            {
                throw std::runtime_error("Nacelle not found in geometry data");
            }

            std::string scriptFilename = nameOfAircraft + "_GetDiametersAndDeltaXToNacelle.vspscript";
            createDiametersScript(scriptFilename, nameOfAircraft, nacelleID, nacelleType, isCustomNacelle);

            std::string command = "vspscript.exe -script " + scriptFilename;
            std::string output = executeCommand(command);

            NacelleDiametersAndXStation result = parseNacelleDiametersOutput(output, nacelleType,nacelleLength);

            std::filesystem::remove(scriptFilename);

            return result;
        }
    };

} // namespace VSPGEOMTRYEXTRACTOR
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace VSP {

struct AeroSettings {
    double altitude;
    double Sref;
    double Cref;
    double Bref;
    double X_cg, Y_cg, Z_cg;
    double Mach;
    std::vector<double> AoA;
    std::vector<double> Beta;
    double Vinf;
    double rho;
    double ReCref;
    double ClMax;
    double MaxTurningAngle;
    std::string Symmetry;  // "YES", "NO", "X", "Y", "Z"
    double FarDist;
    int NumWakeNodes;
    int WakeIters;
    std::string KT;  // "NO", "YES"
    int StabilityType;
    
    std::string IncludePropToAnlysis = "No";
    std::vector<double> rpm;
    std::vector<double> xloc, yloc, zloc;
    std::vector<double> xdir, ydir, zdir;
    std::vector<double> dia, hub;
    std::vector<double> ct, cp;
};

struct ControlSurface {
    std::string name;
    std::vector<std::string> surfaces;
    std::vector<int> gains;
    double deflection;
};

class VSPAeroGenerator {
private:
    std::ofstream file;
    std::string name;
    
    /**
     * @brief Writes a comma-separated vector parameter.
     * @param paramName Parameter label written before the equals sign.
     * @param values Numeric values written as a comma-separated list.
     */
    inline void writeVector(const std::string& paramName, const std::vector<double>& values) {
        file << paramName << " = ";
        for (size_t i = 0; i < values.size(); i++) {
            file << values[i];
            if (i < values.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }
    
public:
    /**
     * @brief Opens the output file named "<aircraftName>_DegenGeom.vspaero".
     * @param aircraftName Aircraft name used for file naming.
     * @throws std::runtime_error If the output file cannot be opened.
     */
    inline VSPAeroGenerator(const std::string& aircraftName) : name(aircraftName) {
        std::string filename = aircraftName + "_DegenGeom.vspaero";
        file.open(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
    }
    
    /**
     * @brief Closes the output file if still open.
     */
    inline ~VSPAeroGenerator() {
        close();
    }
    
    /**
     * @brief Closes the output file stream.
     */
    inline void close() {
        if (file.is_open()) {
            file.close();
        }
    }
    
    /**
     * @brief Writes general aerodynamic settings.
     * @param settings Settings structure that contains all aerodynamic data to write in the .vspaero file.
     */
    inline void writeSettings(const AeroSettings& settings) {
        file << "Sref = " << settings.Sref << "\n";
        file << "Cref = " << settings.Cref << "\n";
        file << "Bref = " << settings.Bref << "\n";
        file << "X_cg = " << settings.X_cg << "\n";
        file << "Y_cg = " << settings.Y_cg << "\n";
        file << "Z_cg = " << settings.Z_cg << "\n";
        file << "Mach = " << settings.Mach << "\n";
        
        writeVector("AoA", settings.AoA);
        writeVector("Beta", settings.Beta);
        
        file << "Vinf = " << settings.Vinf << "\n";
        file << "Rho = " << settings.rho << "\n";
        file << "ReCref = " << settings.ReCref << "\n";
        file << "ClMax = " << settings.ClMax << "\n";
        file << "MaxTurningAngle = " << settings.MaxTurningAngle << "\n";
        file << "Symmetry = " << settings.Symmetry << "\n";
        file << "FarDist = " << settings.FarDist << "\n";
        file << "NumWakeNodes = " << settings.NumWakeNodes << "\n";
        file << "WakeIters = " << settings.WakeIters << "\n";
    }
    
    /**
     * @brief Writes rotor blocks when propeller analysis is enabled.
     * @param settings Settings structure that contains all aerodynamic data to write in the .vspaero file.
     * @param config Configuration string checked for propeller presence.
     */
    inline void writeRotors(const AeroSettings& settings, const std::string& config) {
        if (config.find('P') == std::string::npos || 
            settings.IncludePropToAnlysis != "Yes") {
            return;
        }
        
        std::vector<int> enabledRotors;
        for (size_t i = 0; i < settings.rpm.size(); i++) {
            if (settings.rpm[i] > 0) {
                enabledRotors.push_back(i);
            }
        }
        
        int nRot = enabledRotors.size();
        file << "NumberOfRotors = " << nRot << "\n";
        
        if (nRot > 0) {
            for (int ii = 0; ii < nRot; ii++) {
                int idx = enabledRotors[ii];
                
                file << "PropElement_" << (ii + 1) << "\n";
                file << (ii + 1) << "\n";
                
                file << std::fixed << std::setprecision(4)
                     << settings.xloc[idx] << " "
                     << settings.yloc[idx] << " "
                     << settings.zloc[idx] << "\n";
                
                file << std::fixed << std::setprecision(4)
                     << settings.xdir[idx] << " "
                     << settings.ydir[idx] << " "
                     << settings.zdir[idx] << "\n";
                
                file << (settings.dia[idx] / 2.0) << "\n";
                file << (settings.hub[idx] / 2.0) << "\n";
                file << (-settings.rpm[idx]) << "\n";
                file << settings.ct[idx] << "\n";
                file << settings.cp[idx] << "\n";
            }
        }
    }
    
    /**
     * @brief Writes control surface group data.
     * @param controls List of control groups to serialize.
     */
    inline void writeControlSurfaces(const std::vector<ControlSurface>& controls) {
        if (controls.empty()) {
            return;
        }
        
        file << "NumberOfControlGroups = " << controls.size() << "\n";
        
        for (const auto& ctrl : controls) {
            file << ctrl.name << "\n";
            
            for (size_t i = 0; i < ctrl.surfaces.size(); i++) {
                file << ctrl.surfaces[i];
                if (i < ctrl.surfaces.size() - 1) {
                    file << ",";
                }
            }
            file << "\n";
            
            for (size_t i = 0; i < ctrl.gains.size(); i++) {
                file << ctrl.gains[i];
                if (i < ctrl.gains.size() - 1) {
                    file << ", ";
                }
            }
            file << "\n";
            
            file << ctrl.deflection << "\n";
        }
    }
    
    /**
     * @brief Writes the fixed footer and stability/correction options.
     * @param settings Settings structure that contains all aerodynamic data to write in the .vspaero file.
     */
    inline void writeFooter(const AeroSettings& settings) {
        file << "Preconditioner = Matrix\n";
        file << "Karman-Tsien Correction = " << settings.KT << "\n";
        file << "Stability Type = " << settings.StabilityType << "\n";
        file << "NumberOfQuadTrees = 1\n";
        file << "1 2 0.000000\n";
    }
};

} // namespace VSPS
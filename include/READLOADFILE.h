#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <stdexcept>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace VSPAero {

// ==================== STRUTTURE DATI ====================

struct WingSection {
    int wingId;      // 1=wing, 2=canard, 3=horizontal, 4=vertical
    int sectionId;   // ID della sezione lungo la superficie
    double S, Xavg, Yavg, Zavg, Chord, V_Vref;
    double Cl, Cd, Cs;
    double Cx, Cy, Cz;
    double Cmx, Cmy, Cmz;
    
    /**
     * @brief Default constructor.
     */
    WingSection() = default;
    
    /**
     * @brief Constructs a wing-section record with all parsed aerodynamic values.
     * @param wid Surface identifier.
     * @param sid Section identifier.
     * @param s Section area.
     * @param x X-average coordinate.
     * @param y Y-average coordinate.
     * @param z Z-average coordinate.
     * @param c Local chord.
     * @param v Local velocity ratio over reference velocity.
     * @param cl Lift coefficient.
     * @param cd Drag coefficient.
     * @param cs Side-force coefficient.
     * @param cx_ Force coefficient along X.
     * @param cy_ Force coefficient along Y.
     * @param cz_ Force coefficient along Z.
     * @param cmx_ Moment coefficient around X.
     * @param cmy_ Moment coefficient around Y.
     * @param cmz_ Moment coefficient around Z.
     */
    WingSection(int wid, int sid, double s, double x, double y, double z, double c, double v,
                double cl, double cd, double cs, double cx_, double cy_, double cz_,
                double cmx_, double cmy_, double cmz_)
        : wingId(wid), sectionId(sid), S(s), Xavg(x), Yavg(y), Zavg(z), Chord(c), V_Vref(v),
          Cl(cl), Cd(cd), Cs(cs), Cx(cx_), Cy(cy_), Cz(cz_),
          Cmx(cmx_), Cmy(cmy_), Cmz(cmz_) {}
};

struct ComponentData {
    std::string name;
    double S, Xavg, Yavg, Zavg, Chord, V_Vref;
    double Cl, Cd, Cs;
    double Cx, Cy, Cz;
    double Cmx, Cmy, Cmz;
};

struct CaseData {
    double AoA;
    double Beta;
    
    // Totali
    double Sref, Cref, Bref;
    double Xref, Yref, Zref;
    double Mach, U, Rho;
    
    // Coefficienti totali
    double CL, CDo, CDi, CDtot, CS;
    double CFx, CFy, CFz;
    double CMx, CMy, CMz;
    double L_D, E;
    
    // Dati per componente
    std::map<std::string, ComponentData> components;
    
    // Dati per sezione divisi per superficie
    std::vector<WingSection> allSections;       // Tutte le sezioni
    std::vector<WingSection> wingSections;      // Wing principale (wingId=1)
    std::vector<WingSection> canardSections;    // Canard (wingId=2)
    std::vector<WingSection> horizontalSections;// Horizontal tail (wingId=3)
    std::vector<WingSection> verticalSections;  // Vertical tail (wingId=4)
};

// ==================== LOADER ====================

class LODLoader {
private:
    std::string filename_;
    
    /**
     * @brief Converts a string to double while handling NaN/Inf-like tokens.
     * @param s Input numeric string.
     * @return Parsed value, or 0.0 for invalid/non-finite tokens.
     */
    inline double safeStod(const std::string& s) const {
        try {
            std::string lower = s;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            
            if (lower.find("nan") != std::string::npos || 
                lower.find("inf") != std::string::npos) {
                return 0.0;
            }
            return std::stod(s);
        } catch (...) {
            return 0.0;
        }
    }
    
    /**
     * @brief Extracts AoA from a text line.
     * @param line Input line.
     * @return Extracted AoA value, or 0.0 if not found.
     */
    inline double extractAoA(const std::string& line) const {
        std::regex re1(R"(AoA\s*=\s*([-+]?[0-9]*\.?[0-9]+))");
        std::smatch match;
        if (std::regex_search(line, match, re1)) {
            return std::stod(match[1]);
        }
        
        std::regex re2(R"(AoA_\s+([-+]?[0-9]*\.?[0-9]+))");
        if (std::regex_search(line, match, re2)) {
            return std::stod(match[1]);
        }
        
        return 0.0;
    }
    
    /**
     * @brief Extracts Beta from a text line.
     * @param line Input line.
     * @return Extracted Beta value, or 0.0 if not found.
     */
    inline double extractBeta(const std::string& line) const {
        std::regex re1(R"(Beta\s*=\s*([-+]?[0-9]*\.?[0-9]+))");
        std::smatch match;
        if (std::regex_search(line, match, re1)) {
            return std::stod(match[1]);
        }
        
        std::regex re2(R"(Beta_\s+([-+]?[0-9]*\.?[0-9]+))");
        if (std::regex_search(line, match, re2)) {
            return std::stod(match[1]);
        }
        
        return 0.0;
    }
    
    /**
     * @brief Extracts a numeric value associated with a key.
     * @param line Input line.
     * @param key Key used to build the extraction pattern.
     * @return Extracted value, or 0.0 if not found.
     */
    inline double extractValue(const std::string& line, const std::string& key) const {
        std::regex re(key + R"(\s*[=:_]\s*([-+]?[0-9]*\.?[0-9]+(?:[eE][-+]?[0-9]+)?))");
        std::smatch match;
        if (std::regex_search(line, match, re)) {
            return std::stod(match[1]);
        }
        return 0.0;
    }
    
    /**
     * @brief Trims leading and trailing whitespace.
     * @param str Input string.
     * @return Trimmed string.
     */
    inline std::string trim(const std::string& str) const {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
    
    /**
     * @brief Checks whether a line matches the Wing table header.
     * @param line Input line.
     * @return true if the line looks like a Wing table header.
     */
    inline bool isWingTableHeader(const std::string& line) const {
        return (line.find("Wing") != std::string::npos &&
                line.find("S") != std::string::npos &&
                line.find("Xavg") != std::string::npos);
    }
    
    /**
     * @brief Checks whether a line matches the Component table header.
     * @param line Input line.
     * @return true if the line looks like a Component table header.
     */
    inline bool isComponentTableHeader(const std::string& line) const {
        return (line.find("Comp") != std::string::npos &&
                line.find("S") != std::string::npos &&
                line.find("Xavg") != std::string::npos);
    }
    
    /**
     * @brief Checks whether a line starts with numeric data.
     * @param line Input line.
     * @return true if the first token can be parsed as a number.
     */
inline bool isDataLine(const std::string& line) const {
    if (line.empty()) return false;
    std::istringstream iss(line);
    double test;
    return static_cast<bool>(iss >> test);  // ✅ Cast esplicito a bool
}
    
public:
    /**
     * @brief Constructs a loader for a specific LOD file.
     * @param filename Input file path.
     */
    inline LODLoader(const std::string& filename) 
    : filename_(filename) {
        
    }
    
    /**
     * @brief Loads and parses all cases available in the file.
     * @return Vector containing all parsed cases.
     * @throws std::runtime_error If the file cannot be opened.
     */
    inline std::vector<CaseData> loadAll() const {
        std::ifstream file(filename_);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename_);
        }
        
        std::vector<CaseData> cases;
        std::string line;
        CaseData currentCase;
        bool inCase = false;
        bool inWingTable = false;
        bool inComponentTable = false;
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            // ==================== RILEVAMENTO NUOVO CASO ====================
            if (line.find("AoA_") != std::string::npos) {
                
                if (inCase) {
                    // Organizza sezioni per superficie
                    for (const auto& sec : currentCase.allSections) {
                        if (sec.wingId == 1) {
                            currentCase.wingSections.push_back(sec);
                        } else if (sec.wingId == 2) {
                            currentCase.canardSections.push_back(sec);
                        } else if (sec.wingId == 3) {
                            currentCase.horizontalSections.push_back(sec);
                        } else if (sec.wingId == 4) {
                            currentCase.verticalSections.push_back(sec);
                        }
                    }
                    cases.push_back(currentCase);
                }
                
                // Inizia nuovo caso
                currentCase = CaseData();
                inCase = true;
                inWingTable = false;
                inComponentTable = false;
                
                // Estrai AoA
                currentCase.AoA = extractAoA(line);
                
                continue;
            }
            
            if (!inCase) continue;
            
            // ==================== ESTRAZIONE PARAMETRI ====================
            if (line.find("Beta_") != std::string::npos) {
                currentCase.Beta = extractBeta(line);
            }
            
            if (line.find("Sref_") != std::string::npos) {
                currentCase.Sref = extractValue(line, "Sref_");
            }
            if (line.find("Cref_") != std::string::npos) {
                currentCase.Cref = extractValue(line, "Cref_");
            }
            if (line.find("Bref_") != std::string::npos) {
                currentCase.Bref = extractValue(line, "Bref_");
            }
            if (line.find("Xcg_") != std::string::npos) {
                currentCase.Xref = extractValue(line, "Xcg_");
            }
            if (line.find("Ycg_") != std::string::npos) {
                currentCase.Yref = extractValue(line, "Ycg_");
            }
            if (line.find("Zcg_") != std::string::npos) {
                currentCase.Zref = extractValue(line, "Zcg_");
            }
            if (line.find("Mach_") != std::string::npos) {
                currentCase.Mach = extractValue(line, "Mach_");
            }
            if (line.find("Vinf_") != std::string::npos) {
                currentCase.U = extractValue(line, "Vinf_");
            }
            if (line.find("Rho_") != std::string::npos) {
                currentCase.Rho = extractValue(line, "Rho_");
            }
            
            // ==================== RILEVAMENTO TABELLE ====================
            
            // Tabella Wing
            if (isWingTableHeader(line)) {
                inWingTable = true;
                inComponentTable = false;
                continue; // ✅ NON saltare la prossima riga
            }
            
            // Tabella Component
            if (isComponentTableHeader(line)) {
                inComponentTable = true;
                inWingTable = false;
                continue; // ✅ NON saltare la prossima riga
            }
            
            // ==================== LETTURA TABELLA WING ====================
            if (inWingTable) {
                // Termina tabella Wing se incontriamo:
                // - Riga vuota
                // - Riga con solo trattini
                // - Inizio tabella Component
                if (line.empty() || 
                    line.find("---") != std::string::npos ||
                    line.find("Comp") != std::string::npos) {
                    inWingTable = false;
                    
                    // Se è l'inizio della tabella Component, processiamola
                    if (isComponentTableHeader(line)) {
                        inComponentTable = true;
                    }
                    continue;
                }
                
                // Salta righe che non sono dati (come gli header ripetuti)
                if (!isDataLine(line)) {
                    continue;
                }
                
                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::string token;
                
                while (iss >> token) {
                    tokens.push_back(token);
                }
                
                if (tokens.size() >= 16) {
                    try {
                        int wingId = std::stoi(tokens[0]);
                        double S = safeStod(tokens[1]);
                        double Xavg = safeStod(tokens[2]);
                        double Yavg = safeStod(tokens[3]);
                        double Zavg = safeStod(tokens[4]);
                        double Chord = safeStod(tokens[5]);
                        double V_Vref = safeStod(tokens[6]);
                        double Cl = safeStod(tokens[7]);
                        double Cd = safeStod(tokens[8]);
                        double Cs = safeStod(tokens[9]);
                        double Cx = safeStod(tokens[10]);
                        double Cy = safeStod(tokens[11]);
                        double Cz = safeStod(tokens[12]);
                        double Cmx = safeStod(tokens[13]);
                        double Cmy = safeStod(tokens[14]);
                        double Cmz = safeStod(tokens[15]);
                        
                        int sectionId = currentCase.allSections.size();
                        
                        currentCase.allSections.emplace_back(
                            wingId, sectionId, S, Xavg, Yavg, Zavg, Chord, V_Vref,
                            Cl, Cd, Cs, Cx, Cy, Cz, Cmx, Cmy, Cmz
                        );
                        
                    } catch (const std::exception& e) {
                        // Salta righe invalide
                    }
                }
            }
            
            // ==================== LETTURA TABELLA COMPONENT ====================
            if (inComponentTable) {
                if (line.empty() || 
                    line.find("---") != std::string::npos ||
                    line.find("Total") != std::string::npos) {
                    inComponentTable = false;
                    continue;
                }
                
                // Salta righe che non sono dati
                if (!isDataLine(line)) {
                    continue;
                }
                
                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::string token;
                
                while (iss >> token) {
                    tokens.push_back(token);
                }
                
                if (tokens.size() >= 16) {
                    try {
                        ComponentData comp;
                        comp.name = tokens[0];
                        comp.S = safeStod(tokens[1]);
                        comp.Xavg = safeStod(tokens[2]);
                        comp.Yavg = safeStod(tokens[3]);
                        comp.Zavg = safeStod(tokens[4]);
                        comp.Chord = safeStod(tokens[5]);
                        comp.V_Vref = safeStod(tokens[6]);
                        comp.Cl = safeStod(tokens[7]);
                        comp.Cd = safeStod(tokens[8]);
                        comp.Cs = safeStod(tokens[9]);
                        comp.Cx = safeStod(tokens[10]);
                        comp.Cy = safeStod(tokens[11]);
                        comp.Cz = safeStod(tokens[12]);
                        comp.Cmx = safeStod(tokens[13]);
                        comp.Cmy = safeStod(tokens[14]);
                        comp.Cmz = safeStod(tokens[15]);
                        
                        currentCase.components[comp.name] = comp;
                        
                        if (comp.name == "wing") {
                            currentCase.CL = comp.Cl;
                            currentCase.CDi = comp.Cd;
                        }
                        
                    } catch (const std::exception& e) {
                        // Salta righe invalide
                    }
                }
            }
        }
        
        // ==================== SALVA ULTIMO CASO ====================
        if (inCase) {
            for (const auto& sec : currentCase.allSections) {
                if (sec.wingId == 1) {
                    currentCase.wingSections.push_back(sec);
                } else if (sec.wingId == 2) {
                    currentCase.canardSections.push_back(sec);
                } else if (sec.wingId == 3) {
                    currentCase.horizontalSections.push_back(sec);
                } else if (sec.wingId == 4) {
                    currentCase.verticalSections.push_back(sec);
                }
            }
            cases.push_back(currentCase);
        }
        
        file.close();
        return cases;
    }
    
    /**
     * @brief Loads a specific case matching AoA and Beta within tolerance.
     * @param targetAoA Target angle of attack.
     * @param targetBeta Target sideslip angle.
     * @param tolerance Matching tolerance for both values.
     * @return Matching case.
     * @throws std::runtime_error If no matching case is found.
     */
    inline CaseData loadCase(double targetAoA, double targetBeta = 0.0, double tolerance = 0.01) const {
        auto cases = loadAll();
        
        for (const auto& c : cases) {
            if (std::abs(c.AoA - targetAoA) < tolerance &&
                std::abs(c.Beta - targetBeta) < tolerance) {
                return c;
            }
        }
        
        throw std::runtime_error("Case not found: AoA=" + std::to_string(targetAoA) +
                                 ", Beta=" + std::to_string(targetBeta));
    }
    
    /**
     * @brief Prints a summary of parsed cases to standard output.
     */
    inline void printSummary() const {
        auto cases = loadAll();
        
        std::cout << "\n=== VSPAero LOD File Summary ===" << std::endl;
        std::cout << "File: " << filename_ << std::endl;
        std::cout << "Total cases: " << cases.size() << std::endl;
        std::cout << "\nCase list:" << std::endl;
        std::cout << "  AoA    Beta   Wing  Canard Horiz  Vert    CL" << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;
        
        for (const auto& c : cases) {
            std::cout << std::fixed << std::setprecision(2)
                      << "  " << std::setw(6) << c.AoA
                      << " " << std::setw(6) << c.Beta
                      << " " << std::setw(5) << c.wingSections.size()
                      << " " << std::setw(6) << c.canardSections.size()
                      << " " << std::setw(6) << c.horizontalSections.size()
                      << " " << std::setw(5) << c.verticalSections.size()
                      << " " << std::setw(7) << c.CL
                      << std::endl;
        }
    }
};

} // namespace VSPAero
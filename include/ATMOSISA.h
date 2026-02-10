#pragma once

#include <cmath>
#include <stdexcept>
#include <array>
#include <tuple>
#include <vector>

namespace Atmosphere {

/**
 * @brief International Standard Atmosphere (ISA) Calculator - FULL VERSION
 * 
 * Complete implementation matching MATLAB's atmosisa function
 * Valid altitude range: -5,000 m to 86,000 m
 * 
 * References:
 * - ISO 2533:1975 Standard Atmosphere
 * - U.S. Standard Atmosphere, 1976
 */
class ISA {
public:
    // Physical constants
    static constexpr double R = 287.05287;      // Gas constant [J/(kg·K)]
    static constexpr double gamma = 1.4;        // Ratio of specific heats
    static constexpr double g0 = 9.80665;       // Gravitational acceleration [m/s²]
    static constexpr double R_earth = 6356766.0; // Earth radius [m]
    
    // Sea level conditions
    static constexpr double T0 = 288.15;        // Temperature [K]
    static constexpr double P0 = 101325.0;      // Pressure [Pa]
    static constexpr double rho0 = 1.225;       // Density [kg/m³]
    static constexpr double a0 = 340.294;       // Speed of sound [m/s]
    
    // Sutherland's law constants for viscosity
    static constexpr double C1 = 1.458e-6;      // [kg/(m·s·K^0.5)]
    static constexpr double S = 110.4;          // Sutherland temperature [K]
    
    /**
     * @brief Complete atmospheric properties structure
     */
    struct Properties {
        double T;           // Temperature [K]
        double a;           // Speed of sound [m/s]
        double P;           // Pressure [Pa]
        double rho;         // Density [kg/m³]
        double mu;          // Dynamic viscosity [Pa·s]
        double nu;          // Kinematic viscosity [m²/s]
        double delta;       // Pressure ratio P/P0 [-]
        double theta;       // Temperature ratio T/T0 [-]
        double sigma;       // Density ratio rho/rho0 [-]
    };
    
    /**
     * @brief Calculate complete atmospheric properties at given altitude
     * 
     * @param h Geometric altitude [m] (range: -5000 to 86000)
     * @return Properties Complete atmospheric data
     * @throws std::out_of_range if altitude is outside valid range
     */
    static inline Properties calculate(double h) {
        // Validate altitude range
        if (h < -5000.0 || h > 86000.0) {
            throw std::out_of_range("Altitude must be between -5000 m and 86000 m");
        }
        
        // Convert geometric altitude to geopotential altitude
        double H = geometricToGeopotential(h);
        
        // Determine atmospheric layer
        int layer = getLayer(H);
        
        // Calculate temperature
        double T = calculateTemperature(H, layer);
        
        // Calculate pressure
        double P = calculatePressure(H, T, layer);
        
        // Calculate density
        double rho = P / (R * T);
        
        // Calculate speed of sound
        double a = std::sqrt(gamma * R * T);
        
        // Calculate dynamic viscosity (Sutherland's law)
        double mu = C1 * std::pow(T, 1.5) / (T + S);
        
        // Calculate kinematic viscosity
        double nu = mu / rho;
        
        // Calculate ratios
        double delta = P / P0;
        double theta = T / T0;
        double sigma = rho / rho0;
        
        return {T, a, P, rho, mu, nu, delta, theta, sigma};
    }
    
    /**
     * @brief Calculate only temperature at given altitude
     */
    static inline double temperature(double h) {
        return calculate(h).T;
    }
    
    /**
     * @brief Calculate only speed of sound at given altitude
     */
    static inline double speedOfSound(double h) {
        return calculate(h).a;
    }
    
    /**
     * @brief Calculate only pressure at given altitude
     */
    static inline double pressure(double h) {
        return calculate(h).P;
    }
    
    /**
     * @brief Calculate only density at given altitude
     */
    static inline double density(double h) {
        return calculate(h).rho;
    }
    
    /**
     * @brief Calculate only dynamic viscosity at given altitude
     */
    static inline double viscosity(double h) {
        return calculate(h).mu;
    }
    
    /**
     * @brief Calculate only kinematic viscosity at given altitude
     */
    static inline double kinematicViscosity(double h) {
        return calculate(h).nu;
    }
    
    /**
     * @brief Calculate pressure ratio at given altitude
     */
    static inline double pressureRatio(double h) {
        return calculate(h).delta;
    }

    /**
     * @brief Calculate density ratio at given altitude
     */
    static inline double densityRatio(double h) {
        return calculate(h).sigma;
    }
    
private:
    // Layer boundaries (geopotential altitude) [m]
    static constexpr std::array<double, 9> H_base = {
        -5000.0,    // Layer 0: Below sea level
        0.0,        // Layer 1: Troposphere
        11000.0,    // Layer 2: Tropopause
        20000.0,    // Layer 3: Stratosphere 1
        32000.0,    // Layer 4: Stratosphere 2
        47000.0,    // Layer 5: Stratopause
        51000.0,    // Layer 6: Mesosphere 1
        71000.0,    // Layer 7: Mesosphere 2
        84852.0     // Layer 8: Upper mesosphere
    };
    
    // Temperature lapse rates [K/m]
    static constexpr std::array<double, 9> lapse_rate = {
        0.0065,     // Layer 0
        0.0065,     // Layer 1
        0.0,        // Layer 2 (isothermal)
        -0.001,     // Layer 3
        -0.0028,    // Layer 4
        0.0,        // Layer 5 (isothermal)
        0.0028,     // Layer 6
        0.002,      // Layer 7
        0.0         // Layer 8 (isothermal)
    };
    
    // Base temperatures [K]
    static constexpr std::array<double, 9> T_base = {
        320.65,     // Layer 0: T0 + 32.5K
        288.15,     // Layer 1: Sea level
        216.65,     // Layer 2: Tropopause
        216.65,     // Layer 3
        228.65,     // Layer 4
        270.65,     // Layer 5: Stratopause
        270.65,     // Layer 6
        214.65,     // Layer 7
        186.946     // Layer 8
    };
    
    // Base pressures [Pa] - Calculated from standard atmosphere tables
    static constexpr std::array<double, 9> P_base = {
        177687.0,   // Layer 0
        101325.0,   // Layer 1: Sea level
        22632.1,    // Layer 2
        5474.89,    // Layer 3
        868.019,    // Layer 4
        110.906,    // Layer 5
        66.9389,    // Layer 6
        3.95642,    // Layer 7
        0.373384    // Layer 8
    };
    
    /**
     * @brief Convert geometric altitude to geopotential altitude
     */
    static inline double geometricToGeopotential(double h) {
        return R_earth * h / (R_earth + h);
    }
    
    /**
     * @brief Determine atmospheric layer for given geopotential altitude
     */
    static inline int getLayer(double H) {
        for (int i = 8; i >= 0; --i) {
            if (H >= H_base[i]) {
                return i;
            }
        }
        return 0;
    }
    
    /**
     * @brief Calculate temperature at given geopotential altitude
     */
    static inline double calculateTemperature(double H, int layer) {
        double dH = H - H_base[layer];
        return T_base[layer] - lapse_rate[layer] * dH;
    }
    
    /**
     * @brief Calculate pressure at given geopotential altitude
     */
    static inline double calculatePressure(double H, double T, int layer) {
        double dH = H - H_base[layer];
        double L = lapse_rate[layer];
        double T_b = T_base[layer];
        double P_b = P_base[layer];
        
        if (std::abs(L) < 1e-10) {
            // Isothermal layer
            return P_b * std::exp(-g0 * dH / (R * T_b));
        } else {
            // Gradient layer
            double exponent = g0 / (R * L);
            return P_b * std::pow(T / T_b, exponent);
        }
    }
};

/**
 * @brief MATLAB-compatible atmosisa function (basic version)
 * 
 * Usage:
 *   auto [T, a, P, rho] = atmosisa(h);
 */
inline std::tuple<double, double, double, double> atmosisa(double h) {
    auto props = ISA::calculate(h);
    return {props.T, props.a, props.P, props.rho};
}


  ///@brief Extended atmosisa with all outputs
  
  // Usage:
  // auto props = atmosisa_full(h);
  // std::cout << "Viscosity: " << props.mu << std::endl;
 
inline ISA::Properties atmosisa_full(double h) {
    return ISA::calculate(h);
}


 /// @brief Vectorized atmosisa for multiple altitudes
 
template<typename Container>
inline std::vector<ISA::Properties> atmosisa(const Container& altitudes) {
    std::vector<ISA::Properties> results;
    results.reserve(altitudes.size());
    
    for (double h : altitudes) {
        results.push_back(ISA::calculate(h));
    }
    
    return results;
}

} // namespace Atmosphere



// Le funzioni dichiarate come 'inline' suggeriscono al compilatore di sostituire la chiamata
// alla funzione con il suo corpo, ove possibile, per ridurre l'overhead della chiamata stessa.
// Questo può migliorare le prestazioni per funzioni brevi e frequentemente chiamate.
// Inoltre, l'uso di 'inline' in header file evita errori di linkage quando la funzione viene
// definita in più unità di traduzione. Tuttavia, la decisione finale sull'inlining spetta sempre
// al compilatore.

// Le variabili dichiarate come 'static constexpr' sono costanti a livello di classe:
// 'constexpr' indica che il valore è noto a compile-time e può essere usato in espressioni costanti,
// mentre 'static' fa sì che la variabile sia condivisa da tutte le istanze della classe e non occupi
// spazio aggiuntivo in ciascun oggetto. Questo è utile per definire costanti fisiche o parametri fissi
// ovvero, non serve un oggetto per accedervi.
#pragma once

#include "BUILDAIRCRAFT.h"
#include "EnumAircraftCategory.h"
#include "EnumAircraftEngineType.h"
#include "Interpolant.h"
#include "RegressionMethod.h"
#include "ConvMass.h"
#include "ConvLength.h"
#include "EnumMASS.h"
#include "EnumLENGTH.h"
#include "VSPScriptGenerator.h"
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>


class MomentOfInertia
{
public:

    // --------------------------------------------------------
    // Result struct
    // --------------------------------------------------------
    struct Result
    {
        double Ixx = 0.0;  // Roll  moment of inertia [kg*m^2]
        double Iyy = 0.0;  // Pitch moment of inertia [kg*m^2]
        double Izz = 0.0;  // Yaw   moment of inertia [kg*m^2]
    };

    // --------------------------------------------------------
    // Constructor — takes the builder by const reference and
    // reads category, engine type, and MTOW from commonData
    // --------------------------------------------------------
    explicit MomentOfInertia(const BuildAircraft& builder)
        : aircraftCategory  (builder.getCommonData().getAircraftCategory())
        , engineType(builder.getCommonData().getAircraftEngineType())
        , WTO    (builder.getCommonData().getWTO())
    {}

    // --------------------------------------------------------
    // Main entry point
    //   wingSpan_m        : wing span [m]
    //   fuselageLength_m  : overall fuselage length [m]
    // Returns Result with Ixx, Iyy, Izz in [kg*m^2]
    // --------------------------------------------------------
    Result compute(const VSP::Wing& wing, const VSP::Fuselage& fuselage) const
    {
        // --- Unit conversions (Imperial, as per Roskam) ---
        // kg  → lb  via ConvMass
        const double weightAircaftPounds = ConvMass(Mass::KG, Mass::LB, WTO).getConvertedValues();
        // m   → ft  via ConvLength
        const double wingSpanFeet    = ConvLength(Length::M, Length::FT, wing.totalProjectedSpan).getConvertedValues();
        const double fuselageLengthFeet   = ConvLength(Length::M, Length::FT, fuselage.length).getConvertedValues();
        const double eccentricityFactor    = 0.5 * (wingSpanFeet + fuselageLengthFeet);   // Roskam e-coefficient

        // --- Select coefficient tables based on category + engine type ---
        RoskamTable tbl = selectTable();

        // --- Interpolate radii of gyration ---
        double Rxbar = interpPchipLinearExtrap(tbl.weight, tbl.Rx, weightAircaftPounds);
        double Rybar = interpPchipLinearExtrap(tbl.weight, tbl.Ry, weightAircaftPounds);
        double Rzbar = interpPchipLinearExtrap(tbl.weight, tbl.Rz, weightAircaftPounds);

        // --- Compute inertias (1.3558 = 14.5939 * 0.3048^2 : slug*ft^2 -> kg*m^2) ---
        Result res;
        res.Ixx = SLUG_FT2_TO_KGM2 * (wingSpanFeet  * wingSpanFeet  * weightAircaftPounds * Rxbar * Rxbar / (4.0 * G_FT_S2));
        res.Iyy = SLUG_FT2_TO_KGM2 * (fuselageLengthFeet * fuselageLengthFeet * weightAircaftPounds * Rybar * Rybar / (4.0 * G_FT_S2));
        res.Izz = SLUG_FT2_TO_KGM2 * (eccentricityFactor  * eccentricityFactor  * weightAircaftPounds * Rzbar * Rzbar / (4.0 * G_FT_S2));
        return res;
    }

private:

    // --------------------------------------------------------
    // Physical / conversion constants
    // KG_TO_LBS and M_TO_FT are handled by ConvMass / ConvLength.
    // SLUG_FT2_TO_KGM2 is a composite unit (slug·ft² → kg·m²) not
    // covered by those classes, so it remains as a local constant.
    // --------------------------------------------------------
    static constexpr double G_FT_S2          = 32.174;   // gravitational acceleration [ft/s^2]
    static constexpr double SLUG_FT2_TO_KGM2 = 1.3558;   // slug·ft^2 → kg·m^2  (= 14.5939 * 0.3048^2)

    // --------------------------------------------------------
    // Stored aircraft properties (from builder)
    // --------------------------------------------------------
    AircraftCategory   aircraftCategory;
    AircraftEngineType engineType;
    double             WTO;

    // --------------------------------------------------------
    // Roskam coefficient table
    // --------------------------------------------------------
    struct RoskamTable
    {
        std::vector<double> weight;  // [lbs]
        std::vector<double> Rx;
        std::vector<double> Ry;
        std::vector<double> Rz;
    };

    // --------------------------------------------------------
    // Table selector — mirrors the MATLAB if/elseif chain
    // --------------------------------------------------------
    RoskamTable selectTable() const
    {
        const bool isGAorUAV = (aircraftCategory == AircraftCategory::GENERAL_AVIATION ||
                                aircraftCategory == AircraftCategory::UAV);

        // The selected tables are based on C++ aggregate initialization:
        // the struct members are initialized in-place, in declaration order.
        //   First  {...} → weight vector
        //   Second {...} → Rx vector
        //   Third  {...} → Ry vector
        //   Fourth {...} → Rz vector

        // --- GUAV single engine ---
        if (isGAorUAV && engineType == AircraftEngineType::SINGLE_ENGINE)
        {
            return {
                { 3125, 1127, 1477, 1761, 1885, 2700 },  // weight in lbs
                { 0.248, 0.254, 0.242, 0.212, 0.342, 0.222 }, // Rx
                { 0.338, 0.405, 0.386, 0.362, 0.397, 0.356 }, // Ry
                { 0.393, 0.418, 0.403, 0.394, 0.393, 0.379 }  // Rz
            };
        }

        // --- GUAV twin engine ---
        if (isGAorUAV && engineType == AircraftEngineType::TWIN_ENGINE)
        {
            return {
                { 4880, 4000, 6500, 9000, 5000, 6200, 4851, 8400, 5642, 9925 },
                { 0.260, 0.251, 0.240, 0.232, 0.414, 0.373, 0.324, 0.340, 0.285, 0.256 },
                { 0.329, 0.327, 0.313, 0.360, 0.278, 0.269, 0.318, 0.284, 0.345, 0.212 },
                { 0.399, 0.391, 0.384, 0.396, 0.502, 0.461, 0.446, 0.445, 0.429, 0.336 }
            };
        }

        // --- Regional Turboprop (RT) ---
        if (aircraftCategory == AircraftCategory::REGIONAL_TURBOPROP)
        {
            return {
                { 38500, 12500 },
                { 0.235, 0.203 },
                { 0.363, 0.326 },
                { 0.416, 0.350 }
            };
        }

        // --- Piston Propeller (PP) ---
        if (aircraftCategory == AircraftCategory::GENERAL_AVIATION && engineType == AircraftEngineType::PISTON_PROPELLER)
        {
            return {
                { 107000, 120000, 146500, 60360, 97200, 49500, 45000, 41800, 44500, 20000 },
                { 0.300, 0.316, 0.371, 0.250, 0.322, 0.278, 0.272, 0.286, 0.308, 0.225 },
                { 0.298, 0.336, 0.278, 0.320, 0.324, 0.314, 0.378, 0.351, 0.345, 0.303 },
                { 0.426, 0.448, 0.473, 0.388, 0.456, 0.400, 0.444, 0.443, 0.457, 0.346 }
            };
        }

        // --- Turboprop (TP) ---
        if (aircraftCategory == AircraftCategory::GENERAL_AVIATION && engineType == AircraftEngineType::TURBOPROP)
        {
            return {
                { 103000, 187000, 116000 },
                { 0.317, 0.330, 0.394 },
                { 0.356, 0.357, 0.341 },
                { 0.455, 0.478, 0.497 }
            };
        }

        // --- Transport Jet ---
        if (aircraftCategory == AircraftCategory::TRANSPORT_JET)
        {
            return {
                { 6505,  12000, 7036,  13500,
                  185000,191500,240000,245000,165000,
                  89000, 180000,100000,113000,
                  62000, 800000,350000,74000, 210000 },
                { 0.236, 0.306, 0.243, 0.293,
                  0.320, 0.322, 0.335, 0.305, 0.249, 0.247,
                  0.248, 0.240, 0.246, 0.264, 0.290, 0.332, 0.242, 0.301 },
                { 0.384, 0.303, 0.400, 0.312, 0.342, 0.339,
                  0.338, 0.334, 0.375, 0.442, 0.394, 0.451, 0.382,
                  0.456, 0.329, 0.380, 0.360, 0.349 },
                { 0.430, 0.423, 0.447, 0.420, 0.465, 0.464,
                  0.473, 0.472, 0.452, 0.518, 0.502, 0.550, 0.456,
                  0.517, 0.445, 0.508, 0.435, 0.434 }
            };
        }

        throw std::runtime_error("MomentOfInertia: unsupported aircraft category / engine type combination.");
    }

    // ============================================================
    // PCHIP interpolation inside the data range,
    // linear extrapolation outside — mirrors MATLAB helper.
    //
    // x and y need not be sorted on input (they are sorted here).
    // Duplicate x values are averaged.
    // ============================================================
    static double interpPchipLinearExtrap(std::vector<double> x,
                                          std::vector<double> y,
                                          double xq)
    {
        const std::size_t n = x.size();
        if (n == 0) throw std::invalid_argument("interpPchipLinearExtrap: empty data");
        if (n == 1) return y[0];

        // --- Sort by x ---
        std::vector<std::size_t> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        std::sort(idx.begin(), idx.end(), [&](std::size_t a, std::size_t b){ return x[a] < x[b]; });

        std::vector<double> xs(n), ys(n);
        for (std::size_t i = 0; i < n; ++i) { xs[i] = x[idx[i]]; ys[i] = y[idx[i]]; }

        // --- Merge duplicates (average y at same x) ---
        std::vector<double> xu, yu;
        xu.push_back(xs[0]);
        yu.push_back(ys[0]);
        for (std::size_t i = 1; i < n; ++i)
        {
            if (std::abs(xs[i] - xu.back()) < 1e-12)
                yu.back() = 0.5 * (yu.back() + ys[i]);
            else { xu.push_back(xs[i]); yu.push_back(ys[i]); }
        }

        const std::size_t m = xu.size();
        if (m == 1) return yu[0];

        const double xmin = xu.front();
        const double xmax = xu.back();

        // --- Linear extrapolation outside via Interpolant (two boundary points) ---
        // Mirrors MATLAB: interp1(xs, ys, xq, 'linear', 'extrap')
        // Only the two nearest boundary points are passed so the slope matches
        // exactly the local edge interval, consistent with MATLAB behaviour.
        if (xq <= xmin)
        {
            Interpolant leftExtrap({ xu[0], xu[1] }, { yu[0], yu[1] },
                                   /*degree=*/1, RegressionMethod::LINEAR);
            return leftExtrap.getYValueFromRegression(xq);
        }
        if (xq >= xmax)
        {
            Interpolant rightExtrap({ xu[m-2], xu[m-1] }, { yu[m-2], yu[m-1] },
                                    /*degree=*/1, RegressionMethod::LINEAR);
            return rightExtrap.getYValueFromRegression(xq);
        }

        // --- PCHIP inside ---
        // Step 1: interval lengths and slopes
        std::vector<double> h(m-1), delta(m-1);
        for (std::size_t i = 0; i < m-1; ++i)
        {
            h[i]     = xu[i+1] - xu[i];
            delta[i] = (yu[i+1] - yu[i]) / h[i];
        }

        // Step 2: derivatives d[i] using Fritsch-Carlson conditions
        std::vector<double> d(m);

        // Endpoints: one-sided three-point formula
        d[0]   = pchipEndDerivative(h[0], h[1], delta[0], delta[1]);
        d[m-1] = pchipEndDerivative(h[m-2], h[m-3 < m-2 ? m-3 : m-2],
                                    delta[m-2],
                                    m >= 3 ? delta[m-3] : delta[m-2]);
        // flip sign for the right endpoint (mirrored one-sided)
        d[m-1] = pchipEndDerivative(h[m-2],
                                    m >= 3 ? h[m-3] : h[m-2],
                                    delta[m-2],
                                    m >= 3 ? delta[m-3] : delta[m-2],
                                    /*rightEndpoint=*/true);

        // Interior points
        for (std::size_t i = 1; i < m-1; ++i)
        {
            if (delta[i-1] * delta[i] <= 0.0)
            {
                // Local extremum — set derivative to zero
                d[i] = 0.0;
            }
            else
            {
                // Weighted harmonic mean (preserves shape)
                double w1 = 2.0*h[i]   + h[i-1];
                double w2 = 2.0*h[i-1] + h[i];
                d[i] = (w1 + w2) / (w1/delta[i-1] + w2/delta[i]);
            }
        }

        // Step 3: locate interval and evaluate cubic Hermite
        std::size_t k = 0;
        while (k < m-2 && xq > xu[k+1]) ++k;

        double t  = (xq - xu[k]) / h[k];
        double t2 = t * t;
        double t3 = t2 * t;

        // Hermite basis functions
        double h00 =  2*t3 - 3*t2 + 1;
        double h10 =    t3 - 2*t2 + t;
        double h01 = -2*t3 + 3*t2;
        double h11 =    t3 -   t2;

        return h00*yu[k] + h10*h[k]*d[k] + h01*yu[k+1] + h11*h[k]*d[k+1];
    }

    // --------------------------------------------------------
    // One-sided endpoint derivative for PCHIP
    //   hLeft, hRight : interval widths adjacent to endpoint
    //   dLeft, dRight : slopes on those intervals
    //   rightEndpoint : if true, mirrors indices for the right end
    // --------------------------------------------------------
    static double pchipEndDerivative(double h0, double h1,
                                     double del0, double del1,
                                     bool rightEndpoint = false)
    {
        // Three-point one-sided estimate (Fritsch & Carlson, 1980)
        double d = ((2.0*h0 + h1)*del0 - h0*del1) / (h0 + h1);

        // Impose sign matching with nearest slope
        if (rightEndpoint)
        {
            if (d * del0 < 0.0) d = 0.0;
            else if (del0 * del1 < 0.0 && std::abs(d) > 3.0*std::abs(del0))
                d = 3.0 * del0;
        }
        else
        {
            if (d * del0 < 0.0) d = 0.0;
            else if (del0 * del1 < 0.0 && std::abs(d) > 3.0*std::abs(del0))
                d = 3.0 * del0;
        }
        return d;
    }
};
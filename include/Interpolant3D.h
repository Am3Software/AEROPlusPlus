#pragma once

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include "Interpolant.h"
#include "Interpolant2D.h"
#include "RegressionMethod.h"

/**
 * @brief 3D Interpolation class using multiple 2D interpolants
 *
 * This class performs 3D interpolation by:
 * 1. For each value of the outer parameter (param2), a 2D interpolant is built
 *    using (x, param1) data.
 * 2. The results from each 2D interpolant at the target (x, param1) are then
 *    interpolated along the param2 dimension using a 1D linear interpolant.
 *
 * Coordinate convention:
 * - x        : abscissa of the base chart (e.g., aspect ratio, distance ratio)
 * - param1   : first curve parameter within each 2D slice (e.g., taper ratio)
 * - param2   : outer parameter that selects among 2D slices (e.g., Mach number)
 *
 * Example use case:
 *   A chart family where:
 *   - x       = aspect ratio
 *   - param1  = taper ratio (curves within each chart)
 *   - param2  = Mach number (selects which chart to use)
 *   - output  = lift slope correction factor K
 */
class Interpolant3D
{
private:
    /**
     * @brief Map from outer parameter (param2) to a 2D interpolant
     *
     * Each entry represents one "slice" of the 3D data at a fixed param2 value.
     * The 2D interpolant within each slice handles (x, param1) interpolation.
     */
    std::map<double, Interpolant2D> sliceMap;

    // Regression settings applied to each internal 2D interpolant
    int degree;
    RegressionMethod method;

    /// Cached sorted list of outer parameter values for quick access
    std::vector<double> param2Values;

public:
    /**
     * @brief Construct a new Interpolant3D object
     *
     * @param degree  Polynomial degree used in the internal 1D/2D interpolants
     * @param method  Regression method (POLYNOMIAL, LINEAR, POWER, LOGARITHMIC, etc.)
     */
    Interpolant3D(int degree = 2, RegressionMethod method = RegressionMethod::POLYNOMIAL)
        : degree(degree), method(method)
    {
    }

    /**
     * @brief Add a single curve to a specific 2D slice
     *
     * If the 2D slice for param2Value does not exist yet, it is created automatically.
     *
     * @param param2Value  Value of the outer parameter for this slice (e.g., Mach = 0.3)
     * @param param1Value  Value of the inner curve parameter within the slice (e.g., taper ratio = 0.5)
     * @param x_data       X values for this curve
     * @param y_data       Y values for this curve
     *
     * @throws std::invalid_argument if x_data and y_data have different sizes or are empty
     *
     * Example:
     * @code
     * Interpolant3D interp3D(2, RegressionMethod::POLYNOMIAL);
     * interp3D.addCurve(0.3, 0.5, x_vec, y_vec);  // Mach=0.3, taper=0.5
     * interp3D.addCurve(0.3, 1.0, x_vec, y_vec);  // Mach=0.3, taper=1.0
     * interp3D.addCurve(0.6, 0.5, x_vec, y_vec);  // Mach=0.6, taper=0.5
     * interp3D.addCurve(0.6, 1.0, x_vec, y_vec);  // Mach=0.6, taper=1.0
     * @endcode
     */
    void addCurve(double param2Value,
                  double param1Value,
                  const std::vector<double> &x_data,
                  const std::vector<double> &y_data)
    {
        if (x_data.size() != y_data.size())
        {
            throw std::invalid_argument("x_data and y_data must have the same size");
        }

        if (x_data.empty())
        {
            throw std::invalid_argument("Data vectors cannot be empty");
        }

        // Create the 2D slice if it does not exist yet
        if (sliceMap.find(param2Value) == sliceMap.end())
        {
            sliceMap.emplace(param2Value, Interpolant2D(degree, method));

            // Update the cached list of outer parameter values
            param2Values.clear();
            for (const auto &entry : sliceMap)
            {
                param2Values.push_back(entry.first);
            }
        }

        // Add the curve to the corresponding 2D slice
        sliceMap.at(param2Value).addCurve(param1Value, x_data, y_data);
    }

    /**
     * @brief Perform 3D interpolation at a target point (x, param1, param2)
     *
     * The interpolation is performed in three steps:
     * 1. For each 2D slice (fixed param2), interpolate at (x_target, param1_target)
     *    using the internal Interpolant2D.
     * 2. Collect the results from all slices.
     * 3. Interpolate the collected results along the param2 dimension using
     *    a 1D linear interpolant evaluated at param2_target.
     *
     * @param x_target       Target value of the base abscissa
     * @param param1_target  Target value of the inner curve parameter
     * @param param2_target  Target value of the outer slice parameter
     * @return double         Interpolated output value
     *
     * @throws std::runtime_error if no data has been added
     *
     * Example:
     * @code
     * double result = interp3D.interpolate(5.0, 0.6, 0.45);
     * // x = 5.0 (aspect ratio), param1 = 0.6 (taper ratio), param2 = 0.45 (Mach)
     * @endcode
     */
    double interpolate(double x_target,
                       double param1_target,
                       double param2_target) const
    {
        if (sliceMap.empty())
        {
            throw std::runtime_error("No data has been added to the 3D interpolator");
        }

        // Special case: only one 2D slice available
        if (sliceMap.size() == 1)
        {
            return sliceMap.begin()->second.interpolate(x_target, param1_target);
        }

        // Step 1 & 2: Evaluate each 2D slice at (x_target, param1_target)
        std::vector<double> y_at_slices;

        for (const auto &[param2_val, slice2D] : sliceMap)
        {
            y_at_slices.push_back(slice2D.interpolate(x_target, param1_target));
        }

        // Step 3: Interpolate along the outer parameter dimension (param2)
        Interpolant interp_param2(param2Values, y_at_slices, 1, RegressionMethod::LINEAR);
        return interp_param2.getYValueFromRegression(param2_target);
    }

    /**
     * @brief Get the total number of 2D slices stored
     *
     * @return size_t Number of outer parameter slices
     */
    size_t getNumSlices() const
    {
        return sliceMap.size();
    }

    /**
     * @brief Get the sorted list of outer parameter values (param2)
     *
     * @return std::vector<double> List of param2 values for which slices exist
     */
    std::vector<double> getParam2Values() const
    {
        return param2Values;
    }

    /**
     * @brief Get the range [min, max] of the outer parameter (param2)
     *
     * @return std::pair<double, double> {min_param2, max_param2}
     * @throws std::runtime_error if no data is available
     */
    std::pair<double, double> getParam2Range() const
    {
        if (param2Values.empty())
        {
            throw std::runtime_error("No data available");
        }
        return {param2Values.front(), param2Values.back()};
    }

    /**
     * @brief Get the number of curves stored within a specific 2D slice
     *
     * @param param2Value  The outer parameter value identifying the slice
     * @return size_t      Number of curves in that slice
     * @throws std::out_of_range if the slice does not exist
     */
    size_t getNumCurvesInSlice(double param2Value) const
    {
        if (sliceMap.find(param2Value) == sliceMap.end())
        {
            throw std::out_of_range("No slice found for the given param2 value");
        }
        return sliceMap.at(param2Value).getNumCurves();
    }
};
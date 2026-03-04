#pragma once

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include "Interpolant.h"
#include "RegressionMethod.h"

/**
 * @brief 2D Interpolation class using multiple 1D interpolants
 * 
 * This class performs 2D interpolation by:
 * 1. Interpolating along one dimension (x) for each parameter value
 * 2. Interpolating the results along the parameter dimension
 * 
 * Example use case: Interpolating upwash gradient as a function of 
 * distance ratio (x) and aspect ratio (parameter)
 */
class Interpolant2D
{
private:
    // Map: parameter value -> pair of (x_data, y_data)
    std::map<double, std::pair<std::vector<double>, std::vector<double>>> dataMap;
    
    // Regression settings
    int degree;
    RegressionMethod method;
    
    // Cached parameter values for quick access
    std::vector<double> parameterValues;

public:
    /**
     * @brief Construct a new Interpolant2D object
     * 
     * @param degree Polynomial degree for interpolation (typically 2-3)
     * @param method Regression method (POLYNOMIAL, LINEAR, etc.)
     */
    Interpolant2D(int degree = 2, RegressionMethod method = RegressionMethod::POLYNOMIAL)
        : degree(degree), method(method)
    {
    }

    /**
     * @brief Add a curve for a specific parameter value
     * 
     * @param parameterValue The parameter value (e.g., aspect ratio = 4.0)
     * @param x_data X values for this curve
     * @param y_data Y values for this curve
     */
    void addCurve(double parameterValue, 
                  const std::vector<double>& x_data, 
                  const std::vector<double>& y_data)
    {
        if (x_data.size() != y_data.size())
        {
            throw std::invalid_argument("x_data and y_data must have the same size");
        }
        
        if (x_data.empty())
        {
            throw std::invalid_argument("Data vectors cannot be empty");
        }

        dataMap[parameterValue] = {x_data, y_data};
        
        // Update parameter values cache
        parameterValues.clear();
        for (const auto& pair : dataMap)
        {
            parameterValues.push_back(pair.first);
        }
    }

    /**
     * @brief Perform 2D interpolation
     * 
     * @param x_target X value ( abscissa of your chart ) to interpolate at 
     * @param parameter_target Parameter ( the paramter which the curve are built - No Y axis value) to interpolate at 
     * @return double Interpolated Y value
     * @note Example: Interpolate a chart with abscissa aspect ratio, y-value a K-factor, with curves at differents taper ratios
     *       - x_target: aspect ratio , parameter_target: your desired taper ratio
     *
     */
    double interpolate(double x_target, double parameter_target) const
    {
        if (dataMap.empty())
        {
            throw std::runtime_error("No data has been added to the interpolator");
        }

        if (dataMap.size() == 1)
        {
            // Only one curve available - just interpolate along x
            const auto& curve = dataMap.begin()->second;
            Interpolant interp1D(curve.first, curve.second, degree, method);
            return interp1D.getYValueFromRegression(x_target);
        }

        // Step 1: For each parameter value, interpolate at x_target
        std::vector<double> y_at_target_x;
        
        for (const auto& [param_val, curve_data] : dataMap)
        {
            const auto& [x_data, y_data] = curve_data;
            Interpolant interp1D(x_data, y_data, degree, method);
            y_at_target_x.push_back(interp1D.getYValueFromRegression(x_target));
        }

        // Step 2: Interpolate along the parameter dimension
        // Use linear interpolation for parameter direction (more stable)
        Interpolant interp_param(parameterValues, y_at_target_x, 1, RegressionMethod::LINEAR);
        return interp_param.getYValueFromRegression(parameter_target);
    }

    /**
     * @brief Get the number of curves stored
     */
    size_t getNumCurves() const
    {
        return dataMap.size();
    }

    /**
     * @brief Get the parameter values for which curves are available
     */
    std::vector<double> getParameterValues() const
    {
        return parameterValues;
    }

    /**
     * @brief Get the range of parameter values
     */
    std::pair<double, double> getParameterRange() const
    {
        if (parameterValues.empty())
        {
            throw std::runtime_error("No data available");
        }
        return {parameterValues.front(), parameterValues.back()};
    }
};
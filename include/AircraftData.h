#ifndef AIRCRAFTDATA_H
#define AIRCRAFTDATA_H

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include "RegressionMethod.h"
#include "Interpolant.h"

namespace AERO {

// Structure representing an interpolable variable
struct InterpolableVariable {
    std::string varName;              // Variable name, e.g.: "VAR 1"
    std::string xLabel;               // X axis label, e.g.: "weightPayload"
    std::string yLabel;               // Y axis label, e.g.: "bodyLengthToFuselageOverallLength"
    std::string xUnit;                // X unit, e.g.: "kg"
    std::string yUnit;                // Y unit, e.g.: "adim", "m", etc.
    RegressionMethod method;          // Regression/interpolation method (POLYNOMIAL, LINEAR, etc.)
    int polynomialDegree;             // Polynomial degree for regression
    
    std::vector<double> xData;        // X data already converted
    std::vector<double> yData;        // Y data already converted
    std::vector<double> coefficients; // Interpolation coefficients
    
    // Checks if the variable is valid (X and Y data are not empty and have the same size)
    bool isValid() const {
        return !xData.empty() && xData.size() == yData.size();
    }
    
    // Returns the number of available data points
    int getNumDataPoints() const {
        return xData.size();
    }
    
    // Method to interpolate a new value given xValue
    double interpolate(double xValue) const {

        // Creates an Interpolant object with the current data and settings - The header is Interpolant.h which already contains everything
        Interpolant interp(xData, yData, polynomialDegree, method);
        return interp.getYValueFromRegression(xValue);
    }
};

// Structure representing the data of an aircraft component
// Each ComponentData object contains:
// - The name of the component (e.g.: "FUSELAGE", "WING", etc.)
// - A 'variables' map that associates the name of a variable (string) to an InterpolableVariable object.
//   This allows dynamically managing multiple variables for each component and accessing them by their name.
//   The map allows adding, searching, and interpolating specific variables for each component.
struct ComponentData {
    std::string componentName;        // Component name, e.g.: "FUSELAGE", "WING", etc.
    // The 'variables' map associates the variable name (string) to an InterpolableVariable object.
    // To build it, insert pairs <variable_name, InterpolableVariable>:
    // Example:
    // variables["VAR 1"] = InterpolableVariable{...};
    // variables["VAR 2"] = InterpolableVariable{...};
    std::map<std::string, InterpolableVariable> variables; // Map of interpolable variables
    
    // Returns a variable given its name (const)
    const InterpolableVariable& getVariable(const std::string& varName) const {
        return variables.at(varName);
    }
    
    // Checks if a variable exists
    // This method checks if the 'variables' map contains a variable named 'varName'.
    // Returns true if the variable exists, false otherwise.
    // Explanation: variables.find(varName) searches for the key in the map.
    // If found, returns an iterator to the found element.
    // If NOT found, returns variables.end(), which is an iterator representing "an element past the last".
    // So, if the result is different from end(), the variable exists; otherwise it does not.
    bool hasVariable(const std::string& varName) const {
        return variables.find(varName) != variables.end();
    }
    
    // Returns the list of available variable names
    std::vector<std::string> getAvailableVariables() const {
        std::vector<std::string> varNames;
        // 'auto' here automatically deduces the type of 'pair' as std::pair<const std::string, InterpolableVariable>
        for (const auto& pair : variables) {
            varNames.push_back(pair.first);
        }
        return varNames;
    }
    
    // Returns the number of available variables
    int getNumVariables() const {
        return variables.size();
    }
    
    // Interpolates a specific variable given its name and the X value
    double interpolate(const std::string& varName, double xValue) const {
        return getVariable(varName).interpolate(xValue);
    }

    // Generates and displays the regression chart for a specific variable
    void getChartOfVariableRegression(const std::string& varName, std::string enableChart, std::string aircraftName) const {
        const InterpolableVariable& var = getVariable(varName);
        Interpolant interp(var.xData, var.yData, var.polynomialDegree, var.method);
        interp.getChartOfRegression(var.xLabel, var.yLabel, var.xUnit, var.yUnit, enableChart,aircraftName);
    }
};

// Structure representing the data of an entire aircraft
struct AircraftData {
    ComponentData fuselage;       // Fuselage data
    ComponentData wing;           // Wing data
    ComponentData horizontal;     // Horizontal tail data
    ComponentData vertical;       // Vertical tail data
    ComponentData undercarriage;  // Undercarriage data
    ComponentData engine;         // Engine data
    
    std::string aircraftName;     // Aircraft name
    
    // Returns a reference to the requested component (non-const)
    // Allows modifying the component data - if needed
    ComponentData& getComponent(const std::string& componentName) {
        if (componentName == "FUSELAGE") return fuselage;
        if (componentName == "WING") return wing;
        if (componentName == "HORIZONTAL_TAIL") return horizontal;
        if (componentName == "VERTICAL_TAIL") return vertical;
        if (componentName == "UNDERCARRIAGE") return undercarriage;
        if (componentName == "ENGINES") return engine;
        throw std::runtime_error("Unknown component: " + componentName);
    }
    
    // Returns a const reference to the requested component
    const ComponentData& getComponent(const std::string& componentName) const {
        if (componentName == "FUSELAGE") return fuselage;
        if (componentName == "WING") return wing;
        if (componentName == "HORIZONTAL_TAIL") return horizontal;
        if (componentName == "VERTICAL_TAIL") return vertical;
        if (componentName == "UNDERCARRIAGE") return undercarriage;
        if (componentName == "ENGINES") return engine;
        throw std::runtime_error("Unknown component: " + componentName);
    }

    void setAircraftName(const std::string& name) {
        aircraftName = name;
    }
};

} // namespace AERO

#endif // AIRCRAFTDATA_H
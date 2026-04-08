#ifndef CONVMASS_H
#define CONVMASS_H

#include "EnumMASS.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
/**
 * @class ConvMass
 * @brief Converts mass values between supported units for scalar and container inputs.
 * @tparam T Input data type (scalar or iterable container of numeric values).
 */
class ConvMass {
   
    private:

    Mass inputMass, outputMass;
    T valueToConvertMass;

    // Converti per container (vector, array, etc.)
    template<typename U = T>
    /**
     * @brief Applies mass conversion to container-like values.
     */
    typename std::enable_if<std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();

        for (auto& value : valueToConvertMass)
        {
            value *= conversionFactor;
        }
    }

    // Converti per tipi scalari (double, float, int, etc.)
    template<typename U = T>
    /**
     * @brief Applies mass conversion to scalar values.
     */
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertMass *= conversionFactor;
    }

    /**
     * @brief Returns the multiplicative conversion factor for selected mass units.
     * @return Conversion factor.
     */
    double getConversionFactor() {
        double conversionFactor = 1.0;

        if (inputMass == Mass::KG && outputMass == Mass::LB) {
            conversionFactor = 2.20462;
        }
        else if (inputMass == Mass::LB && outputMass == Mass::KG) {
            conversionFactor = 1.0 / 2.20462;
        }
        else if (inputMass == Mass::KG && outputMass == Mass::SLUG) {
            conversionFactor = 0.0685218;
        }
        else if (inputMass == Mass::SLUG && outputMass == Mass::KG) {
             conversionFactor = 1.0 / 0.0685218;
        }
        else if (inputMass == Mass::LB && outputMass == Mass::SLUG) {
            conversionFactor = 0.0310809;
        }
        else if (inputMass == Mass::SLUG && outputMass == Mass::LB) {
             conversionFactor = 1.0 / 0.0310809;
        }
        else if (inputMass == outputMass) {
            conversionFactor = 1.0;
        }
        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
        }

        return conversionFactor;
    }

    public:

    /**
     * @brief Constructs the converter and immediately converts the provided value.
     * @param inputMass Source mass unit.
     * @param outputMass Target mass unit.
     * @param valueToConvert Input value (scalar or container).
     */
    ConvMass(Mass inputMass, Mass outputMass, const T& valueToConvert)
        : inputMass(inputMass), outputMass(outputMass), valueToConvertMass(valueToConvert)
    {
        convertedValues();
    }

    /**
     * @brief Returns the converted mass value.
     * @return Constant reference to converted data.
     */
    const T& getConvertedValues() const {
        return valueToConvertMass;
    }
};

/**
 * @brief Class template argument deduction guide for ConvMass.
 */
template<typename T>
ConvMass(Mass, Mass, T) -> ConvMass<T>;

#endif
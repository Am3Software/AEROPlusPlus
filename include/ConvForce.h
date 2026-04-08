#ifndef CONVFORCE_H
#define CONVFORCE_H

#include "EnumFORCE.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
/**
 * @class ConvForce
 * @brief Converts force values between supported units for scalar and container inputs.
 * @tparam T Input data type (scalar or iterable container of numeric values).
 */
class ConvForce {
   
    private:

    Force inputForce, outputForce;
    T valueToConvertForce;

    // Converti per container (vector, array, etc.)
    template<typename U = T>
    /**
     * @brief Applies force conversion to container-like values.
     */
    typename std::enable_if<std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();

        for (auto& value : valueToConvertForce)
        {
            value *= conversionFactor;
        }
    }

    // Converti per tipi scalari (double, float, int, etc.)
    template<typename U = T>
    /**
     * @brief Applies force conversion to scalar values.
     */
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertForce *= conversionFactor;
    }

    /**
     * @brief Returns the multiplicative conversion factor for selected force units.
     * @return Conversion factor.
     */
    double getConversionFactor() {
        double conversionFactor = 1.0;

        if (inputForce == Force::NEWTON && outputForce == Force::POUND_FORCE) {
            conversionFactor = 1.0/4.44822;
        }
        else if (inputForce == Force::POUND_FORCE && outputForce == Force::NEWTON) {
            conversionFactor = 4.44822;
        }
        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
        }

        return conversionFactor;
    }

    public:

    /**
     * @brief Constructs the converter and immediately converts the provided value.
     * @param inputForce Source force unit.
     * @param outputForce Target force unit.
     * @param valueToConvert Input value (scalar or container).
     */
    ConvForce(Force inputForce, Force outputForce, const T& valueToConvert)
        : inputForce(inputForce), outputForce(outputForce), valueToConvertForce(valueToConvert)
    {
        convertedValues();
    }

    /**
     * @brief Returns the converted force value.
     * @return Constant reference to converted data.
     */
    const T& getConvertedValues() const {
        return valueToConvertForce;
    }
};

/**
 * @brief Class template argument deduction guide for ConvForce.
 */
template<typename T>
ConvForce(Force, Force, T) -> ConvForce<T>;

#endif
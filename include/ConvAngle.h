#ifndef CONVANGLE_H
#define CONVANGLE_H

#include "EnumANGLE.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
/**
 * @class ConvAngle
 * @brief Converts angle values between supported units for scalar and container inputs.
 * @tparam T Input data type (scalar or iterable container of numeric values).
 */
class ConvAngle {
   
    private:

    Angle inputAngle, outputAngle;
    T valueToConvertAngle;

    // Per container
    template<typename U = T>
    /**
     * @brief Applies angle conversion to container-like values.
     */
    typename std::enable_if<std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        for (auto& value : valueToConvertAngle)
        {
            value *= conversionFactor;
        }
    }

    // Per scalari
    template<typename U = T>
    /**
     * @brief Applies angle conversion to scalar values.
     */
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertAngle *= conversionFactor;
    }

    /**
     * @brief Returns the multiplicative conversion factor for selected angle units.
     * @return Conversion factor.
     */
    double getConversionFactor() {
        double conversionFactor = 1.0;

        if (inputAngle == Angle::DEG && outputAngle == Angle::RAD) {
            conversionFactor = 1.0/57.3;
        }
        else if (inputAngle == Angle::RAD && outputAngle == Angle::DEG) {
            conversionFactor = 57.3;
        }
        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
        }

        return conversionFactor;
    }

    public:

    /**
     * @brief Constructs the converter and immediately converts the provided value.
     * @param inputAngle Source angle unit.
     * @param outputAngle Target angle unit.
     * @param valueToConvert Input value (scalar or container).
     */
    ConvAngle(Angle inputAngle, Angle outputAngle, const T& valueToConvert)
        : inputAngle(inputAngle), outputAngle(outputAngle), valueToConvertAngle(valueToConvert)
    {
        convertedValues();
    }

    /**
     * @brief Returns the converted angle value.
     * @return Constant reference to converted data.
     */
    const T& getConvertedValues() const {
        return valueToConvertAngle;
    }
};

/**
 * @brief Class template argument deduction guide for ConvAngle.
 */
template<typename T>
ConvAngle(Angle, Angle, T) -> ConvAngle<T>;

#endif
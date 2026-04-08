#ifndef CONVAREA_H
#define CONVAREA_H

#include "EnumAREA.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <type_traits>

template <typename T = std::vector<double>>
/**
 * @class ConvArea
 * @brief Converts area values between supported units for scalar and container inputs.
 * @tparam T Input data type (scalar or iterable container of numeric values).
 */
class ConvArea {
   
    private:

    Area inputArea, outputArea;
    T valueToConvertArea;

    // Per container
    template<typename U = T>
    /**
     * @brief Applies area conversion to container-like values.
     */
    typename std::enable_if<std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        for (auto& value : valueToConvertArea)
        {
            value *= conversionFactor;
        }
    }

    // Per scalari
    template<typename U = T>
    /**
     * @brief Applies area conversion to scalar values.
     */
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertArea *= conversionFactor;
    }

    /**
     * @brief Returns the multiplicative conversion factor for selected area units.
     * @return Conversion factor.
     */
    double getConversionFactor() {
        double conversionFactor = 1.0;

        if (inputArea == Area::SQUARE_METER && outputArea == Area::SQUARE_FEET) {
            conversionFactor = 1.0/std::pow(0.3048, 2);
        }
        else if (inputArea == Area::SQUARE_FEET && outputArea == Area::SQUARE_METER) {
            conversionFactor = std::pow(0.3048, 2);
        }
        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
        }

        return conversionFactor;
    }

    public:

    /**
     * @brief Constructs the converter and immediately converts the provided value.
     * @param inputArea Source area unit.
     * @param outputArea Target area unit.
     * @param valueToConvert Input value (scalar or container).
     */
    ConvArea(Area inputArea, Area outputArea, const T& valueToConvert)
        : inputArea(inputArea), outputArea(outputArea), valueToConvertArea(valueToConvert)
    {
        convertedValues();
    }

    /**
     * @brief Returns the converted area value.
     * @return Constant reference to converted data.
     */
    const T& getConvertedValues() const {
        return valueToConvertArea;
    }
};

/**
 * @brief Class template argument deduction guide for ConvArea.
 */
template<typename T>
ConvArea(Area, Area, T) -> ConvArea<T>;

#endif
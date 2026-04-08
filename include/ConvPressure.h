#pragma once


#include "EnumPressure.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
/**
 * @class ConvPressure
 * @brief Converts pressure values between supported units for scalars and containers.
 * @tparam T Input data type (scalar or iterable container of numeric values).
 */
class ConvPressure {
   
    private:

    Pressure inputPressure, outputPressure;
    T valueToConvertPressure;

    // Per container
    template<typename U = T>
    /**
     * @brief Applies pressure conversion to container-like values.
     */
    typename std::enable_if<std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        for (auto& value : valueToConvertPressure)
        {
            value *= conversionFactor;
        }
    }

    // Per scalari
    template<typename U = T>
    /**
     * @brief Applies pressure conversion to scalar values.
     */
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertPressure *= conversionFactor;
    }

    /**
     * @brief Returns the multiplicative conversion factor for selected pressure units.
     * @return Conversion factor.
     */
    double getConversionFactor() {
        double conversionFactor = 1.0;

        if (inputPressure == Pressure::PASCALS && outputPressure == Pressure::PSF) {
            conversionFactor = 0.0208854342; // 1 Pascal = 0.0208854342 PSF
        }
        else if (inputPressure == Pressure::PSF && outputPressure == Pressure::PASCALS) {
            conversionFactor = 47.8802589; // 1 PSF = 47.8802589 Pascal
        }
        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
        }

        return conversionFactor;
    }

    public:

    /**
     * @brief Constructs the converter and immediately converts the provided value.
     * @param inputPressure Source pressure unit.
     * @param outputPressure Target pressure unit.
     * @param valueToConvert Input value (scalar or container).
     */
    ConvPressure(Pressure inputPressure, Pressure outputPressure, const T& valueToConvert)
        : inputPressure(inputPressure), outputPressure(outputPressure), valueToConvertPressure(valueToConvert)
    {
        convertedValues();
    }

    /**
     * @brief Returns the converted pressure value.
     * @return Constant reference to converted data.
     */
    const T& getConvertedValues() const {
        return valueToConvertPressure;
    }
};

/**
 * @brief Class template argument deduction guide for ConvPressure.
 */
template<typename T>
ConvPressure(Pressure, Pressure, T) -> ConvPressure<T>;


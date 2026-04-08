#ifndef CONVLENGTH_H
#define CONVLENGTH_H

#include "EnumLENGTH.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
/**
 * @class ConvLength
 * @brief Converts length values between supported units for scalar and container inputs.
 * @tparam T Input data type (scalar or iterable container of numeric values).
 */
class ConvLength {
   
    private:

    Length inputLength, outputLength;
    T valueToConvertLength;

    // Per container
    template<typename U = T>
    /**
     * @brief Applies length conversion to container-like values.
     */
    typename std::enable_if<std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        for (auto& value : valueToConvertLength)
        {
            value *= conversionFactor;
        }
    }

    // Per scalari
    template<typename U = T>
    /**
     * @brief Applies length conversion to scalar values.
     */
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertLength *= conversionFactor;
    }

    /**
     * @brief Returns the multiplicative conversion factor for selected length units.
     * @return Conversion factor.
     */
    double getConversionFactor() {
        double conversionFactor = 1.0;

        if (inputLength == Length::M && outputLength == Length::FT) {
            conversionFactor = 1.0/0.3048;
        }
        else if (inputLength == Length::FT && outputLength == Length::M) {
            conversionFactor = 0.3048;
        }
        else if (inputLength == Length::M && outputLength == Length::NMI) {
            conversionFactor = 1.0/1852.0;
        }
        else if (inputLength == Length::NMI && outputLength == Length::M) {
            conversionFactor = 1852.0;
        }
        else if (inputLength == Length::FT && outputLength == Length::NMI) {
            conversionFactor = 1.0/6076.12;
        }
        else if (inputLength == Length::NMI && outputLength == Length::FT) {
            conversionFactor = 6076.12;
        }
        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
        }

        return conversionFactor;
    }

    public:

    /**
     * @brief Constructs the converter and immediately converts the provided value.
     * @param inputLength Source length unit.
     * @param outputLength Target length unit.
     * @param valueToConvert Input value (scalar or container).
     */
    ConvLength(Length inputLength, Length outputLength, const T& valueToConvert)
        : inputLength(inputLength), outputLength(outputLength), valueToConvertLength(valueToConvert)
    {
        convertedValues();
    }

    /**
     * @brief Returns the converted length value.
     * @return Constant reference to converted data.
     */
    const T& getConvertedValues() const {
        return valueToConvertLength;
    }
};

/**
 * @brief Class template argument deduction guide for ConvLength.
 */
template<typename T>
ConvLength(Length, Length, T) -> ConvLength<T>;

#endif
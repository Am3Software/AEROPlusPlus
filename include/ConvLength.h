#ifndef CONVLENGTH_H
#define CONVLENGTH_H

#include "EnumLENGTH.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
class ConvLength {
   
    private:

    Length inputLength, outputLength;
    T valueToConvertLength;

    // Per container
    template<typename U = T>
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
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertLength *= conversionFactor;
    }

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

    ConvLength(Length inputLength, Length outputLength, const T& valueToConvert)
        : inputLength(inputLength), outputLength(outputLength), valueToConvertLength(valueToConvert)
    {
        convertedValues();
    }

    const T& getConvertedValues() const {
        return valueToConvertLength;
    }
};

template<typename T>
ConvLength(Length, Length, T) -> ConvLength<T>;

#endif
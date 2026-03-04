#pragma once


#include "EnumPressure.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
class ConvPressure {
   
    private:

    Pressure inputPressure, outputPressure;
    T valueToConvertPressure;

    // Per container
    template<typename U = T>
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
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertPressure *= conversionFactor;
    }

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

    ConvPressure(Pressure inputPressure, Pressure outputPressure, const T& valueToConvert)
        : inputPressure(inputPressure), outputPressure(outputPressure), valueToConvertPressure(valueToConvert)
    {
        convertedValues();
    }

    const T& getConvertedValues() const {
        return valueToConvertPressure;
    }
};

template<typename T>
ConvPressure(Pressure, Pressure, T) -> ConvPressure<T>;


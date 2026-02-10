#ifndef CONVDENSITY_H
#define CONVDENSITY_H

#include "EnumDENSITY.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
class ConvDensity {
   
    private:

    Density inputDensity, outputDensity;
    T valueToConvertDensity;

    // Per container
    template<typename U = T>
    typename std::enable_if<std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        for (auto& value : valueToConvertDensity)
        {
            value *= conversionFactor;
        }
    }

    // Per scalari
    template<typename U = T>
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertDensity *= conversionFactor;
    }

    double getConversionFactor() {
        double conversionFactor = 1.0;

        if (inputDensity == Density::KG_TO_M3 && outputDensity == Density::SLUGS_TO_FT3) {
            conversionFactor = 1.0/515.3788;
        }
        else if (inputDensity == Density::SLUGS_TO_FT3 && outputDensity == Density::KG_TO_M3) {
            conversionFactor = 515.3788;
        }
        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
        }

        return conversionFactor;
    }

    public:

    ConvDensity(Density inputDensity, Density outputDensity, const T& valueToConvert)
        : inputDensity(inputDensity), outputDensity(outputDensity), valueToConvertDensity(valueToConvert)
    {
        convertedValues();
    }

    const T& getConvertedValues() const {
        return valueToConvertDensity;
    }
};

template<typename T>
ConvDensity(Density, Density, T) -> ConvDensity<T>;

#endif
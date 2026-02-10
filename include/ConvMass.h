#ifndef CONVMASS_H
#define CONVMASS_H

#include "EnumMASS.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
class ConvMass {
   
    private:

    Mass inputMass, outputMass;
    T valueToConvertMass;

    // Converti per container (vector, array, etc.)
    template<typename U = T>
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
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertMass *= conversionFactor;
    }

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

    ConvMass(Mass inputMass, Mass outputMass, const T& valueToConvert)
        : inputMass(inputMass), outputMass(outputMass), valueToConvertMass(valueToConvert)
    {
        convertedValues();
    }

    const T& getConvertedValues() const {
        return valueToConvertMass;
    }
};

// Guide di deduzione
template<typename T>
ConvMass(Mass, Mass, T) -> ConvMass<T>;

#endif
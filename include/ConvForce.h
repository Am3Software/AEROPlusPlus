#ifndef CONVFORCE_H
#define CONVFORCE_H

#include "EnumFORCE.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
class ConvForce {
   
    private:

    Force inputForce, outputForce;
    T valueToConvertForce;

    // Converti per container (vector, array, etc.)
    template<typename U = T>
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
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertForce *= conversionFactor;
    }

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

    ConvForce(Force inputForce, Force outputForce, const T& valueToConvert)
        : inputForce(inputForce), outputForce(outputForce), valueToConvertForce(valueToConvert)
    {
        convertedValues();
    }

    const T& getConvertedValues() const {
        return valueToConvertForce;
    }
};

// Guide di deduzione
template<typename T>
ConvForce(Force, Force, T) -> ConvForce<T>;

#endif
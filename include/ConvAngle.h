#ifndef CONVANGLE_H
#define CONVANGLE_H

#include "EnumANGLE.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
class ConvAngle {
   
    private:

    Angle inputAngle, outputAngle;
    T valueToConvertAngle;

    // Per container
    template<typename U = T>
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
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertAngle *= conversionFactor;
    }

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

    ConvAngle(Angle inputAngle, Angle outputAngle, const T& valueToConvert)
        : inputAngle(inputAngle), outputAngle(outputAngle), valueToConvertAngle(valueToConvert)
    {
        convertedValues();
    }

    const T& getConvertedValues() const {
        return valueToConvertAngle;
    }
};

template<typename T>
ConvAngle(Angle, Angle, T) -> ConvAngle<T>;

#endif
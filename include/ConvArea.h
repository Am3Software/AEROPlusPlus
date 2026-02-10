#ifndef CONVAREA_H
#define CONVAREA_H

#include "EnumAREA.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <type_traits>

template <typename T = std::vector<double>>
class ConvArea {
   
    private:

    Area inputArea, outputArea;
    T valueToConvertArea;

    // Per container
    template<typename U = T>
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
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues() {
        double conversionFactor = getConversionFactor();
        valueToConvertArea *= conversionFactor;
    }

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

    ConvArea(Area inputArea, Area outputArea, const T& valueToConvert)
        : inputArea(inputArea), outputArea(outputArea), valueToConvertArea(valueToConvert)
    {
        convertedValues();
    }

    const T& getConvertedValues() const {
        return valueToConvertArea;
    }
};

template<typename T>
ConvArea(Area, Area, T) -> ConvArea<T>;

#endif
#ifndef CONVAREA_H
#define CONVAREA_H

#include "EnumAREA.h"
#include <vector>
#include <iostream>
#include <cmath>

class ConvArea {
   
    private:

    Area inputArea, outputArea;
    std::vector<double> valueToConvertArea;

    std::vector<double> convertedValues() {
    double conversionFactor = 1.0;

        // Determina il fattore di conversione
        if (inputArea == Area::SQUARE_METER && outputArea == Area::SQUARE_FEET) {
            
            conversionFactor = 1.0/std::pow(0.3048, 2);
        }
        else if (inputArea == Area::SQUARE_FEET && outputArea == Area::SQUARE_METER) {

            conversionFactor = std::pow(0.3048, 2);
        }

        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
            return std::vector<double>();
        }

        // Applica la conversione IN-PLACE senza emplace_back
        for (size_t i = 0; i < valueToConvertArea.size(); i++)
        {
            valueToConvertArea[i] = valueToConvertArea[i] * conversionFactor;
        }

        return valueToConvertArea;
    }

    public:

    ConvArea(Area inputArea, Area outputArea, std::vector<double> valueToConvert) {


        this->inputArea = inputArea;
        this->outputArea = outputArea;
        this->valueToConvertArea = valueToConvert;

       convertedValues();


        
    }


   const std::vector<double>& getConvertedValues() const{

        return valueToConvertArea;
    }



    
};

#endif
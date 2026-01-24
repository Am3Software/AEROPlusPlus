#ifndef CONVANGLE_H
#define CONVANGLE_H

#include "EnumANGLE.h"
#include <vector>
#include <iostream>

class ConvAngle {
   
    private:

    Angle inputAngle, outputAngle;
    std::vector<double> valueToConvertAngle;

    std::vector<double> convertedValues() {
    double conversionFactor = 1.0;

        // Determina il fattore di conversione
        if (inputAngle == Angle::DEG && outputAngle == Angle::RAD) {
            
            conversionFactor = 1.0/57.3;
        }
        else if (inputAngle == Angle::RAD && outputAngle == Angle::DEG) {

            conversionFactor = 57.3;
        }

        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
            return std::vector<double>();
        }

        // Applica la conversione IN-PLACE senza emplace_back
        for (size_t i = 0; i < valueToConvertAngle.size(); i++)
        {
            valueToConvertAngle[i] = valueToConvertAngle[i] * conversionFactor;
        }

        return valueToConvertAngle;
    }

    public:

    ConvAngle(Angle inputAngle, Angle outputAngle, std::vector<double> valueToConvert) {


        this->inputAngle = inputAngle;
        this->outputAngle = outputAngle;
        this->valueToConvertAngle = valueToConvert;

       convertedValues();


        
    }


   const std::vector<double>& getConvertedValues() const{

        return valueToConvertAngle;
    }



    
};

#endif
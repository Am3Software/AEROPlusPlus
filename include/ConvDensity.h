#ifndef CONVDENSITY_H
#define CONVDENSITY_H

#include "EnumDENSITY.h"
#include <vector>
#include <iostream>

class ConvDensity {
   
    private:

    Density inputDensity, outputDensity;
    std::vector<double> valueToConvertDensity;

    std::vector<double> convertedValues() {
    double conversionFactor = 1.0;

        // Determina il fattore di conversione
        if (inputDensity == Density::KG_TO_M3 && outputDensity == Density::SLUGS_TO_FT3) {
            
            conversionFactor = 1.0/515.3788;
        }
        else if (inputDensity == Density::SLUGS_TO_FT3 && outputDensity == Density::KG_TO_M3) {

            conversionFactor = 515.3788;
        }

        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
            return std::vector<double>();
        }

        // Applica la conversione IN-PLACE senza emplace_back
        for (size_t i = 0; i < valueToConvertDensity.size(); i++)
        {
            valueToConvertDensity[i] = valueToConvertDensity[i] * conversionFactor;
        }

        return valueToConvertDensity;
    }

    public:

    ConvDensity(Density inputDensity, Density outputDensity, std::vector<double> valueToConvert) {


        this->inputDensity = inputDensity;
        this->outputDensity = outputDensity;
        this->valueToConvertDensity = valueToConvert;

       convertedValues();


        
    }


   const std::vector<double>& getConvertedValues() const{

        return valueToConvertDensity;
    }



    
};

#endif
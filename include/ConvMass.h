#ifndef CONVMASS_H
#define CONVMASS_H

#include "EnumMASS.h"
#include <vector>
#include <iostream>

class ConvMass {
   
    private:

    Mass inputMass, outputMass;
    std::vector<double> valueToConvertMass;

    std::vector<double> convertedValues() {
    double conversionFactor = 1.0;

        // Determina il fattore di conversione
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
            return std::vector<double>();
        }

        // Applica la conversione IN-PLACE senza emplace_back
        for (size_t i = 0; i < valueToConvertMass.size(); i++)
        {
            valueToConvertMass[i] = valueToConvertMass[i] * conversionFactor;
        }

        return valueToConvertMass;
    }

    public:

    ConvMass(Mass inputMass, Mass outputMass, std::vector<double> valueToConvert) {


        this->inputMass = inputMass;
        this->outputMass = outputMass;
        this->valueToConvertMass = valueToConvert;

       convertedValues();


        
    }


   const std::vector<double>& getConvertedValues() const{


        return valueToConvertMass;
    }



    
};

#endif
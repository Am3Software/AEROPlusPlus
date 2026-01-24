#ifndef CONVFORCE_H
#define CONVFORCE_H

#include "EnumFORCE.h"
#include <vector>
#include <iostream>

class ConvForce {
   
    private:

    Force inputForce, outputForce;
    std::vector<double> valueToConvertForce;

    std::vector<double> convertedValues() {
    double conversionFactor = 1.0;

        // Determina il fattore di conversione
        if (inputForce == Force::NEWTON && outputForce == Force::POUND_FORCE) {

            conversionFactor = 1.0/4.44822;
        }
        else if (inputForce == Force::POUND_FORCE && outputForce == Force::NEWTON) {

            conversionFactor = 4.44822;
        }

        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
            return std::vector<double>();
        }

        // Applica la conversione IN-PLACE senza emplace_back
        for (size_t i = 0; i < valueToConvertForce.size(); i++)
        {
            valueToConvertForce[i] = valueToConvertForce[i] * conversionFactor;
        }

        return valueToConvertForce;
    }

    public:

    ConvForce(Force inputForce, Force outputForce, std::vector<double> valueToConvert) {


        this->inputForce = inputForce;
        this->outputForce = outputForce;
        this->valueToConvertForce = valueToConvert;

       convertedValues();


        
    }


   const std::vector<double>& getConvertedValues() const{

        return valueToConvertForce;
    }



    
};

#endif
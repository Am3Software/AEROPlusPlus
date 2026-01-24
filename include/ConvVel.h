#ifndef CONVVEL_H
#define CONVVEL_H

#include "EnumsPEED.h"
#include <vector>
#include <iostream>

class ConvVel {
   
    private:

    Speed inputSpeed, outputSpeed;
    std::vector<double> valueToConvertSpeed;

    std::vector<double> convertedValues() {
    double conversionFactor = 1.0;

        // Determina il fattore di conversione
        if (inputSpeed == Speed::M_TO_S && outputSpeed == Speed::FT_TO_S) {
            conversionFactor = 1.0/0.3048;
        }
        else if (inputSpeed == Speed::M_TO_S && outputSpeed == Speed::FT_TO_MIN) {

            conversionFactor = 1.0/0.00508;
        }

        else if (inputSpeed == Speed::M_TO_S && outputSpeed == Speed::KTS) {

            conversionFactor = 1.0/0.5144444;
        }

        else if (outputSpeed == Speed::FT_TO_S && outputSpeed == Speed::M_TO_S) {

            conversionFactor = 0.3048;
            
        }

        else if (inputSpeed == Speed::FT_TO_S && outputSpeed == Speed::FT_TO_MIN) {

            conversionFactor = 1.0/60.0;
        }
        else if (inputSpeed == Speed::FT_TO_S && outputSpeed == Speed::KTS) {

            conversionFactor = 1.0/1.688;
            
        }

        else if (inputSpeed == Speed::FT_TO_MIN && outputSpeed == Speed::M_TO_S) {

            conversionFactor = 0.00508;
            
        }
        else if (inputSpeed == Speed::FT_TO_MIN && outputSpeed == Speed::FT_TO_S) {

            conversionFactor = 60.0;
            
        }
        else if (inputSpeed == Speed::FT_TO_MIN && outputSpeed == Speed::KTS) {

            conversionFactor = 1.0/101.27;
            
        }

        else if (inputSpeed == Speed::KTS && outputSpeed == Speed::M_TO_S) {

            conversionFactor = 0.5144444;
            
        }
        else if (inputSpeed == Speed::KTS && outputSpeed == Speed::FT_TO_S) {

            conversionFactor = 1.688;
            
        }
        else if (inputSpeed == Speed::KTS && outputSpeed == Speed::FT_TO_MIN) {

            conversionFactor = 101.27;
            
        }
        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
            return std::vector<double>();
        }

        // Applica la conversione IN-PLACE senza emplace_back
        for (size_t i = 0; i < valueToConvertSpeed.size(); i++)
        {
            valueToConvertSpeed[i] = valueToConvertSpeed[i] * conversionFactor;
        }

        return valueToConvertSpeed;
    }

    public:

    ConvVel(Speed inputSpeed, Speed outputSpeed, std::vector<double> valueToConvert) {


        this->inputSpeed = inputSpeed;
        this->outputSpeed = outputSpeed;
        this->valueToConvertSpeed = valueToConvert;

       convertedValues();


        
    }


   const std::vector<double>& getConvertedValues() const{

        return valueToConvertSpeed;
    }



    
};

#endif
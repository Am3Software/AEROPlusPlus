#ifndef CONVLENGTH_H
#define CONVLENGTH_H

#include "EnumLENGTH.h"
#include <vector>
#include <iostream>

class ConvLength {
   
    private:

    Length inputLength, outputLength;
    std::vector<double> valueToConvertLength;

    std::vector<double> convertedValues() {
    double conversionFactor = 1.0;

        // Determina il fattore di conversione
        if (inputLength == Length::M && outputLength == Length::FT) {
            conversionFactor = 1.0/0.3048;
        }
        else if (inputLength == Length::FT && outputLength == Length::M) {
            conversionFactor = 0.3048;
        }

        else if (inputLength == Length::M && outputLength == Length::NMI) {

            conversionFactor = 1.0/1852.0;
        }
        else if (inputLength == Length::NMI && outputLength == Length::M) {

            conversionFactor = 1852.0;
            
        }

        else if (inputLength == Length::FT && outputLength == Length::NMI) {

            conversionFactor = 1.0/6076.12;
        }
        else if (inputLength == Length::NMI && outputLength == Length::FT) {

            conversionFactor = 6076.12;
            
        }
        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
            return std::vector<double>();
        }

        // Applica la conversione IN-PLACE senza emplace_back
        for (size_t i = 0; i < valueToConvertLength.size(); i++)
        {
            valueToConvertLength[i] = valueToConvertLength[i] * conversionFactor;
        }

        return valueToConvertLength;
    }

    public:

    ConvLength(Length inputLength, Length outputLength, std::vector<double> valueToConvert) {


        this->inputLength = inputLength;
        this->outputLength = outputLength;
        this->valueToConvertLength = valueToConvert;

       convertedValues();


        
    }


   const std::vector<double>& getConvertedValues() const{

        return valueToConvertLength;
    }



    
};

#endif
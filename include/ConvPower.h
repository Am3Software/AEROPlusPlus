#ifndef CONVPOWER_H
#define CONVPOWER_H

#include "EnumPOWER.h"
#include <vector>
#include <iostream>

template <typename Container = std::vector<double>>
class ConvPower {
   
    private:

    Power inputPower, outputPower;
    Container valueToConvertPower;

    Container convertedValues() {
    double conversionFactor = 1.0;

        // Determina il fattore di conversione
        if (inputPower == Power::HP && outputPower == Power::WATT) {
            
            conversionFactor = 745.7;
        }
        else if (inputPower == Power::WATT && outputPower == Power::HP) {

            conversionFactor = 1.0/745.7;
        }

        else {
            std::cerr << "Conversion not implemented yet!" << std::endl;
            return std::vector<double>();
        }

        // Applica la conversione IN-PLACE senza emplace_back
        for (size_t i = 0; i < valueToConvertPower.size(); i++)
        {
            valueToConvertPower[i] = valueToConvertPower[i] * conversionFactor;
        }

        return valueToConvertPower;
    }

    public:

    ConvPower(Power inputPower, Power outputPower, Container valueToConvert) {


        this->inputPower = inputPower;
        this->outputPower = outputPower;
        this->valueToConvertPower = valueToConvert;

       convertedValues();


        
    }


   const Container& getConvertedValues() const{

        return valueToConvertPower;
    }

    
};

// Guide di deduzione
template<typename Container>
ConvPower(Power, Power, Container) -> ConvPower<Container>;


#endif
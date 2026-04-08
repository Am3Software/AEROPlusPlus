#ifndef CONVPOWER_H
#define CONVPOWER_H

#include "EnumPOWER.h"
#include <vector>
#include <iostream>

template <typename Container = std::vector<double>>
/**
 * @class ConvPower
 * @brief Converts power values between supported units for scalar and container inputs.
 * @tparam Container Input data type (scalar or iterable container of numeric values).
 */
class ConvPower
{

private:
    Power inputPower, outputPower;
    Container valueToConvertPower;

    /**
     * @brief Applies power conversion to the stored input value.
     * @return Converted value container/scalar.
     */
    Container convertedValues()
    {
        double conversionFactor = 1.0;

        // Determina il fattore di conversione
        if (inputPower == Power::HP && outputPower == Power::WATT)
        {

            conversionFactor = 745.7;
        }
        else if (inputPower == Power::WATT && outputPower == Power::HP)
        {

            conversionFactor = 1.0 / 745.7;
        }

        else
        {
            std::cerr << "Conversion not implemented yet!" << std::endl;
            return Container{};
        }

        // Applica conversione generica
        if constexpr (std::is_arithmetic_v<Container>)
        {
            // Caso scalare: double, float, int...
            valueToConvertPower *= conversionFactor;
        }
        else
        {
            // Caso vector
            for (auto &val : valueToConvertPower)
                val *= conversionFactor;
        }

        return valueToConvertPower;
    }

public:
    /**
     * @brief Constructs the converter and immediately converts the provided value.
     * @param inputPower Source power unit.
     * @param outputPower Target power unit.
     * @param valueToConvert Input value (scalar or container).
     */
    ConvPower(Power inputPower, Power outputPower, Container valueToConvert)
    {

        this->inputPower = inputPower;
        this->outputPower = outputPower;
        this->valueToConvertPower = valueToConvert;

        convertedValues();
    }

    /**
     * @brief Returns the converted power value.
     * @return Constant reference to converted data.
     */
    const Container &getConvertedValues() const
    {

        return valueToConvertPower;
    }
};

/**
 * @brief Class template argument deduction guide for ConvPower.
 */
template <typename Container>
ConvPower(Power, Power, Container) -> ConvPower<Container>;

#endif
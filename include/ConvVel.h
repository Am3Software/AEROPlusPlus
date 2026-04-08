#ifndef CONVVEL_H
#define CONVVEL_H

#include "EnumSPEED.h"
#include <vector>
#include <iostream>
#include <type_traits>

template <typename T = std::vector<double>>
/**
 * @class ConvVel
 * @brief Converts speed values between units for scalar values or containers.
 * @tparam T Input data type (scalar or iterable container of numeric values).
 */
class ConvVel
{

private:
    Speed inputSpeed, outputSpeed;
    T valueToConvertSpeed;

    // Per container
    template <typename U = T>
    /**
     * @brief Applies conversion to container-like values.
     */
    typename std::enable_if<std::is_class<U>::value>::type
    convertedValues()
    {
        double conversionFactor = getConversionFactor();
        for (auto &value : valueToConvertSpeed)
        {
            value *= conversionFactor;
        }
    }

    // Per scalari
    template <typename U = T>
    /**
     * @brief Applies conversion to scalar values.
     */
    typename std::enable_if<!std::is_class<U>::value>::type
    convertedValues()
    {
        double conversionFactor = getConversionFactor();
        valueToConvertSpeed *= conversionFactor;
    }

    /**
     * @brief Returns the multiplicative factor for the selected speed-unit conversion.
     * @return Conversion factor.
     */
    double getConversionFactor()
    {
        double conversionFactor = 1.0;

        if (inputSpeed == Speed::M_TO_S && outputSpeed == Speed::FT_TO_S)
        {
            conversionFactor = 1.0 / 0.3048;
        }
        else if (inputSpeed == Speed::M_TO_S && outputSpeed == Speed::FT_TO_MIN)
        {
            conversionFactor = 1.0 / 0.00508;
        }
        else if (inputSpeed == Speed::M_TO_S && outputSpeed == Speed::KTS)
        {
            conversionFactor = 1.0 / 0.5144444;
        }
        else if (inputSpeed == Speed::M_TO_S && outputSpeed == Speed::KM_TO_H)
        {
            conversionFactor = 3.6;
        }
        else if (inputSpeed == Speed::FT_TO_S && outputSpeed == Speed::M_TO_S)
        {
            conversionFactor = 0.3048;
        }
        else if (inputSpeed == Speed::FT_TO_S && outputSpeed == Speed::FT_TO_MIN)
        {
            conversionFactor = 1.0 / 60.0;
        }
        else if (inputSpeed == Speed::FT_TO_S && outputSpeed == Speed::KTS)
        {
            conversionFactor = 1.0 / 1.688;
        }

        else if (inputSpeed == Speed::FT_TO_S && outputSpeed == Speed::KM_TO_H)
        {
            conversionFactor = 0.3048 * 3.6;
        }
        else if (inputSpeed == Speed::FT_TO_MIN && outputSpeed == Speed::M_TO_S)
        {
            conversionFactor = 0.00508;
        }
        else if (inputSpeed == Speed::FT_TO_MIN && outputSpeed == Speed::FT_TO_S)
        {
            conversionFactor = 60.0;
        }
        else if (inputSpeed == Speed::FT_TO_MIN && outputSpeed == Speed::KTS)
        {
            conversionFactor = 1.0 / 101.27;
        }

        else if (inputSpeed == Speed::FT_TO_MIN && outputSpeed == Speed::KM_TO_H)
        {
            conversionFactor = 60 * (0.3048 / 1000.0);
        }
        else if (inputSpeed == Speed::KTS && outputSpeed == Speed::M_TO_S)
        {
            conversionFactor = 0.5144444;
        }
        else if (inputSpeed == Speed::KTS && outputSpeed == Speed::FT_TO_S)
        {
            conversionFactor = 1.688;
        }
        else if (inputSpeed == Speed::KTS && outputSpeed == Speed::FT_TO_MIN)
        {
            conversionFactor = 101.27;
        }

        else if (inputSpeed == Speed::KTS && outputSpeed == Speed::KM_TO_H)
        {
            conversionFactor = 1.852;
        }

        else
        {
            std::cerr << "Conversion not implemented yet!" << std::endl;
        }

        return conversionFactor;
    }

public:
    /**
     * @brief Constructs the converter and immediately converts the provided value.
     * @param inputSpeed Source speed unit.
     * @param outputSpeed Target speed unit.
     * @param valueToConvert Input value (scalar or container).
     */
    ConvVel(Speed inputSpeed, Speed outputSpeed, const T &valueToConvert)
        : inputSpeed(inputSpeed), outputSpeed(outputSpeed), valueToConvertSpeed(valueToConvert)
    {
        convertedValues();
    }

    /**
     * @brief Returns the converted value.
     * @return Constant reference to converted data.
     */
    const T &getConvertedValues() const
    {
        return valueToConvertSpeed;
    }
};

/**
 * @brief Class template argument deduction guide for ConvVel.
 */
template <typename T>
ConvVel(Speed, Speed, T) -> ConvVel<T>;

#endif
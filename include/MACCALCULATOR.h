#pragma once

#include <string>
#include <vector>
#include <cmath>
#include "EnumTypeOfWing.h"

class MACCalculator
{

private:
    double rootChord;
    double taperRatio;
    double span;
    double kinkStation;
    double taperInBoard;
    double MAC = 0.0;
    double yMAC = 0.0;
    double numeratorCranked = 0.0;
    double denominatorCranked = 0.0;
    double numeratorYMACCranked = 0.0;
    double denominatorYMACCranked = 0.0;
    double factorToYMACCranked = (1.0 / 3.0);

public:
    MACCalculator()
    {
    }

    /// @brief Calculate Mean Aerodynamic Chord (MAC) length (in meters)
    /// @param typeOfWing Type of wing (e.g., STRAIGHT_TAPERED, CRANKED)
    /// @param rootChord The root chord length
    /// @param taperRatio The taper ratio of the wing
    /// @param span The wingspan (in meters)
    /// @param kinkStation The y-station of the wing kink (for CRANKED wings - in meters)
    /// @param taperInBoard The taper ratio inboard of the kink (for CRANKED wings)
    /// @return The Mean Aerodynamic Chord (MAC) length (in meters)
    double getMAC(TypeOfWing typeOfWing, double rootChord, double taperRatio, double span,
                  double kinkStation = 0.0, double taperInBoard = 0.0)
    {

        this->rootChord = rootChord;
        this->taperRatio = taperRatio;
        this->span = span;
        this->kinkStation = kinkStation;
        this->taperInBoard = taperInBoard;

        switch (typeOfWing)
        {

        case TypeOfWing::STRAIGHT_TAPERED:
        {

            MAC = (2.0 / 3.0) * rootChord * ((1 + taperRatio + std::pow(taperRatio, 2)) / (1 + taperRatio));
        }
        break;

        case TypeOfWing::CRANKED:
        {

            numeratorCranked = (std::pow(taperInBoard, 2) * taperInBoard * taperRatio + std::pow(taperRatio, 2)) +
                               (kinkStation / (0.5 * span - kinkStation)) * (1 + taperInBoard + std::pow(taperInBoard, 2));

            denominatorCranked = (taperInBoard + taperRatio) + (kinkStation / (0.5 * span - kinkStation)) * (1 + taperInBoard);

            MAC = (2.0 / 3.0) * rootChord * (numeratorCranked / denominatorCranked);
        }
        break;
        }

        return MAC;
    }


   /// @brief Calculate the spanwise location of the Mean Aerodynamic Chord (yMAC - in meters)
   /// @param typeOfWing Type of wing (e.g., STRAIGHT_TAPERED, CRANKED)
   /// @param span The wingspan (in meters)
   /// @param taperRatio The taper ratio of the wing
   /// @param kinkStation The y-station of the wing kink (for CRANKED wings - in meters)
   /// @param taperInBoard The taper ratio inboard of the kink (for CRANKED wings)
   /// @return The spanwise location of the Mean Aerodynamic Chord (yMAC - in meters)
    double getYMAC(TypeOfWing typeOfWing, double span, double taperRatio,
                   double kinkStation = 0.0, double taperInBoard = 0.0)
    {

        this->rootChord = rootChord;
        this->taperRatio = taperRatio;
        this->span = span;
        this->kinkStation = kinkStation;
        this->taperInBoard = taperInBoard;

        switch (typeOfWing)
        {

        case TypeOfWing::STRAIGHT_TAPERED:
        {

            yMAC = (span / 6.0) * ((1 + 2 * taperRatio) / (1 + taperRatio));
        }

        break;

        case TypeOfWing::CRANKED:
        {

            numeratorYMACCranked = taperInBoard * (1 + 2 * taperRatio * taperInBoard) + (kinkStation / (0.5 * span - kinkStation)) * 
                                   (2 + taperInBoard * taperRatio) + std::pow((kinkStation / (0.5 * span - kinkStation)), 2) * (1 + 2 * taperInBoard);

            denominatorYMACCranked = (taperInBoard + taperRatio) + (kinkStation / (0.5 * span - kinkStation)) * (1 + taperInBoard);

            yMAC = factorToYMACCranked * (0.5 * span - kinkStation) * (numeratorYMACCranked / denominatorYMACCranked);
        }

        break;
        }

        return yMAC;
    }
};
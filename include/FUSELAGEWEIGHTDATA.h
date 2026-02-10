#pragma once

#include <string>
#include <cmath>
#include "BASEWEIGHTDATA.h"


// Fuselage weight data class, inheriting from BaseWeightData (concetto di estensione)
class FuselageWeightData : public BaseWeightData
{

private:
    bool isPressurized = false;
    bool isCargo = false;

public:
     FuselageWeightData() = default;

    // Setters
    FuselageWeightData &setIsPressurized(bool value)
    {
        this->isPressurized = value;
        return *this;
    }

    FuselageWeightData &setIsCargo(bool value)
    {
        this->isCargo = value;
        return *this;
    }


    // Getters
    bool getIsPressurized() const { return isPressurized; }
    bool getIsCargo() const { return isCargo; }
};
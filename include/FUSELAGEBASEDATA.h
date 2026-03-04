#pragma once

#include <string>
#include <cmath>
#include "BASEAIRCRAFTDATA.h"


// Fuselage weight data class, inheriting from BaseAircraftData (concetto di estensione)
class FuselageBaseData : public BaseAircraftData
{

private:
    bool isPressurized = false;
    bool isCargo = false;

public:
     FuselageBaseData() = default;

    // Setters
    FuselageBaseData &setIsPressurized(bool value)
    {
        this->isPressurized = value;
        return *this;
    }

    FuselageBaseData &setIsCargo(bool value)
    {
        this->isCargo = value;
        return *this;
    }


    // Getters
    bool getIsPressurized() const { return isPressurized; }
    bool getIsCargo() const { return isCargo; }
};
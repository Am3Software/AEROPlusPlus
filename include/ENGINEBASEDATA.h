#pragma once

#include <string>
#include <cmath>
#include "BASEAIRCRAFTDATA.h"


// Engine weight data class, inheriting from BaseAircraftData (concetto di estensione)
class EngineBaseData : public BaseAircraftData
{

private:
    int numberOfEngines = 0;
    double thrustToPowerRatio = 0.0;
    double thrustLbf = 0.0;
    double BSHP = 0.0;
    double kPG = 0.0;
    double kTHR = 0.0;
    bool hasWaterInjectionSystem = false;
    bool hasAPU = false;

public:
     EngineBaseData() = default;

    // Setters
    EngineBaseData &setNumberOfEngines(int value)
    {
        this->numberOfEngines = value;
        return *this;
    }

    EngineBaseData &setThrustToPowerRatio(double value)
    {
        this->thrustToPowerRatio = value;
        return *this;
    }

    EngineBaseData &setThrustLbf(double value)
    {
        this->thrustLbf = value;
        return *this;
    }

    EngineBaseData &setBSHP(double value)
    {
        this->BSHP = value;
        return *this;
    }

    EngineBaseData &setKPG(double value)
    {
        this->kPG = value;
        return *this;
    }

    EngineBaseData &setKTHR(double value)
    {
        this->kTHR = value;
        return *this;
    }

    EngineBaseData &setHasWaterInjectionSystem(bool value)
    {
        this->hasWaterInjectionSystem = value;
        return *this;
    }

   

    // Getters
    int getNumberOfEngines() const { return numberOfEngines; }
    double getThrustToPowerRatio() const { return thrustToPowerRatio; }
    double getThrustLbf() const { return thrustLbf; }
    double getBSHP() const { return BSHP; }
    double getKPG() const { return kPG; }
    double getKTHR() const { return kTHR; }
    bool getHasWaterInjectionSystem() const { return hasWaterInjectionSystem; }
    bool getHasAPU() const { return hasAPU; }
};
#pragma once

#include <string>
#include <cmath>
#include "BASEWEIGHTDATA.h"


// Wing weight data class, inheriting from BaseWeightData (concetto di estensione)
class EngineWeightData : public BaseWeightData
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
     EngineWeightData() = default;

    // Setters
    EngineWeightData &setNumberOfEngines(int value)
    {
        this->numberOfEngines = value;
        return *this;
    }

    EngineWeightData &setThrustToPowerRatio(double value)
    {
        this->thrustToPowerRatio = value;
        return *this;
    }

    EngineWeightData &setThrustLbf(double value)
    {
        this->thrustLbf = value;
        return *this;
    }

    EngineWeightData &setBSHP(double value)
    {
        this->BSHP = value;
        return *this;
    }

    EngineWeightData &setKPG(double value)
    {
        this->kPG = value;
        return *this;
    }

    EngineWeightData &setKTHR(double value)
    {
        this->kTHR = value;
        return *this;
    }

    EngineWeightData &setHasWaterInjectionSystem(bool value)
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
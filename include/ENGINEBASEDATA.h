#pragma once

#include <string>
#include <cmath>
#include "BASEAIRCRAFTDATA.h"
#include "EnumTypeOfInlet.h"

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
    double criticalAltitude = 0.0;
    double sfc = 0.0;
    double sfcj = 0.0;
    bool hasWaterInjectionSystem = false;
    bool hasAPU = false;

    TypeOfInlet typeOfInlet;
    


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

    EngineBaseData &setHasAPU(bool value)
    {
        this->hasAPU = value;
        return *this;
    }

    EngineBaseData &setCriticalAltitude(double value)
    {
        this->criticalAltitude = value;
        return *this;
    }

    EngineBaseData &setSFC(double value)
    {
        this->sfc = value;
        return *this;
    }

    EngineBaseData &setSFCJ(double value)
    {
        this->sfcj = value;
        return *this;
    }

    EngineBaseData &setTypeOfInlet(TypeOfInlet typeOfInlet)
    {
        this->typeOfInlet = typeOfInlet;
        return *this;
    }

    // Getters
    int getNumberOfEngines() const { return numberOfEngines; }
    double getThrustToPowerRatio() const { return thrustToPowerRatio; }
    double getThrustLbf() const { return thrustLbf; }
    double getBSHP() const { return BSHP; }
    double getKPG() const { return kPG; }
    double getKTHR() const { return kTHR; }
    double getSFC() const { return sfc; }
    double getSFCJ() const { return sfcj; }
    double getCriticalAltitude() const { return criticalAltitude; }
    bool getHasWaterInjectionSystem() const { return hasWaterInjectionSystem; }
    bool getHasAPU() const { return hasAPU; }
    
    TypeOfInlet getTypeOfInlet() const { return typeOfInlet; }

};
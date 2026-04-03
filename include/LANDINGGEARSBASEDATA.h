#pragma once

#include <string>
#include <cmath>
#include "BASEAIRCRAFTDATA.h"
#include "EnumAircraftCategory.h"
#include "EnumAircraftEngineType.h"
#include "EnumEnginePosition.h"
#include "EnumMaterial.h"
#include "EnumTypeOfComposite.h"
#include "EnumTypeOfStabilizer.h"
#include "EnumTypeOfTail.h"
#include "EnumTypeOfWing.h"
#include "EnumUndercarriagePosition.h"
#include "EnumWeightMethod.h"
#include "EnumWingPosition.h"

// Landing gear data class, inheriting from BaseAircraftData (concetto di estensione)
class LandingGearsBaseData : public BaseAircraftData
{

private:
    bool isRetractableLandingGear = false;
    double strutMainWidth = 0.0;
    double strutMainDiameter = 0.0;
    double strutNoseWidth = 0.0;
    double strutNoseDiameter = 0.0;
    double noseWheelDiamter = 0.0;
    double noseWheelWidth = 0.0;
    double mainWheelDiameter = 0.0;
    double mainWheelWidth = 0.0;
    double strutLengthNose = 0.0;
    double strutLengthMain = 0.0;
    int numberOfNoseWheels = 0;
    int numberOfMainWheels = 0;
    int numberOfNoseStruts = 0;
    int numberOfMainStruts = 0;

public:
    LandingGearsBaseData() = default;

    // Setters
    
    LandingGearsBaseData &setIsRetractableLandingGear(bool value)
    {
        this->isRetractableLandingGear = value;
        return *this;
    }

    LandingGearsBaseData &setStrutMainWidth(double value)
    {
        this->strutMainWidth = value;
        return *this;
    }

    LandingGearsBaseData &setStrutMainDiameter(double value)
    {
        this->strutMainDiameter = value;
        return *this;
    }

    LandingGearsBaseData &setStrutNoseWidth(double value)
    {
        this->strutNoseWidth = value;
        return *this;
    }

    LandingGearsBaseData &setStrutNoseDiameter(double value)
    {
        this->strutNoseDiameter = value;
        return *this;
    }

    LandingGearsBaseData &setNoseWheelDiameter(double value)
    {
        this->noseWheelDiamter = value;
        return *this;
    }

    LandingGearsBaseData &setNoseWheelWidth(double value)
    {
        this->noseWheelWidth = value;
        return *this;
    }

    LandingGearsBaseData &setMainWheelDiameter(double value)
    {
        this->mainWheelDiameter = value;
        return *this;
    }

    LandingGearsBaseData &setMainWheelWidth(double value)
    {
        this->mainWheelWidth = value;
        return *this;
    }

    LandingGearsBaseData &setNumberOfNoseWheels(int value)
    {
        this->numberOfNoseWheels = value;
        return *this;
    }

    LandingGearsBaseData &setNumberOfMainWheels(int value)
    {
        this->numberOfMainWheels = value;
        return *this;
    }

    LandingGearsBaseData &setNumberOfNoseStruts(int value)
    {
        this->numberOfNoseStruts = value;
        return *this;
    }

    LandingGearsBaseData &setNumberOfMainStruts(int value)
    {
        this->numberOfMainStruts = value;
        return *this;
    }

    LandingGearsBaseData &setStrutLengthNose(double value)
    {
        this->strutLengthNose = value;
        return *this;
    }

    LandingGearsBaseData &setStrutLengthMain(double value)
    {
        this->strutLengthMain = value;
        return *this;
    }
       

    // Getters
    bool getIsRetractableLandingGear() const { return isRetractableLandingGear; }
    double getStrutMainWidth() const { return strutMainWidth; }
    double getStrutMainDiameter() const { return strutMainDiameter; }
    double getStrutNoseWidth() const { return strutNoseWidth; }
    double getStrutNoseDiameter() const { return strutNoseDiameter; }
    double getNoseWheelDiameter() const { return noseWheelDiamter; }
    double getNoseWheelWidth() const { return noseWheelWidth; }
    double getMainWheelDiameter() const { return mainWheelDiameter; }
    double getMainWheelWidth() const { return mainWheelWidth; }
    double getStrutLengthNose() const { return strutLengthNose; }
    double getStrutLengthMain() const { return strutLengthMain; }
    int getNumberOfNoseWheels() const { return numberOfNoseWheels; }
    int getNumberOfMainWheels() const { return numberOfMainWheels; }
    int getNumberOfNoseStruts() const { return numberOfNoseStruts; }
    int getNumberOfMainStruts() const { return numberOfMainStruts; }
  
};
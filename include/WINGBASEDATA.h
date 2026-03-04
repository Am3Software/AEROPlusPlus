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

// Wing weight data class, inheriting from BaseAircraftData (concetto di estensione)
class WingBaseData : public BaseAircraftData
{

private:
    double thicknessToRootChordRatioWing = 0.0;
    int nWingMountedEngines = 0;
    double wingKinkThicknessRatio = 0.0;
    double maxOperatingEAS = 0.0;
    double wingStrutPosition = 0.0;
    double strutToWingChordRatio = 0.0;
    double percentageComposite = 0.0; 
    bool isBracedWing = false;
    bool hasSpoilers = false;
    bool hasFowlerFlap = false;
    bool isEngineInstallatedOnWing = false;
    bool isFuelTankInstallatedOnWing = false;
    bool isSimplifiedFlapSystem = false;
    bool isCompositeWing = false;

public:
    WingBaseData() = default;

    // Setters
    WingBaseData &setThicknessToRootChordRatioWing(double value)
    {
        this->thicknessToRootChordRatioWing = value;
        return *this;
    }

    WingBaseData &setPercentageComposite(double value)
    {
        this->percentageComposite = value;
        return *this;
    }

    WingBaseData &setNWingMountedEngines(int value)
    {
        this->nWingMountedEngines = value;
        return *this;
    }

    WingBaseData &setWingKinkThicknessRatio(double value)
    {
        this->wingKinkThicknessRatio = value;
        return *this;
    }

    WingBaseData &setMaxOperatingEAS(double value)
    {
        this->maxOperatingEAS = value;
        return *this;
    }

    WingBaseData &setWingStrutPosition(double value)
    {
        this->wingStrutPosition = value;
        return *this;
    }

    WingBaseData &setStrutToWingChordRatio(double value)
    {
        this->strutToWingChordRatio = value;
        return *this;
    }

    WingBaseData &setIsBracedWing(bool value)
    {
        this->isBracedWing = value;
        return *this;
    }

    WingBaseData &setHasSpoilers(bool value)
    {
        this->hasSpoilers = value;
        return *this;
    }

    WingBaseData &setHasFowlerFlap(bool value)
    {
        this->hasFowlerFlap = value;
        return *this;
    }

    WingBaseData &setIsEngineInstallatedOnWing(bool value)
    {
        this->isEngineInstallatedOnWing = value;
        return *this;
    }

    WingBaseData &setIsFuelTankInstallatedOnWing(bool value)
    {
        this->isFuelTankInstallatedOnWing = value;
        return *this;
    }

    WingBaseData &setIsSimplifiedFlapSystem(bool value)
    {
        this->isSimplifiedFlapSystem = value;
        return *this;
    }

    WingBaseData &setIsCompositeWing(bool value)
    {
        this->isCompositeWing = value;
        return *this;
    }

    // Getters
    double getThicknessToRootChordRatioWing() const { return thicknessToRootChordRatioWing; }
    double getPercentageComposite() const { return percentageComposite; }
    int getNWingMountedEngines() const { return nWingMountedEngines; }
    double getWingKinkThicknessRatio() const { return wingKinkThicknessRatio; }
    double getMaxOperatingEAS() const { return maxOperatingEAS; }
    double getWingStrutPosition() const { return wingStrutPosition; }
    double getStrutToWingChordRatio() const { return strutToWingChordRatio; }
    bool getIsBracedWing() const { return isBracedWing; }
    bool getHasSpoilers() const { return hasSpoilers; }
    bool getHasFowlerFlap() const { return hasFowlerFlap; }
    bool getIsEngineInstallatedOnWing() const { return isEngineInstallatedOnWing; }
    bool getIsFuelTankInstallatedOnWing() const { return isFuelTankInstallatedOnWing; }
    bool getIsSimplifiedFlapSystem() const { return isSimplifiedFlapSystem; }
    bool getIsCompositeWing() const { return isCompositeWing; }
};
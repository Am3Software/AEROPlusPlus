#pragma once

#include <string>
#include <cmath>
#include "BASEWEIGHTDATA.h"
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

// Wing weight data class, inheriting from BaseWeightData (concetto di estensione)
class WingWeightData : public BaseWeightData
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
    WingWeightData() = default;

    // Setters
    WingWeightData &setThicknessToRootChordRatioWing(double value)
    {
        this->thicknessToRootChordRatioWing = value;
        return *this;
    }

    WingWeightData &setPercentageComposite(double value)
    {
        this->percentageComposite = value;
        return *this;
    }

    WingWeightData &setNWingMountedEngines(int value)
    {
        this->nWingMountedEngines = value;
        return *this;
    }

    WingWeightData &setWingKinkThicknessRatio(double value)
    {
        this->wingKinkThicknessRatio = value;
        return *this;
    }

    WingWeightData &setMaxOperatingEAS(double value)
    {
        this->maxOperatingEAS = value;
        return *this;
    }

    WingWeightData &setWingStrutPosition(double value)
    {
        this->wingStrutPosition = value;
        return *this;
    }

    WingWeightData &setStrutToWingChordRatio(double value)
    {
        this->strutToWingChordRatio = value;
        return *this;
    }

    WingWeightData &setIsBracedWing(bool value)
    {
        this->isBracedWing = value;
        return *this;
    }

    WingWeightData &setHasSpoilers(bool value)
    {
        this->hasSpoilers = value;
        return *this;
    }

    WingWeightData &setHasFowlerFlap(bool value)
    {
        this->hasFowlerFlap = value;
        return *this;
    }

    WingWeightData &setIsEngineInstallatedOnWing(bool value)
    {
        this->isEngineInstallatedOnWing = value;
        return *this;
    }

    WingWeightData &setIsFuelTankInstallatedOnWing(bool value)
    {
        this->isFuelTankInstallatedOnWing = value;
        return *this;
    }

    WingWeightData &setIsSimplifiedFlapSystem(bool value)
    {
        this->isSimplifiedFlapSystem = value;
        return *this;
    }

    WingWeightData &setIsCompositeWing(bool value)
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
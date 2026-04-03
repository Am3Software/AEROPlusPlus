#pragma once

#include <string>
#include <cmath>
#include "BASEAIRCRAFTDATA.h"

class WeightCorrectionFactor : public BaseAircraftData
{

private:
    
    // Correction factors
    double kWing = 1.0;
    double kCanard = 1.0;
    double kHorizontal = 1.0;
    double kVertical = 1.0;
    double kVentralFin = 1.0;
    double kFuselage = 1.0;
    double kBoom = 1.0;
    double kEoir = 1.0;
    double kUnderCarriage = 1.0;
    double kAPU = 1.0;
    double kControlSurfaces = 1.0;
    double kPropulsionGroup = 1.0;
    double kInstruments = 1.0;
    double kHydraulicAndPneumatic = 1.0;
    double kElectricalGroup = 1.0;
    double kAvionic = 1.0;
    double kFurnishingAndEquipment = 1.0;
    double kAntiIcingAndConditioning = 1.0;
    double kOperatingItems = 1.0;
    double kPayload = 1.0;
    double kCrew = 1.0;
    double kFuel = 1.0;

public:
    WeightCorrectionFactor() = default;

    // Setters

    WeightCorrectionFactor &setKWing(double value)
    {
        this->kWing = value;
        return *this;
    }

    WeightCorrectionFactor &setKCanard(double value)
    {
        this->kCanard = value;
        return *this;
    }

    WeightCorrectionFactor &setKHorizontal(double value)
    {
        this->kHorizontal = value;
        return *this;
    }

    WeightCorrectionFactor &setKVertical(double value)
    {
        this->kVertical = value;
        return *this;
    }

    WeightCorrectionFactor &setKVentralFin(double value)
    {
        this->kVentralFin = value;
        return *this;
    }

    WeightCorrectionFactor &setKFuselage(double value)
    {
        this->kFuselage = value;
        return *this;
    }

    WeightCorrectionFactor &setKBoom(double value)
    {
        this->kBoom = value;
        return *this;
    }

    WeightCorrectionFactor &setKEoir(double value)
    {
        this->kEoir = value;
        return *this;
    }

    WeightCorrectionFactor &setKUnderCarriage(double value)
    {
        this->kUnderCarriage = value;
        return *this;
    }

    WeightCorrectionFactor &setKAPU(double value)
    {
        this->kAPU = value;
        return *this;
    }

    WeightCorrectionFactor &setKControlSurfaces(double value)
    {
        this->kControlSurfaces = value;
        return *this;
    }

    WeightCorrectionFactor &setKPropulsionGroup(double value)
    {
        this->kPropulsionGroup = value;
        return *this;
    }

    WeightCorrectionFactor &setKInstruments(double value)
    {
        this->kInstruments = value;
        return *this;
    }

    WeightCorrectionFactor &setKHydraulicAndPneumatic(double value)
    {
        this->kHydraulicAndPneumatic = value;
        return *this;
    }

    WeightCorrectionFactor &setKElectricalGroup(double value)
    {
        this->kElectricalGroup = value;
        return *this;
    }

    WeightCorrectionFactor &setKAvionic(double value)
    {
        this->kAvionic = value;
        return *this;
    }

    WeightCorrectionFactor &setKFurnishingAndEquipment(double value)
    {
        this->kFurnishingAndEquipment = value;
        return *this;
    }

    WeightCorrectionFactor &setKAntiIcingAndConditioning(double value)
    {
        this->kAntiIcingAndConditioning = value;
        return *this;
    }

    WeightCorrectionFactor &setKOperatingItems(double value)
    {
        this->kOperatingItems = value;
        return *this;
    }

    WeightCorrectionFactor &setKPayload(double value)
    {
        this->kPayload = value;
        return *this;
    }

    WeightCorrectionFactor &setKCrew(double value)
    {
        this->kCrew = value;
        return *this;
    }

    WeightCorrectionFactor &setKFuel(double value)
    {
        this->kFuel = value;
        return *this;
    }

    // Getters
  
    double getKWing() const { return kWing; }
    double getKCanard() const { return kCanard; }
    double getKHorizontal() const { return kHorizontal; }
    double getKVertical() const { return kVertical; }
    double getKVentralFin() const { return kVentralFin; }
    double getKFuselage() const { return kFuselage; }
    double getKBoom() const { return kBoom; }
    double getKEoir() const { return kEoir; }
    double getKUnderCarriage() const { return kUnderCarriage; }
    double getKAPU() const { return kAPU; }
    double getKControlSurfaces() const { return kControlSurfaces; }
    double getKPropulsionGroup() const { return kPropulsionGroup; }
    double getKInstruments() const { return kInstruments; }
    double getKHydraulicAndPneumatic() const { return kHydraulicAndPneumatic; }
    double getKElectricalGroup() const { return kElectricalGroup; }
    double getKAvionic() const { return kAvionic; }
    double getKFurnishingAndEquipment() const { return kFurnishingAndEquipment; }
    double getKAntiIcingAndConditioning() const { return kAntiIcingAndConditioning; }
    double getKOperatingItems() const { return kOperatingItems; }
    double getKPayload() const { return kPayload; }
    double getKCrew() const { return kCrew; }
    double getKFuel() const { return kFuel; }
};
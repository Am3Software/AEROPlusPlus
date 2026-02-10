#pragma once

#include <string>
#include <cmath>
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

class BaseWeightData
{

protected:

    double WTO = 0.0;
    double payloadWeight = 0.0;
    double fuelWeight = 0.0;
    double crewWeight = 0.0;

    // Wing variables related
    double nUltimateLoad = 0.0;
    double diveSpeed = 0.0;
    bool isSweptWing = false;
    double kMainSparPosition = 0.25;
    double kSecondarySparPosition = 0.55;

    // Canard variables related
    bool hasCanard = false;
    double thicknessToRootChordRatioCanard = 0.0;

    // Tail boom variables related
    int numberOfBooms = 0;
    bool hasBoom = false;

     // EO/IR variables related
     bool hasEOIR = false;
   
    // Landing gear variables related
    bool isRetractableLandingGear = false;
    double strutLength = 0.0;

    // Control surfaces variables related
    double factorToControlSurface = 0.0;

    // Engine group variables related
    int numberOfEngines = 0;
    double thrustToPowerRatio = 0.0;
    double thrustLbf = 0.0;
    double BSHP = 0.0;

    // Operating items variables related
    int numberOfPax = 0;
    double rangeCovered = 0.0;

    // Genral info
    bool hasFurnishinghAndEquipment = true;
    bool hasAirConditioningAndAntiIce = true;
    bool hasAPU = false;


    // Enumeration data
    AircraftCategory aircraftCategory;
    AircraftEngineType aircraftEngineType;
    UndercarriagePosition undercarriagePosition;
    EnginePosition enginePosition;
    Material frameMaterial;
    Material stringersMaterial;
    Material skinMaterial;
    TypeOfComposite typeOfComposite;
    TypeOfStabilizer typeOfStabilizer;
    TypeOfTail typeOfTail;
    TypeOfWing typeOfWing;
    WeightMethod weightMethodWing;
    WeightMethod weightMethodFuselage;
    WingPosition wingPosition;

public:

    BaseWeightData() = default;

    BaseWeightData & setWTO(double WTO)
    {

        this->WTO = WTO;
        return *this;
    }

    BaseWeightData & setPayloadWeight(double payloadWeight)
    {

        this->payloadWeight = payloadWeight;
        return *this;
    }

    BaseWeightData & setCrewWeight(double crewWeight)
    {

        this->crewWeight = crewWeight;
        return *this;
    }

    BaseWeightData & setFuelWeight(double fuelWeight)
    {

        this->fuelWeight = fuelWeight;
        return *this;
    }

    BaseWeightData & setDiveSpeed(double diveSpeed){

        this->diveSpeed = diveSpeed;
        return *this;
    }

     BaseWeightData & setNUltimateLoad(double nUltimateLoad)
    {

        this->nUltimateLoad = nUltimateLoad;
        return *this;
    }


    BaseWeightData & setIsSweptWing(bool isSweptWing)
    {

        this->isSweptWing = isSweptWing;
        return *this;
    }

    BaseWeightData & setKMainSparPosition(double kMainSparPosition)
    {

        this->kMainSparPosition = kMainSparPosition;
        return *this;
    }

    BaseWeightData & setKSecondarySparPosition(double kSecondarySparPosition)
    {

        this->kSecondarySparPosition = kSecondarySparPosition;
        return *this;
    }

    /// @brief Set tail boom properties
    /// @param numberofBooms The number of tail booms
    /// @param diveSpeedBoom The dive speed for the tail boom in m/s
    BaseWeightData & setNumbersOfBoom(int numberOfBooms)
    {
        this->numberOfBooms = numberOfBooms;

        return *this;
       
    }

    /// @brief Set landing gear properties
    /// @param isRetractableLandingGear Whether the landing gear is retractable or not
    BaseWeightData & setLandingGearProperties(bool isRetractableLandingGear)
    {
        this->isRetractableLandingGear = isRetractableLandingGear;
        return *this;
    }

    BaseWeightData & setStrutLength(double strutLength)
    {
        this->strutLength = strutLength;
        return *this;
    }

    /// @brief Set control surfaces properties
    /// @param factorToControlSurface Control surface factor (Ksc = 0.23 to flight airplanes without duplicated system controls - Ksc = 0.44 to transport airplanes, manually controlled - Ksc = 0.64 to transport airplanes, with powered controls and TED high-lift devices)
    BaseWeightData & setControlSurfacesProperties(double factorToControlSurface)
    {
        this->factorToControlSurface = factorToControlSurface;
        return *this;
    }


    /// @brief Set operating items properties
    /// @param numberOfPax The number of passengers
    /// @param rangeCovered The range covered in nautical miles
    BaseWeightData & setNumberOfPax(int numberOfPax)
    {
        this->numberOfPax = numberOfPax;
        return *this;
    }

    BaseWeightData & setRangeCovered(double rangeCovered)
    {
        this->rangeCovered = rangeCovered;
        return *this;
    }
    /// @brief Set aircraft category
    /// @param category The aircraft category
    BaseWeightData & setAircraftCategory(AircraftCategory category)
    {
        this->aircraftCategory = category;
        return *this;
    }

    BaseWeightData & setAircraftEngineType(AircraftEngineType engineType)
    {
        this->aircraftEngineType = engineType;
        return *this;
    }

    BaseWeightData & setUndercarriagePosition(UndercarriagePosition position)
    {
        this->undercarriagePosition = position;
        return *this;
    }

    BaseWeightData & setEnginePosition(EnginePosition position)
    {
        this->enginePosition = position;
        return *this;
    }

    BaseWeightData & setFrameMaterial(Material material)
    {
        this->frameMaterial = material;
        return *this;
    }

    BaseWeightData & setStringersMaterial(Material material)
    {
        this->stringersMaterial = material;
        return *this;
    }

    BaseWeightData & setSkinMaterial(Material material)
    {
        this->skinMaterial = material;
        return *this;
    }

    BaseWeightData & setTypeOfComposite(TypeOfComposite type)
    {
        this->typeOfComposite = type;
        return *this;
    }

    BaseWeightData & setTypeOfStabilizer(TypeOfStabilizer type)
    {
        this->typeOfStabilizer = type;
        return *this;
    }

    BaseWeightData & setTypeOfTail(TypeOfTail type)
    {
        this->typeOfTail = type;
        return *this;
    }

    BaseWeightData & setTypeOfWing(TypeOfWing type)
    {
        this->typeOfWing = type;
        return *this;
    }

    BaseWeightData & setWeightMethodWing(WeightMethod methodWing)
    {
        this->weightMethodWing = methodWing;
        return *this;
    }

    BaseWeightData & setWeightMethodFuselage(WeightMethod methodFuselage)
    {
        this->weightMethodFuselage = methodFuselage;
        return *this;
    }

    BaseWeightData & setWingPosition(WingPosition position)
    {
        this->wingPosition = position;
        return *this;
    }

    BaseWeightData & setHasCanard(bool hasCanard)
    {
        this->hasCanard = hasCanard;
        return *this;
    }

    BaseWeightData & setHasBoom(bool hasBoom)
    {
        this->hasBoom = hasBoom;
        return *this;
    }

     BaseWeightData & setHasEOIR(bool hasEOIR)
    {
        this->hasEOIR = hasEOIR;
        return *this;
    }

    BaseWeightData & setThicknessToRootChordRatioCanard(double value)
    {
        this->thicknessToRootChordRatioCanard = value;
        return *this;
    }

    BaseWeightData & setHasFurnishinghAndEquipment(bool hasFurnishinghAndEquipment)
    {
        this->hasFurnishinghAndEquipment = hasFurnishinghAndEquipment;
        return *this;
    }

    BaseWeightData & setHasAirConditioningAndAntiIce(bool hasAirConditioningAndAntiIce)
    {
        this->hasAirConditioningAndAntiIce = hasAirConditioningAndAntiIce;
        return *this;
    }

     BaseWeightData &setHasAPU(bool hasAPU)
    {
        this->hasAPU = hasAPU;
        return *this;
    }

    // ========== GETTERS ==========

    double getWTO() const { return WTO; }
    double getPayloadWeight() const { return payloadWeight; }
    double getCrewWeight() const { return crewWeight; }
    double getFuelWeight() const { return fuelWeight; }

    double getDiveSpeed() const { return diveSpeed; }

    //Wing getters
    double getNUltimateLoad() const { return nUltimateLoad; }
    double getKMainSparPosition() const { return kMainSparPosition; }
    double getKSecondarySparPosition() const { return kSecondarySparPosition; }
    bool getIsSweptWing() const { return isSweptWing; }

    // Canard getters
    bool getHasCanard() const { return hasCanard; }
    double getThicknessToRootChordRatioCanard() const { return thicknessToRootChordRatioCanard; }

    
    // Tail boom getters
    int getNumberOfBooms() const { return numberOfBooms; }
    bool getHasBoom() const { return hasBoom; }

     // EO/IR getters
     bool getHasEOIR() const { return hasEOIR; }
    

    // Landing gear getters
    bool getIsRetractableLandingGear() const { return isRetractableLandingGear; }
    double getStrutLength() const { return strutLength; }

    // Control surfaces getters
    double getFactorToControlSurface() const { return factorToControlSurface; }


    // Operating items getters
    int getNumberOfPax() const { return numberOfPax; }
    double getRangeCovered() const { return rangeCovered; }

    // Genral info getters
    bool getHasFurnishinghAndEquipment() const { return hasFurnishinghAndEquipment; }
    bool getHasAirConditioningAndAntiIce() const { return hasAirConditioningAndAntiIce; }
    bool getHasAPU() const { return hasAPU; }
    

    // Aircraft category getter
    AircraftCategory getAircraftCategory() const { return aircraftCategory; }

    // Aircraft engine type getter
    AircraftEngineType getAircraftEngineType() const { return aircraftEngineType; }

    // Undercarriage position getter
    UndercarriagePosition getUndercarriagePosition() const { return undercarriagePosition; }

    // Engine position getter
    EnginePosition getEnginePosition() const { return enginePosition; }

    // Frame material getters
    Material getFrameMaterial() const { return frameMaterial; }

    // Stringers material getters
    Material getStringersMaterial() const { return stringersMaterial; }

    // Skin material getters
    Material getSkinMaterial() const { return skinMaterial; }

    // Type of composite getter
    TypeOfComposite getTypeOfComposite() const { return typeOfComposite; }

    // Type of stabilizer getter
    TypeOfStabilizer getTypeOfStabilizer() const { return typeOfStabilizer; }

    // Type of tail getter
    TypeOfTail getTypeOfTail() const { return typeOfTail; }

    // Type of wing getter
    TypeOfWing getTypeOfWing() const { return typeOfWing; }

    // Weight method getter
    WeightMethod getWeightMethodWing() const { return weightMethodWing; }
    WeightMethod getWeightMethodFuselage() const { return weightMethodFuselage; }

    // Wing position getter
    WingPosition getWingPosition() const { return wingPosition; }
};
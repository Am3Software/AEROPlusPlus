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
#include "EnumAircraftCategory.h"
#include "EnumAircraftEngineCategory.h"
#include "EnumSkinRoughnessType.h"
#include "EnumWingFairingType.h"
#include "EnumTypeOfFlap.h"
#include "EnumWindScreenType.h"

class BaseAircraftData
{

protected:
    std::string nameOfAircraft;

    double WTO = 0.0;
    double payloadWeight = 0.0;
    double fuelWeight = 0.0;
    double crewWeight = 0.0;

    // Wing variables related
    double nUltimateLoad = 0.0;
    double adimAerodynamicCenter = 0.0;
    double diveSpeed = 0.0;
    bool isSweptWing = false;
    double kMainSparPosition = 0.25;
    double kSecondarySparPosition = 0.55;
    double meanAirfoilSlopeWing = 0.0;
    double meanAirfoilSlopeHorizontalTail = 0.0;
    double meanAirfoilSlopeVerticalTail = 0.0;

    // Canard variables related
    bool hasCanard = false;
    double thicknessToRootChordRatioCanard = 0.0;
    double meanAirfoilSlopeCanard = 0.0;

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
    int numberOfBlades = 0;
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

    // CRew weight related variables
    int numberOfCrewMembers = 0;

    // Enumeration data
    AircraftCategory aircraftCategory;
    AircraftEngineType aircraftEngineType;
    AircraftEngineCategory aircraftEngineCategory;
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
    WingPosition canardPosition;
    SkinRoughnessType skinRoughnessTypeWing;
    SkinRoughnessType skinRoughnessTypeCanard;
    SkinRoughnessType skinRoughnessTypeHorizontalTail;
    SkinRoughnessType skinRoughnessTypeVerticalTail;
    SkinRoughnessType skinRoughnessTypeFuselage;
    SkinRoughnessType skinRoughnessTypeNacelle;
    SkinRoughnessType skinRoughnessTypeBoom;
    WingFairingType wingFairingType;
    WingFairingType canardFairingType;
    TypeOfFlap typeOfFlap;
    WindScreenType windScreenType;

public:
    BaseAircraftData() = default;

    BaseAircraftData &setNameOfAircraft(const std::string &nameOfAircraft)
    {
        this->nameOfAircraft = nameOfAircraft;
        return *this;
    }

    BaseAircraftData &setWTO(double WTO)
    {

        this->WTO = WTO;
        return *this;
    }

    BaseAircraftData &setPayloadWeight(double payloadWeight)
    {

        this->payloadWeight = payloadWeight;
        return *this;
    }

    BaseAircraftData &setNumberOfCrewMembers(int numberOfCrewMembers)
    {

        this->numberOfCrewMembers = numberOfCrewMembers;
        return *this;
    }

    BaseAircraftData &setCrewWeight(double crewWeight)
    {

        this->crewWeight = crewWeight;
        return *this;
    }

    BaseAircraftData &setFuelWeight(double fuelWeight)
    {

        this->fuelWeight = fuelWeight;
        return *this;
    }

    BaseAircraftData &setDiveSpeed(double diveSpeed)
    {

        this->diveSpeed = diveSpeed;
        return *this;
    }

    BaseAircraftData &setNUltimateLoad(double nUltimateLoad)
    {

        this->nUltimateLoad = nUltimateLoad;
        return *this;
    }

    BaseAircraftData &setAdimAerodynamicCenter(double adimAerodynamicCenter)
    {

        this->adimAerodynamicCenter = adimAerodynamicCenter;
        return *this;
    }

    BaseAircraftData &setIsSweptWing(bool isSweptWing)
    {

        this->isSweptWing = isSweptWing;
        return *this;
    }

    BaseAircraftData &setKMainSparPosition(double kMainSparPosition)
    {

        this->kMainSparPosition = kMainSparPosition;
        return *this;
    }

    BaseAircraftData &setKSecondarySparPosition(double kSecondarySparPosition)
    {

        this->kSecondarySparPosition = kSecondarySparPosition;
        return *this;
    }

    BaseAircraftData &setMeanAirfoilSlopeWing(double meanAirfoilSlopeWing)
    {
        this->meanAirfoilSlopeWing = meanAirfoilSlopeWing;
        return *this;
    }

    BaseAircraftData &setMeanAirfoilSlopeHorizontalTail(double meanAirfoilSlopeHorizontalTail)
    {
        this->meanAirfoilSlopeHorizontalTail = meanAirfoilSlopeHorizontalTail;
        return *this;
    }

    /// @brief Set tail boom properties
    /// @param numberofBooms The number of tail booms
    /// @param diveSpeedBoom The dive speed for the tail boom in m/s
    BaseAircraftData &setNumbersOfBoom(int numberOfBooms)
    {
        this->numberOfBooms = numberOfBooms;

        return *this;
    }

    /// @brief Set landing gear properties
    /// @param isRetractableLandingGear Whether the landing gear is retractable or not
    BaseAircraftData &setLandingGearProperties(bool isRetractableLandingGear)
    {
        this->isRetractableLandingGear = isRetractableLandingGear;
        return *this;
    }

    BaseAircraftData &setStrutLength(double strutLength)
    {
        this->strutLength = strutLength;
        return *this;
    }

    /// @brief Set control surfaces properties
    /// @param factorToControlSurface Control surface factor (Ksc = 0.23 to flight airplanes without duplicated system controls - Ksc = 0.44 to transport airplanes, manually controlled - Ksc = 0.64 to transport airplanes, with powered controls and TED high-lift devices)
    BaseAircraftData &setControlSurfacesProperties(double factorToControlSurface)
    {
        this->factorToControlSurface = factorToControlSurface;
        return *this;
    }

    /// @brief Set operating items properties
    /// @param numberOfPax The number of passengers
    /// @param rangeCovered The range covered in nautical miles
    BaseAircraftData &setNumberOfPax(int numberOfPax)
    {
        this->numberOfPax = numberOfPax;
        return *this;
    }

    BaseAircraftData &setRangeCovered(double rangeCovered)
    {
        this->rangeCovered = rangeCovered;
        return *this;
    }
    /// @brief Set aircraft category
    /// @param category The aircraft category
    BaseAircraftData &setAircraftCategory(AircraftCategory category)
    {
        this->aircraftCategory = category;
        return *this;
    }

    BaseAircraftData &setAircraftEngineType(AircraftEngineType engineType)
    {
        this->aircraftEngineType = engineType;
        return *this;
    }

    BaseAircraftData &setUndercarriagePosition(UndercarriagePosition position)
    {
        this->undercarriagePosition = position;
        return *this;
    }

    BaseAircraftData &setEnginePosition(EnginePosition position)
    {
        this->enginePosition = position;
        return *this;
    }

    BaseAircraftData &setFrameMaterial(Material material)
    {
        this->frameMaterial = material;
        return *this;
    }

    BaseAircraftData &setStringersMaterial(Material material)
    {
        this->stringersMaterial = material;
        return *this;
    }

    BaseAircraftData &setSkinMaterial(Material material)
    {
        this->skinMaterial = material;
        return *this;
    }

    BaseAircraftData &setTypeOfComposite(TypeOfComposite type)
    {
        this->typeOfComposite = type;
        return *this;
    }

    BaseAircraftData &setTypeOfStabilizer(TypeOfStabilizer type)
    {
        this->typeOfStabilizer = type;
        return *this;
    }

    BaseAircraftData &setTypeOfTail(TypeOfTail type)
    {
        this->typeOfTail = type;
        return *this;
    }

    BaseAircraftData &setTypeOfWing(TypeOfWing type)
    {
        this->typeOfWing = type;
        return *this;
    }

    BaseAircraftData &setWeightMethodWing(WeightMethod methodWing)
    {
        this->weightMethodWing = methodWing;
        return *this;
    }

    BaseAircraftData &setWeightMethodFuselage(WeightMethod methodFuselage)
    {
        this->weightMethodFuselage = methodFuselage;
        return *this;
    }

    BaseAircraftData &setWingPosition(WingPosition position)
    {
        this->wingPosition = position;
        return *this;
    }

    BaseAircraftData &setHasCanard(bool hasCanard)
    {
        this->hasCanard = hasCanard;
        return *this;
    }

    BaseAircraftData &setMeanAirfoilSlopeCanard(double slope)
    {
        this->meanAirfoilSlopeCanard = slope;
        return *this;
    }

    BaseAircraftData &setHasBoom(bool hasBoom)
    {
        this->hasBoom = hasBoom;
        return *this;
    }

    BaseAircraftData &setHasEOIR(bool hasEOIR)
    {
        this->hasEOIR = hasEOIR;
        return *this;
    }

    BaseAircraftData &setThicknessToRootChordRatioCanard(double value)
    {
        this->thicknessToRootChordRatioCanard = value;
        return *this;
    }

    BaseAircraftData &setHasFurnishinghAndEquipment(bool hasFurnishinghAndEquipment)
    {
        this->hasFurnishinghAndEquipment = hasFurnishinghAndEquipment;
        return *this;
    }

    BaseAircraftData &setHasAirConditioningAndAntiIce(bool hasAirConditioningAndAntiIce)
    {
        this->hasAirConditioningAndAntiIce = hasAirConditioningAndAntiIce;
        return *this;
    }

    BaseAircraftData &setHasAPU(bool hasAPU)
    {
        this->hasAPU = hasAPU;
        return *this;
    }

    BaseAircraftData &setNumberOfBlades(int numberOfBBlades)
    {

        this->numberOfBlades = numberOfBBlades;
        return *this;
    }

    BaseAircraftData &setMeanAirfoilVerticalSlope(double meanAirfoilSlopeVerticalTail)
    {
        this->meanAirfoilSlopeVerticalTail = meanAirfoilSlopeVerticalTail;
        return *this;
    }

    BaseAircraftData &setAircraftEngineCategory(AircraftEngineCategory engineCategory)
    {
        this->aircraftEngineCategory = engineCategory;
        return *this;
    }

    BaseAircraftData &setSkinRoughnessTypeWing(SkinRoughnessType skinRoughnessTypeWing)
    {
        this->skinRoughnessTypeWing = skinRoughnessTypeWing;
        return *this;
    }

    BaseAircraftData &setSkinRoughnessTypeCanard(SkinRoughnessType skinRoughnessTypeCanard)
    {
        this->skinRoughnessTypeCanard = skinRoughnessTypeCanard;
        return *this;
    }

    BaseAircraftData &setSkinRoughnessTypeHorizontalTail(SkinRoughnessType skinRoughnessTypeHorizontalTail)
    {
        this->skinRoughnessTypeHorizontalTail = skinRoughnessTypeHorizontalTail;
        return *this;
    }

    BaseAircraftData &setSkinRoughnessTypeVerticalTail(SkinRoughnessType skinRoughnessTypeVerticalTail)
    {
        this->skinRoughnessTypeVerticalTail = skinRoughnessTypeVerticalTail;
        return *this;
    }

    BaseAircraftData &setSkinRoughnessTypeFuselage(SkinRoughnessType skinRoughnessTypeFuselage)
    {
        this->skinRoughnessTypeFuselage = skinRoughnessTypeFuselage;
        return *this;
    }

    BaseAircraftData &setSkinRoughnessTypeNacelle(SkinRoughnessType skinRoughnessTypeNacelle)
    {
        this->skinRoughnessTypeNacelle = skinRoughnessTypeNacelle;
        return *this;
    }

    BaseAircraftData &setSkinRoughnessTypeBoom(SkinRoughnessType skinRoughnessTypeBoom)
    {
        this->skinRoughnessTypeBoom = skinRoughnessTypeBoom;
        return *this;
    }

    BaseAircraftData &setWingFairingType(WingFairingType wingFairingType)
    {
        this->wingFairingType = wingFairingType;
        return *this;
    }

    BaseAircraftData &setTypeOfFlap(TypeOfFlap typeOfFlap)
    {
        this->typeOfFlap = typeOfFlap;
        return *this;
    }

    BaseAircraftData &setCanardPosition(WingPosition canardPosition)
    {
        this->canardPosition = canardPosition;
        return *this;
    }

    BaseAircraftData &setCanardFairingType(WingFairingType canardFairingType)
    {
        this->canardFairingType = canardFairingType;
        return *this;
    }

    BaseAircraftData &setWindScreenType(WindScreenType windScreenType)
    {
        this->windScreenType = windScreenType;
        return *this;
    }

    // ========== GETTERS ==========

    std::string getNameOfAircraft() const { return nameOfAircraft; }
    double getWTO() const { return WTO; }
    double getPayloadWeight() const { return payloadWeight; }
    int getNumberOfCrewMembers() const { return numberOfCrewMembers; }
    double getCrewWeight() const { return crewWeight; }
    double getFuelWeight() const { return fuelWeight; }

    double getDiveSpeed() const { return diveSpeed; }

    // Wing getters
    double getNUltimateLoad() const { return nUltimateLoad; }
    double getAdimAerodynamicCenter() const { return adimAerodynamicCenter; }
    double getKMainSparPosition() const { return kMainSparPosition; }
    double getKSecondarySparPosition() const { return kSecondarySparPosition; }
    bool getIsSweptWing() const { return isSweptWing; }
   

    // Canard getters
    bool getHasCanard() const { return hasCanard; }
    double getThicknessToRootChordRatioCanard() const { return thicknessToRootChordRatioCanard; }
    double getMeanAirfoilSlopeCanard() const { return meanAirfoilSlopeCanard; }

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
    double getMeanAirfoilSlopeWing() const { return meanAirfoilSlopeWing; }
    double getMeanAirfoilSlopeHorizontalTail() const { return meanAirfoilSlopeHorizontalTail; }
    double getMeanAirfoilSlopeVerticalTail() const { return meanAirfoilSlopeVerticalTail; }
    bool getHasFurnishinghAndEquipment() const { return hasFurnishinghAndEquipment; }
    bool getHasAirConditioningAndAntiIce() const { return hasAirConditioningAndAntiIce; }
    bool getHasAPU() const { return hasAPU; }
    int getNumberOfBlades() const { return numberOfBlades; }

    // Aircraft category getter
    AircraftCategory getAircraftCategory() const { return aircraftCategory; }

    // Aircraft engine type getter
    AircraftEngineType getAircraftEngineType() const { return aircraftEngineType; }

    AircraftEngineCategory getAircraftEngineCategory() const { return aircraftEngineCategory; }

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

    // Canard position getter
    WingPosition getCanardPosition() const { return canardPosition; }

    //Skin roughness type getter
    SkinRoughnessType getSkinRoughnessTypeWing() const { return skinRoughnessTypeWing; }
    SkinRoughnessType getSkinRoughnessTypeCanard() const { return skinRoughnessTypeCanard; }
    SkinRoughnessType getSkinRoughnessTypeHorizontalTail() const { return skinRoughnessTypeHorizontalTail; }
    SkinRoughnessType getSkinRoughnessTypeVerticalTail() const { return skinRoughnessTypeVerticalTail; }
    SkinRoughnessType getSkinRoughnessTypeFuselage() const { return skinRoughnessTypeFuselage; }
    SkinRoughnessType getSkinRoughnessTypeNacelle() const { return skinRoughnessTypeNacelle; }
    SkinRoughnessType getSkinRoughnessTypeBoom() const { return skinRoughnessTypeBoom; }

    // Wing fairing type getter
    WingFairingType getWingFairingType() const { return wingFairingType; }

    // Canard fairing type getter
    WingFairingType getCanardFairingType() const { return canardFairingType; }

    // Type of flap getter
    TypeOfFlap getTypeOfFlap() const { return typeOfFlap; }

    // Wind screen type getter
    WindScreenType getWindScreenType() const { return windScreenType; }
};
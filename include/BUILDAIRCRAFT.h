#pragma once

#include "BASEAIRCRAFTDATA.h"
#include "WINGBASEDATA.h"
#include "FUSELAGEBASEDATA.h"
#include "ENGINEBASEDATA.h"
#include "WEIGHTS.h"
#include "XMLPARSER.h"
#include "VSPAeroGenerator.h"
#include "ATMOSISA.h"
#include "ConvDensity.h"
#include "EnumDensity.h"
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
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <list>
#include <format>
#include <cmath>
#include <numeric>
#include <cctype>
#include <string>

// ============================================================
// Struct that aggregates all aircraft data
// Returned by buildAircraft() and can be passed to COGCalculator
// ============================================================
struct AircraftBuildData
{
    BaseAircraftData commonData;
    WingBaseData wingData;
    FuselageBaseData fuselageData;
    EngineBaseData engineData;
};

class BuildAircraft
{

protected:
    BaseAircraftData commonData;
    WingBaseData wingData;
    FuselageBaseData fuselageData;
    EngineBaseData engineData;
    XMLUtil::XMLParser parser;
    VSP::AeroSettings settings;

private:
    std::string nameOfAircraft;
    std::string currentWorkDirectory = "";
    // Essendo static constexpr, esiste a compile-time e non dipende dall'ordine di inizializzazione, quindi sarà sempre disponibile nel costruttore.
    // Dato che std::string non è un tipo literal, cioè alloca memoria dinamicamente (sull'heap tramite new), si deve usare const char* per garantire che sia un'espressione costante a compile-time.
    static constexpr const char *folderNameWeightsSettings = "AircraftSettings";
    double seaLevelDensity = 0.0;
    double densityRatio = 0.0;
    double maxOperatingEAS = 0.0;

public:
    BuildAircraft(std::string nameOfAircraft, VSP::AeroSettings settings)
        : nameOfAircraft(nameOfAircraft),
          settings(settings),
          currentWorkDirectory(std::filesystem::current_path().string()),
          parser(std::filesystem::current_path().string() + "/" + folderNameWeightsSettings + "/" + nameOfAircraft + "_aircraftSettings.xml")
    {
    }

    // ============================================================
    // Getters to access individual data blocks after buildAircraft()
    // ============================================================
    BaseAircraftData getCommonData() const { return commonData; }
    WingBaseData getWingData() const { return wingData; }
    FuselageBaseData getFuselageData() const { return fuselageData; }
    EngineBaseData getEngineData() const { return engineData; }

    // ============================================================
    // Returns all data in a single aggregated object
    // ============================================================
    AircraftBuildData getBuildData() const
    {
        return {commonData, wingData, fuselageData, engineData};
    }

    // ============================================================
    // Builds the aircraft by reading from XML
    // Returns *this for chaining or use getBuildData() afterwards
    // ============================================================
    BuildAircraft &buildAircraft()
    {
        seaLevelDensity = Atmosphere::ISA::density(0.0);

        densityRatio = Atmosphere::ISA::densityRatio(settings.altitude);

        // --- COMMON DATA ---
        commonData
            .setNameOfAircraft(parser.getValue<std::string>("myXMLDataToAircraft/generalData/nameOfAircraft"))
            .setWTO(parser.getValue<double>("myXMLDataToAircraft/generalData/maximumTakeOffWeight"))
            .setNumberOfPax(parser.getValue<int>("myXMLDataToAircraft/generalData/numberOfPax"))
            .setPayloadWeight(parser.getValue<double>("myXMLDataToAircraft/generalData/payload"))
            .setCrewWeight(parser.getValue<double>("myXMLDataToAircraft/crewData/crewMass"))
            .setFuelWeight(parser.getValue<double>("myXMLDataToAircraft/generalData/maxFuelCapability"))
            .setDiveSpeed(parser.getValue<double>("myXMLDataToAircraft/generalData/diveSpeed"))
            .setTypeOfWing(stringToTypeOfWing(parser.getValue<std::string>("myXMLDataToAircraft/wingData/typeOfWing")))
            .setWeightMethodWing(stringToWeightMethod(parser.getValue<std::string>("myXMLDataToAircraft/wingData/wingMethodWeight")))
            .setNUltimateLoad(parser.getValue<double>("myXMLDataToAircraft/generalData/nUlt"))
            .setAdimAerodynamicCenter(parser.getValue<double>("myXMLDataToAircraft/wingData/adimAerodynamicCenter"))
            .setIsSweptWing(parser.getValue<bool>("myXMLDataToAircraft/wingData/isSweptWing"))
            .setWingPosition(stringToWingPosition(parser.getValue<std::string>("myXMLDataToAircraft/generalData/wingPosition")))
            .setKMainSparPosition(parser.getValue<double>("myXMLDataToAircraft/wingData/mainSparPositionWing"))
            .setKSecondarySparPosition(parser.getValue<double>("myXMLDataToAircraft/wingData/secondSparPositionWing"))
            .setMeanAirfoilSlopeWing(parser.getValue<double>("myXMLDataToAircraft/aerodynamicData/meanAirfoilWingSlope"))
            .setMeanAirfoilSlopeHorizontalTail(parser.getValue<double>("myXMLDataToAircraft/aerodynamicData/meanAirfoilHorizontalSlope"))
            .setMeanAirfoilVerticalSlope(parser.getValue<double>("myXMLDataToAircraft/aerodynamicData/meanAirfoilVerticalSlope"))
            .setWeightMethodFuselage(stringToWeightMethod(parser.getValue<std::string>("myXMLDataToAircraft/fuselageData/fuselageWeightMethod")))
            .setFrameMaterial(stringToMaterial(parser.getValue<std::string>("myXMLDataToAircraft/fuselageData/framesMaterial")))
            .setStringersMaterial(stringToMaterial(parser.getValue<std::string>("myXMLDataToAircraft/fuselageData/stringersMaterial")))
            .setSkinMaterial(stringToMaterial(parser.getValue<std::string>("myXMLDataToAircraft/fuselageData/skinMaterial")))
            .setUndercarriagePosition(stringToUndercarriagePosition(parser.getValue<std::string>("myXMLDataToAircraft/wingData/underCarriagePosition")))
            .setTypeOfComposite(stringToComposite(parser.getValue<std::string>("myXMLDataToAircraft/generalData/typeOfComposite")))
            .setTypeOfStabilizer(stringToTypeOfStabilizer(parser.getValue<std::string>("myXMLDataToAircraft/horizontalData/flagStabilizer")))
            .setTypeOfTail(stringToTypeOfTail(parser.getValue<std::string>("myXMLDataToAircraft/generalData/tailConfiguration")))
            .setLandingGearProperties(parser.getValue<bool>("myXMLDataToAircraft/generalData/isRetractableGear"))
            .setStrutLength(parser.getValue<double>("myXMLDataToAircraft/generalData/strutLength"))
            .setHasCanard(parser.getValue<bool>("myXMLDataToAircraft/canardData/hasCanard"))
            .setMeanAirfoilSlopeCanard(parser.getValue<double>("myXMLDataToAircraft/aerodynamicData/meanAirfoilCanardSlope"))
            .setHasEOIR(parser.getValue<bool>("myXMLDataToAircraft/eoirData/hasEoir"))
            .setHasBoom(parser.getValue<bool>("myXMLDataToAircraft/boomData/hasBoom"))
            .setNumbersOfBoom(parser.getValue<int>("myXMLDataToAircraft/boomData/boomNumbers"))
            .setControlSurfacesProperties(parser.getValue<double>("myXMLDataToAircraft/generalData/controlSurfaceParameter"))
            .setRangeCovered(parser.getValue<double>("myXMLDataToAircraft/generalData/rangeCovered"))
            .setAircraftCategory(stringToAircraftCategory(parser.getValue<std::string>("myXMLDataToAircraft/generalData/aircraftCategory")))
            .setAircraftEngineType(stringToAircraftEngineType(parser.getValue<std::string>("myXMLDataToAircraft/generalData/aircraftEngineType")))
            .setEnginePosition(stringToEnginePosition(parser.getValue<std::string>("myXMLDataToAircraft/engineData/engineConfiguration")))
            .setNumberOfBlades(parser.getValue<int>("myXMLDataToAircraft/engineData/numberOfBlades"))
            .setHasFurnishinghAndEquipment(parser.getValue<bool>("myXMLDataToAircraft/generalData/enableFeQ"))
            .setHasAirConditioningAndAntiIce(parser.getValue<bool>("myXMLDataToAircraft/generalData/enableACI"))
            .setHasAPU(parser.getValue<bool>("myXMLDataToAircraft/generalData/enableAPU"));

        // --- WING DATA ---
        wingData
            .setThicknessToRootChordRatioWing(parser.getValue<double>("myXMLDataToAircraft/wingData/thicknessToRootChordRatio"))
            .setNWingMountedEngines(parser.getValue<int>("myXMLDataToAircraft/wingData/numberOfWingMountedEngines"))
            .setWingKinkThicknessRatio(parser.getValue<double>("myXMLDataToAircraft/wingData/wingKinkThicknessRatio"))
            .setMaxOperatingEAS(settings.Mach * Atmosphere::ISA::speedOfSound(settings.altitude) * sqrt(densityRatio))
            .setWingStrutPosition(parser.getValue<double>("myXMLDataToAircraft/wingData/wingStrutPosition"))
            .setStrutToWingChordRatio(parser.getValue<double>("myXMLDataToAircraft/wingData/strutToWingChordRatio"))
            .setPercentageComposite(parser.getValue<double>("myXMLDataToAircraft/wingData/percentageCompositeWing"))
            .setIsBracedWing(parser.getValue<bool>("myXMLDataToAircraft/wingData/isBracedWing"))
            .setHasSpoilers(parser.getValue<bool>("myXMLDataToAircraft/wingData/spoilerFlag"))
            .setHasFowlerFlap(parser.getValue<bool>("myXMLDataToAircraft/wingData/isFowlerFlap"))
            .setIsEngineInstallatedOnWing(parser.getValue<bool>("myXMLDataToAircraft/wingData/isEngineInstallatedOnWing"))
            .setIsFuelTankInstallatedOnWing(parser.getValue<bool>("myXMLDataToAircraft/wingData/isisFuelTankInstallatedOnWing"))
            .setIsSimplifiedFlapSystem(parser.getValue<bool>("myXMLDataToAircraft/wingData/isSimplifiedFlapSystem"))
            .setIsCompositeWing(parser.getValue<bool>("myXMLDataToAircraft/wingData/isCompositeWing"))
            .setWTO(commonData.getWTO())
            .setNUltimateLoad(commonData.getNUltimateLoad())
            .setDiveSpeed(commonData.getDiveSpeed())
            .setAircraftCategory(commonData.getAircraftCategory())
            .setAircraftEngineType(commonData.getAircraftEngineType())
            .setUndercarriagePosition(commonData.getUndercarriagePosition())
            .setEnginePosition(commonData.getEnginePosition())
            .setTypeOfComposite(commonData.getTypeOfComposite())
            .setTypeOfStabilizer(commonData.getTypeOfStabilizer())
            .setTypeOfTail(commonData.getTypeOfTail())
            .setWingPosition(commonData.getWingPosition())
            .setIsSweptWing(commonData.getIsSweptWing())
            .setKMainSparPosition(commonData.getKMainSparPosition())
            .setKSecondarySparPosition(commonData.getKSecondarySparPosition())
            .setHasCanard(commonData.getHasCanard())
            .setMeanAirfoilSlopeCanard(commonData.getMeanAirfoilSlopeCanard())
            .setHasBoom(commonData.getHasBoom())
            .setHasEOIR(commonData.getHasEOIR())
            .setLandingGearProperties(commonData.getIsRetractableLandingGear())
            .setStrutLength(commonData.getStrutLength())
            .setControlSurfacesProperties(commonData.getFactorToControlSurface())
            .setNameOfAircraft(commonData.getNameOfAircraft());

        // --- FUSELAGE DATA ---
        fuselageData
            .setIsPressurized(parser.getValue<bool>("myXMLDataToAircraft/fuselageData/isPressurized"))
            .setIsCargo(parser.getValue<bool>("myXMLDataToAircraft/fuselageData/cargoFlag"))
            .setWTO(commonData.getWTO())
            .setNUltimateLoad(commonData.getNUltimateLoad())
            .setAdimAerodynamicCenter(commonData.getAdimAerodynamicCenter())
            .setDiveSpeed(commonData.getDiveSpeed())
            .setAircraftCategory(commonData.getAircraftCategory())
            .setAircraftEngineType(commonData.getAircraftEngineType())
            .setUndercarriagePosition(commonData.getUndercarriagePosition())
            .setEnginePosition(commonData.getEnginePosition())
            .setTypeOfComposite(commonData.getTypeOfComposite())
            .setTypeOfStabilizer(commonData.getTypeOfStabilizer())
            .setTypeOfTail(commonData.getTypeOfTail())
            .setWingPosition(commonData.getWingPosition())
            .setIsSweptWing(commonData.getIsSweptWing())
            .setKMainSparPosition(commonData.getKMainSparPosition())
            .setKSecondarySparPosition(commonData.getKSecondarySparPosition())
            .setHasCanard(commonData.getHasCanard())
            .setHasBoom(commonData.getHasBoom())
            .setHasEOIR(commonData.getHasEOIR())
            .setLandingGearProperties(commonData.getIsRetractableLandingGear())
            .setStrutLength(commonData.getStrutLength())
            .setControlSurfacesProperties(commonData.getFactorToControlSurface())
            .setFrameMaterial(commonData.getFrameMaterial())
            .setStringersMaterial(commonData.getStringersMaterial())
            .setSkinMaterial(commonData.getSkinMaterial())
            .setNameOfAircraft(commonData.getNameOfAircraft());

        // --- ENGINE DATA ---
        engineData
            .setNumberOfEngines(parser.getValue<int>("myXMLDataToAircraft/engineData/numberOfEngines"))
            .setThrustToPowerRatio(parser.getValue<double>("myXMLDataToAircraft/engineData/thrustToPowerRatio"))
            .setThrustLbf(parser.getValue<double>("myXMLDataToAircraft/engineData/thrust"))
            .setBSHP(parser.getValue<double>("myXMLDataToAircraft/engineData/power"))
            .setKPG(parser.getValue<double>("myXMLDataToAircraft/engineData/kPGFactor"))
            .setKTHR(parser.getValue<double>("myXMLDataToAircraft/engineData/kTHRFactor"))
            .setHasWaterInjectionSystem(parser.getValue<bool>("myXMLDataToAircraft/engineData/hasWaterInjectionSystem"))
            .setAircraftCategory(commonData.getAircraftCategory())
            .setAircraftEngineType(commonData.getAircraftEngineType())
            .setNameOfAircraft(commonData.getNameOfAircraft())
            .setNumberOfBlades(commonData.getNumberOfBlades());

        return *this;
    }
};
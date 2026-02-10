#pragma once

#include "BASEWEIGHTDATA.h"
#include "WINGWEIGHTDATA.h"
#include "FUSELAGEWEIGHTDATA.h"
#include "ENGINEWEIGHTDATA.h"
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
    BaseWeightData    commonData;
    WingWeightData    wingData;
    FuselageWeightData fuselageData;
    EngineWeightData  engineData;
};

class BuildAircraft
{

protected:
    BaseWeightData     commonData;
    WingWeightData     wingData;
    FuselageWeightData fuselageData;
    EngineWeightData   engineData;
    XMLUtil::XMLParser parser;
    VSP::AeroSettings settings;

private:
    std::string nameOfAircraft;
    std::string currentWorkDirectory = "";
    // Essendo static constexpr, esiste a compile-time e non dipende dall'ordine di inizializzazione, quindi sarà sempre disponibile nel costruttore.
    // Dato che std::string non è un tipo literal, cioè alloca memoria dinamicamente (sull'heap tramite new), si deve usare const char* per garantire che sia un'espressione costante a compile-time.
    static constexpr const char* folderNameWeightsSettings = "WeightsSettings"; 
    double seaLevelDensity = 0.0;
    double densityRatio    = 0.0;
    double maxOperatingEAS = 0.0;

public:
    BuildAircraft(std::string nameOfAircraft, VSP::AeroSettings settings)
        : nameOfAircraft(nameOfAircraft),
          settings(settings),
          currentWorkDirectory(std::filesystem::current_path().string()),
          parser(std::filesystem::current_path().string() + "/" + folderNameWeightsSettings + "/" + nameOfAircraft + "_weightsSettings.xml")
    {
    }

    // ============================================================
    // Getters to access individual data blocks after buildAircraft()
    // ============================================================
    BaseWeightData     getCommonData()    const { return commonData;    }
    WingWeightData     getWingData()      const { return wingData;      }
    FuselageWeightData getFuselageData()  const { return fuselageData;  }
    EngineWeightData   getEngineData()    const { return engineData;    }

    // ============================================================
    // Returns all data in a single aggregated object
    // ============================================================
    AircraftBuildData getBuildData() const
    {
        return { commonData, wingData, fuselageData, engineData };
    }

    // ============================================================
    // Builds the aircraft by reading from XML
    // Returns *this for chaining or use getBuildData() afterwards
    // ============================================================
    BuildAircraft& buildAircraft()
    {
        seaLevelDensity = Atmosphere::ISA::density(0.0);

        
        densityRatio = Atmosphere::ISA::densityRatio(settings.altitude);

        // --- COMMON DATA ---
        commonData
            .setWTO(parser.getValue<double>("myXMLDataToWeights/generalData/maximumTakeOffWeight"))
            .setNumberOfPax(parser.getValue<int>("myXMLDataToWeights/generalData/numberOfPax"))
            .setPayloadWeight(parser.getValue<double>("myXMLDataToWeights/generalData/payload"))
            .setCrewWeight(parser.getValue<double>("myXMLDataToWeights/crewData/crewMass"))
            .setFuelWeight(parser.getValue<double>("myXMLDataToWeights/generalData/maxFuelCapability"))
            .setDiveSpeed(parser.getValue<double>("myXMLDataToWeights/generalData/diveSpeed"))
            .setTypeOfWing(stringToTypeOfWing(parser.getValue<std::string>("myXMLDataToWeights/wingData/typeOfWing")))
            .setWeightMethodWing(stringToWeightMethod(parser.getValue<std::string>("myXMLDataToWeights/wingData/wingMethodWeight")))
            .setNUltimateLoad(parser.getValue<double>("myXMLDataToWeights/generalData/nUlt"))
            .setIsSweptWing(parser.getValue<bool>("myXMLDataToWeights/wingData/isSweptWing"))
            .setWingPosition(stringToWingPosition(parser.getValue<std::string>("myXMLDataToWeights/generalData/wingPosition")))
            .setKMainSparPosition(parser.getValue<double>("myXMLDataToWeights/wingData/mainSparPositionWing"))
            .setKSecondarySparPosition(parser.getValue<double>("myXMLDataToWeights/wingData/secondSparPositionWing"))
            .setWeightMethodFuselage(stringToWeightMethod(parser.getValue<std::string>("myXMLDataToWeights/fuselageData/fuselageWeightMethod")))
            .setFrameMaterial(stringToMaterial(parser.getValue<std::string>("myXMLDataToWeights/fuselageData/framesMaterial")))
            .setStringersMaterial(stringToMaterial(parser.getValue<std::string>("myXMLDataToWeights/fuselageData/stringersMaterial")))
            .setSkinMaterial(stringToMaterial(parser.getValue<std::string>("myXMLDataToWeights/fuselageData/skinMaterial")))
            .setUndercarriagePosition(stringToUndercarriagePosition(parser.getValue<std::string>("myXMLDataToWeights/wingData/underCarriagePosition")))
            .setTypeOfComposite(stringToComposite(parser.getValue<std::string>("myXMLDataToWeights/generalData/typeOfComposite")))
            .setTypeOfStabilizer(stringToTypeOfStabilizer(parser.getValue<std::string>("myXMLDataToWeights/horizontalData/flagStabilizer")))
            .setTypeOfTail(stringToTypeOfTail(parser.getValue<std::string>("myXMLDataToWeights/generalData/tailConfiguration")))
            .setLandingGearProperties(parser.getValue<bool>("myXMLDataToWeights/generalData/isRetractableGear"))
            .setStrutLength(parser.getValue<double>("myXMLDataToWeights/generalData/strutLength"))
            .setHasCanard(parser.getValue<bool>("myXMLDataToWeights/canardData/hasCanard"))
            .setHasEOIR(parser.getValue<bool>("myXMLDataToWeights/eoirData/hasEoir"))
            .setHasBoom(parser.getValue<bool>("myXMLDataToWeights/boomData/hasBoom"))
            .setNumbersOfBoom(parser.getValue<int>("myXMLDataToWeights/boomData/boomNumbers"))
            .setControlSurfacesProperties(parser.getValue<double>("myXMLDataToWeights/generalData/controlSurfaceParameter"))
            .setRangeCovered(parser.getValue<double>("myXMLDataToWeights/generalData/rangeCovered"))
            .setAircraftCategory(stringToAircraftCategory(parser.getValue<std::string>("myXMLDataToWeights/generalData/aircraftCategory")))
            .setAircraftEngineType(stringToAircraftEngineType(parser.getValue<std::string>("myXMLDataToWeights/generalData/aircraftEngineType")))
            .setEnginePosition(stringToEnginePosition(parser.getValue<std::string>("myXMLDataToWeights/engineData/engineConfiguration")))
            .setHasFurnishinghAndEquipment(parser.getValue<bool>("myXMLDataToWeights/generalData/enableFeQ"))
            .setHasAirConditioningAndAntiIce(parser.getValue<bool>("myXMLDataToWeights/generalData/enableACI"))
            .setHasAPU(parser.getValue<bool>("myXMLDataToWeights/generalData/enableAPU"));

        // --- WING DATA ---
        wingData
            .setThicknessToRootChordRatioWing(parser.getValue<double>("myXMLDataToWeights/wingData/thicknessToRootChordRatio"))
            .setNWingMountedEngines(parser.getValue<int>("myXMLDataToWeights/wingData/numberOfWingMountedEngines"))
            .setWingKinkThicknessRatio(parser.getValue<double>("myXMLDataToWeights/wingData/wingKinkThicknessRatio"))
            .setMaxOperatingEAS(settings.Mach * Atmosphere::ISA::speedOfSound(settings.altitude) * sqrt(densityRatio))
            .setWingStrutPosition(parser.getValue<double>("myXMLDataToWeights/wingData/wingStrutPosition"))
            .setStrutToWingChordRatio(parser.getValue<double>("myXMLDataToWeights/wingData/strutToWingChordRatio"))
            .setPercentageComposite(parser.getValue<double>("myXMLDataToWeights/wingData/percentageCompositeWing"))
            .setIsBracedWing(parser.getValue<bool>("myXMLDataToWeights/wingData/isBracedWing"))
            .setHasSpoilers(parser.getValue<bool>("myXMLDataToWeights/wingData/spoilerFlag"))
            .setHasFowlerFlap(parser.getValue<bool>("myXMLDataToWeights/wingData/isFowlerFlap"))
            .setIsEngineInstallatedOnWing(parser.getValue<bool>("myXMLDataToWeights/wingData/isEngineInstallatedOnWing"))
            .setIsFuelTankInstallatedOnWing(parser.getValue<bool>("myXMLDataToWeights/wingData/isisFuelTankInstallatedOnWing"))
            .setIsSimplifiedFlapSystem(parser.getValue<bool>("myXMLDataToWeights/wingData/isSimplifiedFlapSystem"))
            .setIsCompositeWing(parser.getValue<bool>("myXMLDataToWeights/wingData/isCompositeWing"))
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
            .setHasBoom(commonData.getHasBoom())
            .setHasEOIR(commonData.getHasEOIR())
            .setLandingGearProperties(commonData.getIsRetractableLandingGear())
            .setStrutLength(commonData.getStrutLength())
            .setControlSurfacesProperties(commonData.getFactorToControlSurface());

            // --- FUSELAGE DATA ---
            fuselageData
            .setIsPressurized(parser.getValue<bool>("myXMLDataToWeights/fuselageData/isPressurized"))
            .setIsCargo(parser.getValue<bool>("myXMLDataToWeights/fuselageData/cargoFlag"))
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
            .setHasBoom(commonData.getHasBoom())
            .setHasEOIR(commonData.getHasEOIR())
            .setLandingGearProperties(commonData.getIsRetractableLandingGear())
            .setStrutLength(commonData.getStrutLength())
            .setControlSurfacesProperties(commonData.getFactorToControlSurface())
            .setFrameMaterial(commonData.getFrameMaterial())
            .setStringersMaterial(commonData.getStringersMaterial())
            .setSkinMaterial(commonData.getSkinMaterial());


        // --- ENGINE DATA ---
        engineData
            .setNumberOfEngines(parser.getValue<int>("myXMLDataToWeights/engineData/numberOfEngines"))
            .setThrustToPowerRatio(parser.getValue<double>("myXMLDataToWeights/engineData/thrustToPowerRatio"))
            .setThrustLbf(parser.getValue<double>("myXMLDataToWeights/engineData/thrust"))
            .setBSHP(parser.getValue<double>("myXMLDataToWeights/engineData/power"))
            .setKPG(parser.getValue<double>("myXMLDataToWeights/engineData/kPGFactor"))
            .setKTHR(parser.getValue<double>("myXMLDataToWeights/engineData/kTHRFactor"))
            .setHasWaterInjectionSystem(parser.getValue<bool>("myXMLDataToWeights/engineData/hasWaterInjectionSystem"))
            .setAircraftCategory(commonData.getAircraftCategory())
            .setAircraftEngineType(commonData.getAircraftEngineType());

        return *this;
    }
};
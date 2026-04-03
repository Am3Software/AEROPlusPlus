#pragma once

#include "BASEAIRCRAFTDATA.h"
#include "WINGBASEDATA.h"
#include "FUSELAGEBASEDATA.h"
#include "ENGINEBASEDATA.h"
#include "LANDINGGEARSBASEDATA.h"
#include "WEIGHTSCORRECTIONFACTORS.h"
#include "COGCORRECTIONFACTORS.h"
#include "OSWALDFACTORCORRECTIONFACTOR.h"
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
    LandingGearsBaseData landingGearsData;
};

class BuildAircraft
{

protected:
    BaseAircraftData commonData;
    WingBaseData wingData;
    FuselageBaseData fuselageData;
    EngineBaseData engineData;
    LandingGearsBaseData landingGearsData;
    WeightCorrectionFactor weightsCorrectionFactors;
    COGCorrectionFactor cogCorrectionFactors;
    OswaldCorrectionFactor oswaldCorrectionFactors;

    XMLUtil::XMLParser parser;
    VSP::AeroSettings settings;

private:
    std::string nameOfAircraft;
    std::string currentWorkDirectory = "";
    // Essendo static constexpr, esiste a compile-time e non dipende dall'ordine di inizializzazione, quindi sarà sempre disponibile nel costruttore.
    // Dato che std::string non è un tipo literal, cioè alloca memoria dinamicamente (sull'heap tramite new), si deve usare const char* per garantire che sia un'espressione costante a compile-time.
    static constexpr const char *folderNameAircraftSettings = "AircraftSettings";
    double seaLevelDensity = 0.0;
    double densityRatio = 0.0;
    double maxOperatingEAS = 0.0;

public:
    BuildAircraft(std::string nameOfAircraft, VSP::AeroSettings settings)
        : nameOfAircraft(nameOfAircraft),
          settings(settings),
          currentWorkDirectory(std::filesystem::current_path().string()),
          parser(std::filesystem::current_path().string() + "/" + folderNameAircraftSettings + "/" + nameOfAircraft + "_aircraftSettings.xml")
    {
    }

    // ============================================================
    // Getters to access individual data blocks after buildAircraft()
    // ============================================================
    BaseAircraftData getCommonData() const { return commonData; }
    WingBaseData getWingData() const { return wingData; }
    FuselageBaseData getFuselageData() const { return fuselageData; }
    EngineBaseData getEngineData() const { return engineData; }
    LandingGearsBaseData getLandingGearsData() const { return landingGearsData; }
    WeightCorrectionFactor getWeightCorrectionFactor() const { return weightsCorrectionFactors; }
    COGCorrectionFactor getCOGCorrectionFactor() const { return cogCorrectionFactors; }
    OswaldCorrectionFactor getOswaldCorrectionFactor() const { return oswaldCorrectionFactors; }
    // ============================================================
    // Returns all data in a single aggregated object
    // ============================================================
    AircraftBuildData getBuildData() const
    {
        return {commonData, wingData, fuselageData, engineData, landingGearsData};
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
            .setNumberOfCrewMembers(parser.getValue<int>("myXMLDataToAircraft/crewData/crewNumber"))
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
            .setHasCanard(parser.getValue<bool>("myXMLDataToAircraft/canardData/hasCanard"))
            .setMeanAirfoilSlopeCanard(parser.getValue<double>("myXMLDataToAircraft/aerodynamicData/meanAirfoilCanardSlope"))
            .setHasEOIR(parser.getValue<bool>("myXMLDataToAircraft/eoirData/hasEoir"))
            .setHasBoom(parser.getValue<bool>("myXMLDataToAircraft/boomData/hasBoom"))
            .setNumbersOfBoom(parser.getValue<int>("myXMLDataToAircraft/boomData/boomNumbers"))
            .setControlSurfacesProperties(parser.getValue<double>("myXMLDataToAircraft/generalData/controlSurfaceParameter"))
            .setRangeCovered(parser.getValue<double>("myXMLDataToAircraft/generalData/rangeCovered"))
            .setAircraftCategory(stringToAircraftCategory(parser.getValue<std::string>("myXMLDataToAircraft/generalData/aircraftCategory")))
            .setAircraftEngineType(stringToAircraftEngineType(parser.getValue<std::string>("myXMLDataToAircraft/engineData/aircraftEngineType")))
            .setAircraftEngineCategory(stringToAircraftEngineCategory(parser.getValue<std::string>("myXMLDataToAircraft/generalData/aircraftEngineCategory")))
            .setEnginePosition(stringToEnginePosition(parser.getValue<std::string>("myXMLDataToAircraft/engineData/engineConfiguration")))
            .setNumberOfBlades(parser.getValue<int>("myXMLDataToAircraft/engineData/numberOfBlades"))
            .setHasFurnishinghAndEquipment(parser.getValue<bool>("myXMLDataToAircraft/generalData/enableFeQ"))
            .setHasAirConditioningAndAntiIce(parser.getValue<bool>("myXMLDataToAircraft/generalData/enableACI"))
            .setHasAPU(parser.getValue<bool>("myXMLDataToAircraft/generalData/enableAPU"))
            .setSkinRoughnessTypeWing(stringToSkinRoughnessType(parser.getValue<std::string>("myXMLDataToAircraft/wingData/skinRoughnessTypeWing")))
            .setSkinRoughnessTypeCanard(stringToSkinRoughnessType(parser.getValue<std::string>("myXMLDataToAircraft/canardData/skinRoughnessTypeCanard")))
            .setSkinRoughnessTypeHorizontalTail(stringToSkinRoughnessType(parser.getValue<std::string>("myXMLDataToAircraft/horizontalData/skinRoughnessTypeHorizontal")))
            .setSkinRoughnessTypeVerticalTail(stringToSkinRoughnessType(parser.getValue<std::string>("myXMLDataToAircraft/verticalData/skinRoughnessTypeVertical")))
            .setSkinRoughnessTypeFuselage(stringToSkinRoughnessType(parser.getValue<std::string>("myXMLDataToAircraft/fuselageData/skinRoughnessTypeFuselage")))
            .setSkinRoughnessTypeNacelle(stringToSkinRoughnessType(parser.getValue<std::string>("myXMLDataToAircraft/engineData/skinRoughnessTypeNacelle")))
            .setSkinRoughnessTypeBoom(stringToSkinRoughnessType(parser.getValue<std::string>("myXMLDataToAircraft/boomData/skinRoughnessTypeBoom")))
            .setCanardPosition(stringToWingPosition(parser.getValue<std::string>("myXMLDataToAircraft/canardData/canardPosition")))
            .setWindScreenType(stringToWindScreenType(parser.getValue<std::string>("myXMLDataToAircraft/fuselageData/windScreenType")));

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
            .setMaximumFalpDeflection(parser.getValue<double>("myXMLDataToAircraft/wingData/maximumFlapDeflection"))
            .setMaximumCanardFlapDeflection(parser.getValue<double>("myXMLDataToAircraft/canardData/maximumCanardFlapDefelction"))
            .setMaximumElevatorDeflection(parser.getValue<double>("myXMLDataToAircraft/horizontalData/maximumElevatorDeflection"))
            .setMaximumRudderDeflection(parser.getValue<double>("myXMLDataToAircraft/verticalData/maximumRudderDeflection"))
            .setHasSpoilers(parser.getValue<bool>("myXMLDataToAircraft/wingData/spoilerFlag"))
            .setHasFowlerFlap(parser.getValue<bool>("myXMLDataToAircraft/wingData/isFowlerFlap"))
            .setIsEngineInstallatedOnWing(parser.getValue<bool>("myXMLDataToAircraft/wingData/isEngineInstallatedOnWing"))
            .setIsFuelTankInstallatedOnWing(parser.getValue<bool>("myXMLDataToAircraft/wingData/isisFuelTankInstallatedOnWing"))
            .setIsSimplifiedFlapSystem(parser.getValue<bool>("myXMLDataToAircraft/wingData/isSimplifiedFlapSystem"))
            .setIsCompositeWing(parser.getValue<bool>("myXMLDataToAircraft/wingData/isCompositeWing"))
            .setWingFairingType(stringToWingFairingType(parser.getValue<std::string>("myXMLDataToAircraft/wingData/wingFairingType")))
            .setTypeOfFlap(stringToTypeOfFlap(parser.getValue<std::string>("myXMLDataToAircraft/wingData/typeOfFlap")))
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
            .setCriticalAltitude(parser.getValue<double>("myXMLDataToAircraft/engineData/criticalAltitude"))
            .setSFC(parser.getValue<double>("myXMLDataToAircraft/engineData/sfc"))
            .setSFCJ(parser.getValue<double>("myXMLDataToAircraft/engineData/sfcj"))
            .setTypeOfInlet(stringToTypeOfInlet(parser.getValue<std::string>("myXMLDataToAircraft/engineData/typeOfInlet")))
            .setAircraftCategory(commonData.getAircraftCategory())
            .setAircraftEngineType(commonData.getAircraftEngineType())
            .setNameOfAircraft(commonData.getNameOfAircraft())
            .setNumberOfBlades(commonData.getNumberOfBlades());

        // --- LANDING GEARS DATA ---
        landingGearsData
            .setIsRetractableLandingGear(parser.getValue<bool>("myXMLDataToAircraft/landingGearData/isRetractableGear"))
            .setStrutMainWidth(parser.getValue<double>("myXMLDataToAircraft/landingGearData/strutMainWidth"))
            .setStrutMainDiameter(parser.getValue<double>("myXMLDataToAircraft/landingGearData/strutMainDiameter"))
            .setStrutNoseWidth(parser.getValue<double>("myXMLDataToAircraft/landingGearData/strutNoseWidth"))
            .setStrutNoseDiameter(parser.getValue<double>("myXMLDataToAircraft/landingGearData/strutNoseDiameter"))
            .setNoseWheelDiameter(parser.getValue<double>("myXMLDataToAircraft/landingGearData/noseWheelDiamter"))
            .setNoseWheelWidth(parser.getValue<double>("myXMLDataToAircraft/landingGearData/noseWheelWidth"))
            .setMainWheelDiameter(parser.getValue<double>("myXMLDataToAircraft/landingGearData/mainWheelDiamter"))
            .setMainWheelWidth(parser.getValue<double>("myXMLDataToAircraft/landingGearData/mainWheelWidth"))
            .setNumberOfNoseWheels(parser.getValue<int>("myXMLDataToAircraft/landingGearData/numberOfNoseWheel"))
            .setNumberOfMainWheels(parser.getValue<int>("myXMLDataToAircraft/landingGearData/numberOfMainWheel"))
            .setNumberOfNoseStruts(parser.getValue<int>("myXMLDataToAircraft/landingGearData/numberOfNoseStrut"))
            .setNumberOfMainStruts(parser.getValue<int>("myXMLDataToAircraft/landingGearData/numberOfMainStrut"))
            .setStrutLengthMain(parser.getValue<double>("myXMLDataToAircraft/landingGearData/strutLengthMain"))
            .setStrutLengthNose(parser.getValue<double>("myXMLDataToAircraft/landingGearData/strutLengthNose"));

        // --- WEIGHT CORRECTION FACTORS ---
        weightsCorrectionFactors
            .setKWing(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kWing"))
            .setKCanard(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kCanard"))
            .setKHorizontal(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kHorizontal"))
            .setKVertical(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kVertical"))
            .setKVentralFin(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kVentralFin"))
            .setKFuselage(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kFuselage"))
            .setKBoom(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kBoom"))
            .setKEoir(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kEoir"))
            .setKUnderCarriage(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kUnderCarriage"))
            .setKAPU(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kAPU"))
            .setKControlSurfaces(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kControlSurfaces"))
            .setKPropulsionGroup(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kPropulsionGroup"))
            .setKInstruments(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kInstruments"))
            .setKHydraulicAndPneumatic(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kHydraulicAndPenumatic"))
            .setKElectricalGroup(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kElectricalGroup"))
            .setKAvionic(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kAvionic"))
            .setKFurnishingAndEquipment(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kFurnishingAndEquipment"))
            .setKAntiIcingAndConditioning(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kAntiIcingAndConditioning"))
            .setKOperatingItems(parser.getValue<double>("myXMLDataToAircraft/weightCalibrationFactor/kOperatingItems"));

        auto kWingCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kWing");
        auto kCanardCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kCanard");
        auto kHorizontalCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kHorizontal");
        auto kVerticalCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kVertical");
        auto kVentralFinCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kVentralFin");
        auto kFuselageCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kFuselage");
        auto kBoomCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kBoom");
        auto kEOIRCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kEoir");
        auto kUnderCarriageCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kUnderCarriage");
        auto kAPUCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kAPU");
        auto kControlSurfacesCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kControlSurfaces");
        auto kPropulsionGroupCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kPropulsionGroup");
        auto kInstrumentsCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kInstruments");
        auto kHydraulicAndPneumaticCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kHydraulicAndPenumatic");
        auto kElectricalGroupCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kElectricalGroup");
        auto kAvionicCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kAvionic");
        auto kFurnishingAndEquipmentCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kFurnishingAndEquipment");
        auto kAntiIcingAndConditioningCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kAntiIcingAndConditioning");
        auto kOperatingItemsCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kOperatingItems");
        auto kCrewCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kCrew");
        auto kPayloadCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kPayload");
        auto kFuelCog = parser.getValue<std::vector<double>>("myXMLDataToAircraft/cogCalibrationFactor/kFuel");

        cogCorrectionFactors
            .setKXWing(kWingCog[0])
            .setKZWing(kWingCog[1])
            .setKXCanard(kCanardCog[0])
            .setKZCanard(kCanardCog[1])
            .setKXHorizontal(kHorizontalCog[0])
            .setKZHorizontal(kHorizontalCog[1])
            .setKXVertical(kVerticalCog[0])
            .setKZVertical(kVerticalCog[1])
            .setKXVentralFin(kVentralFinCog[0])
            .setKZVentralFin(kVentralFinCog[1])
            .setKXFuselage(kFuselageCog[0])
            .setKZFuselage(kFuselageCog[1])
            .setKXBoom(kBoomCog[0])
            .setKZBoom(kBoomCog[1])
            .setKXEoir(kEOIRCog[0])
            .setKZEoir(kEOIRCog[1])
            .setKXUnderCarriage(kUnderCarriageCog[0])
            .setKZUnderCarriage(kUnderCarriageCog[1])
            .setKXAPU(kAPUCog[0])
            .setKZAPU(kAPUCog[1])
            .setKXControlSurfaces(kControlSurfacesCog[0]) // ← mancava [0]
            .setKZControlSurfaces(kControlSurfacesCog[1]) // ← mancava [1]
            .setKXPropulsionGroup(kPropulsionGroupCog[0]) // ← vettore
            .setKZPropulsionGroup(kPropulsionGroupCog[1])
            .setKXInstruments(kInstrumentsCog[0])
            .setKZInstruments(kInstrumentsCog[1])
            .setKXHydraulicAndPneumatic(kHydraulicAndPneumaticCog[0])
            .setKZHydraulicAndPneumatic(kHydraulicAndPneumaticCog[1])
            .setKXElectricalGroup(kElectricalGroupCog[0])
            .setKZElectricalGroup(kElectricalGroupCog[1])
            .setKXAvionic(kAvionicCog[0])
            .setKZAvionic(kAvionicCog[1])
            .setKXFurnishingAndEquipment(kFurnishingAndEquipmentCog[0])
            .setKZFurnishingAndEquipment(kFurnishingAndEquipmentCog[1])
            .setKXAntiIcingAndConditioning(kAntiIcingAndConditioningCog[0])
            .setKZAntiIcingAndConditioning(kAntiIcingAndConditioningCog[1])
            .setKXOperatingItems(kOperatingItemsCog[0])
            .setKZOperatingItems(kOperatingItemsCog[1])
            .setKXCrew(kCrewCog[0])
            .setKZCrew(kCrewCog[1])
            .setKXPayload(kPayloadCog[0])
            .setKZPayload(kPayloadCog[1])
            .setKXFuel(kFuelCog[0])
            .setKZFuel(kFuelCog[1]);


            // --- OSWALD FACTOR CORRECTION FACTORS ---

            oswaldCorrectionFactors
            .setKOswaldTakeOff(parser.getValue<double>("myXMLDataToAircraft/oswaldCoeffCalibrationFactor/keTO"))
            .setKOswaldClimb(parser.getValue<double>("myXMLDataToAircraft/oswaldCoeffCalibrationFactor/keClimb"))
            .setKOswaldDescent(parser.getValue<double>("myXMLDataToAircraft/oswaldCoeffCalibrationFactor/keDesc"))
            .setKOswaldLanding(parser.getValue<double>("myXMLDataToAircraft/oswaldCoeffCalibrationFactor/keLan"));



        return *this;
    }
};
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include "Interpolant.h"
#include "VSPScriptGenerator.h"
#include "WETTEDAREA.h"
#include "WEIGHTS.h"
#include "BASEWEIGHTDATA.h"
#include "BUILDAIRCRAFT.h"
#include "WingWeightData.h"
#include "FUSELAGEWEIGHTDATA.h"
#include "ENGINEWEIGHTDATA.h"
#include "SforzaSweepConversion.h"
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

namespace COG
{
    struct COGDATA
    {

        // Center of Gravity coordinates
        double xCG = 0.0;
        double yCG = 0.0;
        double zCG = 0.0;

        // Correction factors
        double kXCG = 1.0;
        double kYCG = 1.0;
        double kZCG = 1.0;
    };

    struct Weights
    {

        // Weights of the components
        double wingWeight = 0.0;
        double canardWeight = 0.0;
        double horizontalWeight = 0.0;
        double verticalWeight = 0.0;
        double fuselageWeight = 0.0;
        double nacelleWeight = 0.0;
        double boomWeight = 0.0;
        double eoirWeight = 0.0;
        double landingGearWeight = 0.0;
        double controlSurfacesWeight = 0.0;
        double propulsionGroupWeight = 0.0;
        double apuWeight = 0.0;
        double instrumentsWeight = 0.0;
        double hydraulicAndPneumaticWeight = 0.0;
        double electricalGroupWeight = 0.0;
        double avionicGroupWeight = 0.0;
        double furnishingsAndEquipmentWeight = 0.0;
        double airConditioningAndAntiIceWeight = 0.0;
        double operatingItemsWeight = 0.0;
        double payloadWeight = 0.0;
        double crewWeight = 0.0;
        double fuelWeight = 0.0;
        double totalAircraftWeight = 0.0;
    };

    class COGCalculator
    {

    private:
        Weight weight;
        BaseWeightData builderData;
        WingWeightData wingData;
        FuselageWeightData fuselageData;
        EngineWeightData engineData;
        WETTEDAREA::WettedArea wettedAreaCalculator;
        COG::Weights weights;
        COG::COGDATA centerOfGravityData;

        std::string nameOfAircraft = "";

        // Weights of the components
        double wingWeight = 0.0;
        double canardWeight = 0.0;
        double horizontalWeight = 0.0;
        double verticalWeight = 0.0;
        double fuselageWeight = 0.0;
        double nacelleWeight = 0.0;
        double boomWeight = 0.0;
        double eoirWeight = 0.0;
        double landingGearWeight = 0.0;
        double noseLandingGearWeight = 0.0;
        double mainLandingGearWeight = 0.0;
        double controlSurfacesWeight = 0.0;
        double propulsionGroupWeight = 0.0;
        double apuWeight = 0.0;
        double instrumentsWeight = 0.0;
        double hydraulicAndPneumaticWeight = 0.0;
        double electricalGroupWeight = 0.0;
        double avionicGroupWeight = 0.0;
        double furnishingsAndEquipmentWeight = 0.0;
        double airConditioningAndAntiIceWeight = 0.0;
        double operatingItemsWeight = 0.0;
        double payloadWeight = 0.0;
        double crewWeight = 0.0;
        double fuelWeight = 0.0;
        double totalAircraftWeight = 0.0;

        // Wing variables related to COG calculation
        double xCGWing = 0.0;
        double yCGWing = 0.0;
        double zCGWing = 0.0;

        // Canard variables related to COG calculation
        double xCGCanard = 0.0;
        double yCGCanard = 0.0;
        double zCGCanard = 0.0;

        // Horizontal tail variables related to COG calculation
        double xCGHorizontal = 0.0;
        double yCGHorizontal = 0.0;
        double zCGHorizontal = 0.0;

        // Vertical tail variables related to COG calculation
        double xCGVertical = 0.0;
        double yCGVertical = 0.0;
        double zCGVertical = 0.0;

        // Fuselage variables related to COG calculation
        double xCGFuselage = 0.0;
        double yCGFuselage = 0.0;
        double zCGFuselage = 0.0;

        // Nacelle variables related to COG calculation
        double xCGNacelle = 0.0;
        double yCGNacelle = 0.0;
        double zCGNacelle = 0.0;

        // Boom variables related to COG calculation
        double xCGBoom = 0.0;
        double yCGBoom = 0.0;
        double zCGBoom = 0.0;

        // EOIR variables related to COG calculation
        double xCGEOIR = 0.0;
        double yCGEOIR = 0.0;
        double zCGEOIR = 0.0;

        // Landing gear variables related to COG calculation
        double xCGLandingGear = 0.0;
        double yCGLandingGear = 0.0;
        double zCGLandingGear = 0.0;

        // Control surfaces variables related to COG calculation
        double xCGControlSurfaces = 0.0;
        double yCGControlSurfaces = 0.0;
        double zCGControlSurfaces = 0.0;

        // Propulsion group variables related to COG calculation
        double xCGPropulsionGroup = 0.0;
        double yCGPropulsionGroup = 0.0;
        double zCGPropulsionGroup = 0.0;

        // APU variables related to COG calculation
        double xCGAPU = 0.0;
        double yCGAPU = 0.0;
        double zCGAPU = 0.0;

        // Instruments variables related to COG calculation
        double xCGInstruments = 0.0;
        double yCGInstruments = 0.0;
        double zCGInstruments = 0.0;

        // Hydraulic and pneumatic systems variables related to COG calculation
        double xCGHydraulicAndPneumatic = 0.0;
        double yCGHydraulicAndPneumatic = 0.0;
        double zCGHydraulicAndPneumatic = 0.0;

        // Electrical group variables related to COG calculation
        double xCGElectricalGroup = 0.0;
        double yCGElectricalGroup = 0.0;
        double zCGElectricalGroup = 0.0;

        // Avionic group variables related to COG calculation
        double xCGAvionicGroup = 0.0;
        double yCGAvionicGroup = 0.0;
        double zCGAvionicGroup = 0.0;

        // Furnishings and equipment variables related to COG calculation
        double xCGFurnishingsAndEquipment = 0.0;
        double yCGFurnishingsAndEquipment = 0.0;
        double zCGFurnishingsAndEquipment = 0.0;

        // Air conditioning and anti-ice variables related to COG calculation
        double xCGAirConditioningAndAntiIce = 0.0;
        double yCGAirConditioningAndAntiIce = 0.0;
        double zCGAirConditioningAndAntiIce = 0.0;

        // Operating items variables related to COG calculation
        double xCGOperatingItems = 0.0;
        double yCGOperatingItems = 0.0;
        double zCGOperatingItems = 0.0;

        // Payload variables related to COG calculation
        double xCGPayload = 0.0;
        double yCGPayload = 0.0;
        double zCGPayload = 0.0;

        // Crew variables related to COG calculation
        double xCGCrew = 0.0;
        double yCGCrew = 0.0;
        double zCGCrew = 0.0;

        // Fuel variables related to COG calculation
        double xCGFuel = 0.0;
        double yCGFuel = 0.0;
        double zCGFuel = 0.0;

        // Total aircraft variables related to COG calculation
        double xCGTotalAircraft = 0.0;
        double yCGTotalAircraft = 0.0;
        double zCGTotalAircraft = 0.0;

        // Sweep angle
        double sweepC2 = 0.0;
        double sweepC4 = 0.0;

        // Roskam range xCG wing parcent of the chord
        double minmumXCGWingChord = 0.38;
        double maximumXCGWingChord = 0.42;

        // Factors to main and secondary spar positions
        double mainSparPositionWing = 0.0;
        double secondarySparPositionWing = 0.0;
        double kMainSparPosition = 0.0;
        double kSecondarySparPosition = 0.0;

        // Constant factors
        double factorSpanHorizontal = 0.25;
        double originalRootChordVer = 0.0;
        std::vector<double> possibleZLocOfTheHorizontalTail = {0.0, 0.0};
        std::vector<double> ratioSpanTovertical = {0.38, 0.55};
        double minimumPercentageOfXCGLocationFuselage = 0.42;                  // Percetage of the fuselage length where the xCG is located in the most forward configuration (Roskam)
        double maximumPercentageOfXCGLocationFuselage = 0.45;                  // Percetage of the fuselage length where the xCG is located in the most aft configuration (Roskam)
        double percentageOfXCGLocationFuselageNoWingMountedLandingGear = 0.47; // Percentage of the fuselage length where the xCG is located in the current configuration (Roskam)
        double minimumPercentageOfXCGLocationBoom = 0.4;                       // Percetage of the boom length where the xCG is located in the most forward configuration (Roskam)
        double maximumPercentageOfXCGLocationBoom = 0.45;                      // Percetage of the boom length where the xCG is located in the most aft configuration (Roskam)
        double percentageOfXCGLocationNoseLandingGear = 0.17;                  // Percetage of the fuselage length where the xCG is located for the nose landing gear
        double percentageOfXCGLocationMainLandingGear = 0.55;                  // Percetage of the fuselage length where the xCG is located for the main landing gear
        double percentageOfXCGLocationNoseLandingGearNoWingMounted = 0.14;     // Percetage of the fuselage length where the xCG is located for the nose landing gear
        double percentageOfXCGLocationMainLandingGearNoWingMounted = 0.56;     // Percetage of the fuselage length where the xCG is located for the main landing gear
        double percentageOfXCGLocationControlSurfaces = 0.6;                   // Percetage of the fuselage length where the xCG is located for the control surfaces (wing mounted)
        double percentageOfXCGLocationControlSurfacesNoWingMounted = 0.75;     // Percetage of the fuselage length where the xCG is located for the control surfaces
        double percentageOfXCGLocationNacelle = 0.45;                          // Percetage of the nacelle length where the xCG is located
        double percentageOfXCGLocationAPU = 0.95;                              // Percetage of the fuselage length where the xCG is located
        double percentageOfXCGLocationInstruments = 0.14;                      // Percetage of the fuselage length where the xCG is located for the instruments
        double percentageOfZCGLocationInstruments = 0.3;                       // Percetage of the fuselage diameter where the zCG is located for the instruments
        double percentageOfXCGLocationHydraulicAndPneumatic = 0.55;            // Percetage of the fuselage length where the xCG is located for the hydraulic and pneumatic systems
        double percentageOfXCGLocationElectricalGroup = 0.30;                  // Percetage of the fuselage length where the xCG is located for the electrical group
        double percentageOfXCGLocationAvionicGroup = 0.12;                     // Percetage of the fuselage length where the xCG is located for the avionic group
        double percentageOfXCGLocationFurnishingsAndEquipment = 0.5;           // Percetage of the fuselage length where the xCG is located for the furnishings and equipment
        double percentageOfXCGLocationAirConditioningAndAntiIce = 0.5;         // Percetage of the fuselage length where the xCG is located for the air conditioning and anti-ice systems
        double percentageOfZCGLocationAirConditioningAndAntiIce = -0.25;       // Percetage of the fuselage diameter where the zCG is located for the air conditioning and anti-ice systems
        double percentageOfXCGLocationOperatingItems = 0.45;                   // Percetage of the fuselage length where the xCG is located for the operating items
        double percentageOfXCGLocationPayload = 0.5;                           // Percetage of the fuselage length where the xCG is located for the payload
        double percentageOfXCGLocationCrew = 0.14;                             // Percetage of the fuselage length where the xCG is located for the crew
        double percentageOfMACLocationFuel = 0.5;                              // Percetage of the MAC length where the xCG is located for the fuel
        double postionMainSparWing = 0.0;
        double positionSecondarySparWing = 0.0;

        TypeOfComposite typeOfComposite = TypeOfComposite::UNKNOWN;
        AircraftCategory aircraftCategory = AircraftCategory::UNKNOWN;
        AircraftEngineType aircraftEngineType = AircraftEngineType::UNKNOWN;
        TypeOfStabilizer typeOfHorizontalStabilizer = TypeOfStabilizer::UNKNOWN;
        TypeOfTail typeOFTail = TypeOfTail::UNKNOWN;

        VSP::Wing wing;
        VSP::Wing canard;
        VSP::Wing horizontalTail;
        VSP::Wing verticalTail;
        VSP::Fuselage fuselage;
        VSP::Nacelle nacelle;
        VSP::Boom boom;
        VSP::Pod pod;
        VSP::EOIR eoir;

    public:
        COGCalculator(std::string nameOfAircraft,
                      BaseWeightData builderData,
                      WingWeightData wingData,
                      FuselageWeightData fuselageData,
                      EngineWeightData engineData,
                      VSP::Wing wing,
                      VSP::Wing horizontalTail,
                      VSP::Wing verticalTail,
                      VSP::Fuselage fuselage,
                      VSP::Nacelle nacelle,
                      VSP::Wing canard = VSP::Wing{},
                      VSP::Boom boom = VSP::Boom{},
                      VSP::Pod pod = VSP::Pod{},
                      VSP::EOIR eoir = VSP::EOIR{})
            : nameOfAircraft(nameOfAircraft),
              builderData(builderData),
              wingData(wingData),
              fuselageData(fuselageData),
              engineData(engineData),
              wing(wing),
              horizontalTail(horizontalTail),
              verticalTail(verticalTail),
              fuselage(fuselage),
              nacelle(nacelle),
              canard(canard),
              boom(boom),
              pod(pod),
              eoir(eoir),
              wettedAreaCalculator("GetGeomOfAircraft.vspscript")
        {
        }

        // Disrtuctor
        ~COGCalculator() = default;

        // ======================= COG functions =======================

        // WING COG
        std::tuple<double, double, double> inline calculateCOGWing()
        {

            if (builderData.getIsSweptWing())
            {
                xCGWing = (wing.xloc + 0.5 * (minmumXCGWingChord + maximumXCGWingChord) * wing.croot.front()) / fuselage.length;
                yCGWing = 0.0;
                zCGWing = (wing.zloc + 0.5 * wing.totalSpan * tan(wing.averageDihedral / 57.3)) / fuselage.diameter;
            }
            else
            {

                mainSparPositionWing = builderData.getKMainSparPosition() * wing.croot.front();
                secondarySparPositionWing = builderData.getKSecondarySparPosition() * wing.croot.front();

                xCGWing = (wing.xloc + mainSparPositionWing + 0.7 * (secondarySparPositionWing - mainSparPositionWing)) / fuselage.length;
                yCGWing = 0.0;
                zCGWing = (wing.zloc + 0.5 * wing.totalSpan * tan(wing.averageDihedral / 57.3)) / fuselage.diameter;
            }

            return {std::make_tuple(xCGWing, yCGWing, zCGWing)};
        }

        // CANARD COG

        std::tuple<double, double, double> inline calculateCOGCanard()
        {

            if (builderData.getHasCanard()) {

            xCGCanard = (canard.xloc + 0.5 * (minmumXCGWingChord + maximumXCGWingChord) * canard.croot.front()) / fuselage.length;
            yCGCanard = 0.0;
            zCGCanard = (canard.zloc + 0.5 * canard.totalSpan * tan(canard.averageDihedral / 57.3)) / fuselage.diameter;

            }

            return {std::make_tuple(xCGCanard, yCGCanard, zCGCanard)};
        }

        // HORIZONTAL TAIL COG

        std::tuple<double, double, double> inline calculateCOGHorizontalTail()
        {

            xCGHorizontal = (horizontalTail.xloc + maximumXCGWingChord * horizontalTail.croot.front()) / fuselage.length;
            yCGHorizontal = 0.0;
            zCGHorizontal = (horizontalTail.zloc + factorSpanHorizontal * horizontalTail.totalSpan * tan(horizontalTail.averageDihedral / 57.3)) / fuselage.diameter;

            return {std::make_tuple(xCGHorizontal, yCGHorizontal, zCGHorizontal)};
        }

        // VERTICAL TAIL COG

        std::tuple<double, double, double> inline calculateCOGVerticalTail()
        {

            if (verticalTail.averageDihedral > 0.0 || verticalTail.averageDihedral < 0.0)
            {

                verticalTail.totalProjectedSpan = 0.5 * verticalTail.totalProjectedSpan;
            }

            xCGVertical = (verticalTail.xloc + maximumXCGWingChord * verticalTail.croot.front() * cos(verticalTail.zrot / 57.3)) / fuselage.length;
            yCGVertical = 0.0;

            possibleZLocOfTheHorizontalTail = {0.0, verticalTail.zloc + verticalTail.totalProjectedSpan * cos(verticalTail.zrot / 57.3) +
                                                        verticalTail.croot.front() * sin(verticalTail.zrot / 57.3) -
                                                        verticalTail.ctip.back() * sin(verticalTail.zrot / 57.3)};

            Interpolant interp1(possibleZLocOfTheHorizontalTail, ratioSpanTovertical, 1, RegressionMethod::LINEAR);
            zCGVertical = (horizontalTail.zloc + interp1.getYValueFromRegression(horizontalTail.zloc)) / fuselage.diameter;

            return {std::make_tuple(xCGVertical, yCGVertical, zCGVertical)};
        }

        // FUSELAGE COG

        std::tuple<double, double, double> inline calculateCOGFuselage()
        {

            if (builderData.getUndercarriagePosition() == UndercarriagePosition::WING_MOUNTED)
            {

                xCGFuselage = 0.5 * (minimumPercentageOfXCGLocationFuselage + maximumPercentageOfXCGLocationFuselage) / fuselage.length;
                yCGFuselage = 0.0;
                zCGFuselage = 0.0;
            }

            else
            {

                xCGFuselage = percentageOfXCGLocationFuselageNoWingMountedLandingGear * fuselage.length / fuselage.length;
                yCGFuselage = 0.0;
                zCGFuselage = 0.0;
            }

            return {std::make_tuple(xCGFuselage, yCGFuselage, zCGFuselage)};
        }

        // BOOM COG

        std::tuple<double, double, double> inline calculateCOGBoom()
        {

            if (builderData.getHasBoom())
            {
                xCGBoom = (boom.xloc.front() + 0.5 * (minimumPercentageOfXCGLocationBoom + maximumPercentageOfXCGLocationBoom) * boom.length.front()) / fuselage.length;
                yCGBoom = 0.0;
                zCGBoom = (boom.zloc.front() - boom.length.front() * sin(boom.yrot.front() / 57.3)) / fuselage.diameter;
            }

            return {std::make_tuple(xCGBoom, yCGBoom, zCGBoom)};
        }

        // EOIR COG

        std::tuple<double, double, double> inline calculateCOGEOIR()
        {

            if (builderData.getHasEOIR())
            {
                xCGEOIR = (eoir.xloc.front() + 0.5 * eoir.diameter * eoir.fineratio.front()) / fuselage.length;
                yCGEOIR = 0.0;
                zCGEOIR = (eoir.zloc.front() + 0.5 * eoir.diameter) / fuselage.diameter;
            }

            return {std::make_tuple(xCGEOIR, yCGEOIR, zCGEOIR)};
        }

        // LANDING GEAR COG

        std::tuple<double, double, double> inline calculateCOGLandingGear()
        {

            weight.landingGearWeightTorenbeek(builderData);
            noseLandingGearWeight = weight.getNoseLandingGearWeightTorenbeek();
            mainLandingGearWeight = weight.getMainLandingGearWeightTorenbeek();

            if (builderData.getUndercarriagePosition() == UndercarriagePosition::WING_MOUNTED)
            {

                xCGLandingGear = ((noseLandingGearWeight * percentageOfXCGLocationNoseLandingGear + mainLandingGearWeight * percentageOfXCGLocationMainLandingGear) /
                                  (noseLandingGearWeight + mainLandingGearWeight)) /
                                 fuselage.length;
                yCGLandingGear = 0.0;
                zCGLandingGear = -(0.5 * fuselage.diameter + builderData.getStrutLength()) / fuselage.diameter;
            }

            else
            {

                xCGLandingGear = ((noseLandingGearWeight * percentageOfXCGLocationNoseLandingGearNoWingMounted + mainLandingGearWeight * percentageOfXCGLocationMainLandingGearNoWingMounted) /
                                  (noseLandingGearWeight + mainLandingGearWeight));
                yCGLandingGear = 0.0;
                zCGLandingGear = -(0.5 * fuselage.diameter + builderData.getStrutLength()) / fuselage.diameter;

                
            }
            return {std::make_tuple(xCGLandingGear, yCGLandingGear, zCGLandingGear)};
        }

        // CONTROL SURFACES COG

        std::tuple<double, double, double> inline calculateCOGControlSurfaces()
        {

            if (builderData.getUndercarriagePosition() == UndercarriagePosition::WING_MOUNTED)
            {

                xCGControlSurfaces = (percentageOfXCGLocationControlSurfaces * fuselage.length) / fuselage.length;
                yCGControlSurfaces = 0.0;
                zCGControlSurfaces = ((wing.zloc * wing.planformArea + canard.zloc * canard.planformArea + horizontalTail.zloc * horizontalTail.planformArea + verticalTail.zloc * verticalTail.planformArea) /
                                      (wing.planformArea + canard.planformArea + horizontalTail.planformArea + verticalTail.planformArea)) /
                                     fuselage.diameter;
            }

            else
            {

                xCGControlSurfaces = (percentageOfXCGLocationControlSurfacesNoWingMounted * fuselage.length) / fuselage.length;
                yCGControlSurfaces = 0.0;
                zCGControlSurfaces = ((wing.zloc * wing.planformArea + canard.zloc * canard.planformArea + horizontalTail.zloc * horizontalTail.planformArea + verticalTail.zloc * verticalTail.planformArea) /
                                      (wing.planformArea + canard.planformArea + horizontalTail.planformArea + verticalTail.planformArea)) /
                                     fuselage.diameter;

              
            }

              return {std::make_tuple(xCGControlSurfaces, yCGControlSurfaces, zCGControlSurfaces)};
        }

        // PROPULSION GROUP COG

        std::tuple<double, double, double> inline calculateCOGPropulsionGroup()
        {

            xCGPropulsionGroup = (nacelle.xloc.front() + percentageOfXCGLocationNacelle * nacelle.length) / fuselage.length;
            yCGPropulsionGroup = 0.0;
            zCGPropulsionGroup = nacelle.zloc.front() / fuselage.diameter;

            return {std::make_tuple(xCGPropulsionGroup, yCGPropulsionGroup, zCGPropulsionGroup)};
        }

        // APU COG

        std::tuple<double, double, double> inline calculateCOGAPU()
        {

            if (builderData.getHasAPU())
            {

                xCGAPU = (percentageOfXCGLocationAPU * fuselage.length) / fuselage.length;
                yCGAPU = 0.0;
                zCGAPU = 0.0 / fuselage.diameter;
            }

            return {std::make_tuple(xCGAPU, yCGAPU, zCGAPU)};
        }

        // INSTRUMENTS COG

        std::tuple<double, double, double> inline calculateCOGInstruments()
        {

            xCGInstruments = (percentageOfXCGLocationInstruments * fuselage.length) / fuselage.length;
            yCGInstruments = 0.0;
            zCGInstruments = (percentageOfZCGLocationInstruments * fuselage.diameter) / fuselage.diameter;

            return {std::make_tuple(xCGInstruments, yCGInstruments, zCGInstruments)};
        }

        // HYDRAULIC AND PNEUMATIC SYSTEMS COG

        std::tuple<double, double, double> inline calculateCOGHydraulicAndPneumatic()
        {

            xCGHydraulicAndPneumatic = (percentageOfXCGLocationHydraulicAndPneumatic * fuselage.length) / fuselage.length;
            yCGHydraulicAndPneumatic = 0.0;
            zCGHydraulicAndPneumatic = ((wing.zloc * wing.planformArea + horizontalTail.zloc * horizontalTail.planformArea) /
                                        (wing.planformArea + horizontalTail.planformArea)) /
                                       fuselage.diameter;

            return {std::make_tuple(xCGHydraulicAndPneumatic, yCGHydraulicAndPneumatic, zCGHydraulicAndPneumatic)};
        }

        // ELECTRICAL GROUP COG

        std::tuple<double, double, double> inline calculateCOGElectricalGroup()
        {

            xCGElectricalGroup = (percentageOfXCGLocationElectricalGroup * fuselage.length) / fuselage.length;
            yCGElectricalGroup = 0.0;
            zCGElectricalGroup = 0.0 / fuselage.diameter;

            return {std::make_tuple(xCGElectricalGroup, yCGElectricalGroup, zCGElectricalGroup)};
        }

        // AVIONIC GROUP COG

        std::tuple<double, double, double> inline calculateCOGAvionicGroup()
        {

            xCGAvionicGroup = (percentageOfXCGLocationAvionicGroup * fuselage.length) / fuselage.length;
            yCGAvionicGroup = 0.0;
            zCGAvionicGroup = 0.0 / fuselage.diameter;

            return {std::make_tuple(xCGAvionicGroup, yCGAvionicGroup, zCGAvionicGroup)};
        }

        // FURNISHINGS AND EQUIPMENT COG

        std::tuple<double, double, double> inline calculateCOGFurnishingsAndEquipment()
        {

            if (builderData.getHasFurnishinghAndEquipment())
            {
                xCGFurnishingsAndEquipment = (percentageOfXCGLocationFurnishingsAndEquipment * fuselage.length) / fuselage.length;
                yCGFurnishingsAndEquipment = 0.0;
                zCGFurnishingsAndEquipment = 0.0 / fuselage.diameter;
            }

            return {std::make_tuple(xCGFurnishingsAndEquipment, yCGFurnishingsAndEquipment, zCGFurnishingsAndEquipment)};
        }

        // AIR CONDITIONING AND ANTI-ICE COG

        std::tuple<double, double, double> inline calculateCOGAirConditioningAndAntiIce()
        {

            if (builderData.getHasAirConditioningAndAntiIce())
            {
                xCGAirConditioningAndAntiIce = (percentageOfXCGLocationAirConditioningAndAntiIce * fuselage.length) / fuselage.length;
                yCGAirConditioningAndAntiIce = 0.0;
                zCGAirConditioningAndAntiIce = (percentageOfZCGLocationAirConditioningAndAntiIce * fuselage.diameter) / fuselage.diameter;
            }

            return {std::make_tuple(xCGAirConditioningAndAntiIce, yCGAirConditioningAndAntiIce, zCGAirConditioningAndAntiIce)};
        }

        // OPERATING ITEMS COG

        std::tuple<double, double, double> inline calculateCOGOperatingItems()
        {

            xCGOperatingItems = (percentageOfXCGLocationOperatingItems * fuselage.length) / fuselage.length;
            yCGOperatingItems = 0.0;
            zCGOperatingItems = 0.0 / fuselage.diameter;

            return {std::make_tuple(xCGOperatingItems, yCGOperatingItems, zCGOperatingItems)};
        }

        // PAYLOAD COG

        std::tuple<double, double, double> inline calculateCOGPayload()
        {

            xCGPayload = (percentageOfXCGLocationPayload * fuselage.length) / fuselage.length;
            yCGPayload = 0.0;
            zCGPayload = 0.0 / fuselage.diameter;

            return {std::make_tuple(xCGPayload, yCGPayload, zCGPayload)};
        }

        // CREW COG

        std::tuple<double, double, double> inline calculateCOGCrew()
        {

            xCGCrew = (percentageOfXCGLocationCrew * fuselage.length) / fuselage.length;
            yCGCrew = 0.0;
            zCGCrew = 0.0 / fuselage.diameter;

            return {std::make_tuple(xCGCrew, yCGCrew, zCGCrew)};
        }

        // FUEL COG

        std::tuple<double, double, double> inline calculateCOGFuel()
        {

            if (wing.averageLeadingEdgeSweep == 0.0)
            {

                xCGFuel = (wing.xloc + percentageOfMACLocationFuel * wing.MAC) / fuselage.length;
                yCGFuel = 0.0;
                zCGFuel = 0.0 / fuselage.diameter;
            }

            else
            {

                mainSparPositionWing = builderData.getKMainSparPosition() * wing.croot.front();
                secondarySparPositionWing = builderData.getKSecondarySparPosition() * wing.croot.front();

                xCGFuel = (wing.xloc + (mainSparPositionWing + (secondarySparPositionWing - mainSparPositionWing) * tan(wing.averageLeadingEdgeSweep / 57.3)) + 0.5 * wing.MAC) / fuselage.length;
                yCGFuel = 0.0;
                zCGFuel = 0.0 / fuselage.diameter;
            }

            return {std::make_tuple(xCGFuel, yCGFuel, zCGFuel)};
        }

        // ======================= Weights function =======================
        void inline getWeights()
        {


            // Calculate wetted area for all components (fuselage, nacelles, boom if present)
            wettedAreaCalculator.getAllGeoms(nameOfAircraft, "GetGeomOfAircraft.vspscript");

            // Access and display results
            const auto &results = wettedAreaCalculator.getWettedAreaResults();

            // =========== WING WEIGHT ===========
            switch (builderData.getWeightMethodWing())
            {

            case WeightMethod::WING_TORENBEEK:

            {

                ConvSweep SforzaConverterWing(wing.averageLeadingEdgeSweep, wing.taperRatio, wing.aspectRatio, 0.0, 0.5);
                sweepC2 = SforzaConverterWing.getSweepAngle();

                wingWeight = weight.wingWeightTorenbeek(wingData,
                                                        wing.totalProjectedSpan,
                                                        sweepC2,
                                                        wing.croot.front(),
                                                        wing.planformArea);
            }
            break;

            case WeightMethod::WING_SADRAEY:

            {

                ConvSweep SforzaConverterWing(wing.averageLeadingEdgeSweep, wing.taperRatio, wing.aspectRatio, 0.0, 0.25);
                sweepC4 = SforzaConverterWing.getSweepAngle();

                wingWeight = weight.wingWeightSadraey(wingData,
                                                      wing.planformArea,
                                                      wing.MAC,
                                                      wing.aspectRatio,
                                                      wing.taperRatio,
                                                      sweepC4);
            }
            break;

            case WeightMethod::WING_CHIOZZOTTO:

            {
                ConvSweep SforzaConverterWing(wing.averageLeadingEdgeSweep, wing.taperRatio, wing.aspectRatio, 0.0, 0.5);
                sweepC2 = SforzaConverterWing.getSweepAngle();

                wingWeight = weight.wingWeightChiozzotto(wingData,
                                                         wing.planformArea,
                                                         wing.aspectRatio,
                                                         wing.taperRatio,
                                                         sweepC2);
            }

            break;
            }

            // =========== CANARD WEIGHT ===========

            if (builderData.getHasCanard())
            {
                ConvSweep SforzaConverterCanard(canard.averageLeadingEdgeSweep, canard.taperRatio, canard.aspectRatio, 0.0, 0.5);
                sweepC2 = SforzaConverterCanard.getSweepAngle();

                canardWeight = weight.canardWeightTorenbeek(builderData,
                                                            canard.totalProjectedSpan,
                                                            canard.planformArea,
                                                            sweepC2);
            }

            // =========== HORIZONTAL TAIL WEIGHT ===========

            ConvSweep SforzaConverterHorizontal(horizontalTail.averageLeadingEdgeSweep, horizontalTail.taperRatio, horizontalTail.aspectRatio, 0.0, 0.5);
            sweepC2 = SforzaConverterHorizontal.getSweepAngle();
            horizontalWeight = weight.horizontalTailWeightTorenbeek(wingData,
                                                                    horizontalTail.planformArea,
                                                                    sweepC2);

            // =========== VERTICAL TAIL WEIGHT ===========

            ConvSweep SforzaConverterVertical(verticalTail.averageLeadingEdgeSweep, verticalTail.taperRatio, verticalTail.aspectRatio, 0.0, 0.5);
            sweepC2 = SforzaConverterVertical.getSweepAngle();
            verticalWeight = weight.verticalTailWeightTorenbeek(wingData,
                                                                verticalTail.totalProjectedSpan,
                                                                verticalTail.planformArea,
                                                                sweepC2,
                                                                horizontalTail.zloc,
                                                                horizontalTail.planformArea);

            // =========== FUSELAGE WEIGHT ===========

            switch (builderData.getWeightMethodFuselage())
            {

            case WeightMethod::FUSELAGE_TORENBEEK:

            {

                fuselageWeight = weight.fuselageWeightTorenbeek(fuselageData,
                                                                fuselage.tailArm,
                                                                fuselage.width,
                                                                fuselage.diameter,
                                                                results.WET_FUSE_AREA);
            }

            break;

            case WeightMethod::FUSELAGE_TORENBEEK_MATERIAL:

            {

                fuselageWeight = weight.fuselgeWeightTorenbeekMaterial(fuselageData,
                                                                       fuselage.tailArm,
                                                                       fuselage.width,
                                                                       fuselage.diameter,
                                                                       results.WET_FUSE_AREA);
            }
            break;
            }

            // =========== TAIL BOOM WEIGHT ===========

            if (builderData.getHasBoom())  {
            boomWeight = weight.tailBoomWeightRoskam(builderData,
                                                     boom.width,
                                                     boom.height,
                                                     results.WET_BOOM_AREA,
                                                     fuselage.tailArm);

            }

            // =========== EO/IR WEIGHT ===========

            if (builderData.getHasEOIR())
            {
                eoirWeight = weight.eoirWeightGundlach(builderData,
                                                       eoir.diameter,
                                                       eoir.fineratio.front());
            }

            // =========== LANDING GEAR WEIGHT ===========

            landingGearWeight = weight.landingGearWeightTorenbeek(builderData);

            // =========== CONTROL SURFACES WEIGHT ===========

            controlSurfacesWeight = weight.contrsolSurfaceWeightTorenbeek(builderData);

            // =========== PROPULSION GROUP WEIGHT ===========

            propulsionGroupWeight = weight.propulsionGroupWeight(engineData);

            // =========== NACELLE WEIGHT ===========

            nacelleWeight = weight.nacelleWeight(engineData, results.WET_NAC_AREA);

            // =========== APU WEIGHT ===========

            apuWeight = weight.apuWeight(builderData);

            // =========== INSTRUMENTS WEIGHT ===========

            instrumentsWeight = weight.instrumentWeight(builderData);

            // =========== HYDRAULIC AND PNEUMATIC SYSTEM WEIGHT ===========

            hydraulicAndPneumaticWeight = weight.hydraulicSystemWeight(builderData);

            // =========== ELECTRICAL GROUP WEIGHT ===========

            electricalGroupWeight = weight.electricalGroupWeight(builderData);

            // =========== AVIONICS GROUP WEIGHT ===========

            avionicGroupWeight = weight.avionicsWeight(builderData);

            // =========== FURNISHINGS AND EQUIPMENT WEIGHT ===========

            furnishingsAndEquipmentWeight = weight.furnishingsAndEquipmentWeight(builderData);

            // =========== AIR CONDITIONING AND ANTI-ICE WEIGHT ===========

            airConditioningAndAntiIceWeight = weight.airConditioningAndAntiIceWeight(builderData);

            // =========== OPERATING ITEMS WEIGHT ===========

            operatingItemsWeight = weight.operatingItemsWeight(builderData);

            // =========== PAYLOAD WEIGHT ===========

            payloadWeight = builderData.getPayloadWeight();

            // =========== CREW WEIGHT ===========

            crewWeight = builderData.getCrewWeight();

            // =========== FUEL WEIGHT ===========

            fuelWeight = builderData.getFuelWeight();

            weights = {
                .wingWeight = wingWeight,
                .canardWeight = canardWeight,
                .horizontalWeight = horizontalWeight,
                .verticalWeight = verticalWeight,
                .fuselageWeight = fuselageWeight,
                .nacelleWeight = nacelleWeight,
                .boomWeight = boomWeight,
                .eoirWeight = eoirWeight,
                .landingGearWeight = landingGearWeight,
                .controlSurfacesWeight = controlSurfacesWeight,
                .propulsionGroupWeight = propulsionGroupWeight,
                .apuWeight = apuWeight,
                .instrumentsWeight = instrumentsWeight,
                .hydraulicAndPneumaticWeight = hydraulicAndPneumaticWeight,
                .electricalGroupWeight = electricalGroupWeight,
                .avionicGroupWeight = avionicGroupWeight,
                .furnishingsAndEquipmentWeight = furnishingsAndEquipmentWeight,
                .airConditioningAndAntiIceWeight = airConditioningAndAntiIceWeight,
                .operatingItemsWeight = operatingItemsWeight,
                .payloadWeight = payloadWeight,
                .crewWeight = crewWeight,
                .fuelWeight = fuelWeight,
                .totalAircraftWeight = wingWeight + canardWeight + horizontalWeight +
                                       verticalWeight + fuselageWeight + nacelleWeight + boomWeight +
                                       eoirWeight + landingGearWeight + controlSurfacesWeight +
                                       propulsionGroupWeight + apuWeight +
                                       instrumentsWeight + hydraulicAndPneumaticWeight +
                                       electricalGroupWeight + avionicGroupWeight +
                                       furnishingsAndEquipmentWeight + airConditioningAndAntiIceWeight +
                                       operatingItemsWeight + payloadWeight + crewWeight + fuelWeight};
        }

        std::tuple<double, double, double> inline calculateCOGAircraft()
        {

        
            totalAircraftWeight = wingWeight +
                                  canardWeight +
                                  horizontalWeight +
                                  verticalWeight +
                                  fuselageWeight +
                                  boomWeight +
                                  eoirWeight +
                                  landingGearWeight +
                                  controlSurfacesWeight +
                                  propulsionGroupWeight +
                                  nacelleWeight +
                                  apuWeight +
                                  instrumentsWeight +
                                  hydraulicAndPneumaticWeight +
                                  electricalGroupWeight +
                                  avionicGroupWeight +
                                  furnishingsAndEquipmentWeight +
                                  airConditioningAndAntiIceWeight +
                                  operatingItemsWeight +
                                  payloadWeight +
                                  crewWeight +
                                  fuelWeight;

            auto [xCGWing, yCGWing, zCGWing] = calculateCOGWing();

            auto [xCGCanard, yCGCanard, zCGCanard] = calculateCOGCanard();

            auto [xCGHorizontal, yCGHorizontal, zCGHorizontal] = calculateCOGHorizontalTail();

            auto [xCGVertical, yCGVertical, zCGVertical] = calculateCOGVerticalTail();

            auto [xCGFuselage, yCGFuselage, zCGFuselage] = calculateCOGFuselage();

            auto [xCGBoom, yCGBoom, zCGBoom] = calculateCOGBoom();

            auto [xCGEOIR, yCGEOIR, zCGEOIR] = calculateCOGEOIR();

            auto [xCGLandingGear, yCGLandingGear, zCGLandingGear] = calculateCOGLandingGear();

            auto [xCGControlSurfaces, yCGControlSurfaces, zCGControlSurfaces] = calculateCOGControlSurfaces();

            auto [xCGPropulsionGroup, yCGPropulsionGroup, zCGPropulsionGroup] = calculateCOGPropulsionGroup();

            auto [xCGAPU, yCGAPU, zCGAPU] = calculateCOGAPU();

            auto [xCGInstruments, yCGInstruments, zCGInstruments] = calculateCOGInstruments();

            auto [xCGHydraulicAndPneumatic, yCGHydraulicAndPneumatic, zCGHydraulicAndPneumatic] = calculateCOGHydraulicAndPneumatic();

            auto [xCGElectricalGroup, yCGElectricalGroup, zCGElectricalGroup] = calculateCOGElectricalGroup();

            auto [xCGAvionicGroup, yCGAvionicGroup, zCGAvionicGroup] = calculateCOGAvionicGroup();

            auto [xCGFurnishingsAndEquipment, yCGFurnishingsAndEquipment, zCGFurnishingsAndEquipment] = calculateCOGFurnishingsAndEquipment();

            auto [xCGAirConditioningAndAntiIce, yCGAirConditioningAndAntiIce, zCGAirConditioningAndAntiIce] = calculateCOGAirConditioningAndAntiIce();

            auto [xCGOperatingItems, yCGOperatingItems, zCGOperatingItems] = calculateCOGOperatingItems();

            auto [xCGPayload, yCGPayload, zCGPayload] = calculateCOGPayload();

            auto [xCGCrew, yCGCrew, zCGCrew] = calculateCOGCrew();

            auto [xCGFuel, yCGFuel, zCGFuel] = calculateCOGFuel();

            xCGTotalAircraft = fuselage.length * (wingWeight * xCGWing + canardWeight * xCGCanard + horizontalWeight * xCGHorizontal + verticalWeight * xCGVertical + fuselageWeight * xCGFuselage + boomWeight * xCGBoom + eoirWeight * xCGEOIR + landingGearWeight * xCGLandingGear + controlSurfacesWeight * xCGControlSurfaces + (propulsionGroupWeight + nacelleWeight) * xCGPropulsionGroup + apuWeight * xCGAPU + instrumentsWeight * xCGInstruments + hydraulicAndPneumaticWeight * xCGHydraulicAndPneumatic + electricalGroupWeight * xCGElectricalGroup + avionicGroupWeight * xCGAvionicGroup + furnishingsAndEquipmentWeight * xCGFurnishingsAndEquipment + airConditioningAndAntiIceWeight * xCGAirConditioningAndAntiIce + operatingItemsWeight * xCGOperatingItems + payloadWeight * xCGPayload + crewWeight * xCGCrew + fuelWeight * xCGFuel) / totalAircraftWeight;

            yCGTotalAircraft = 0.0;

            zCGTotalAircraft = fuselage.diameter * (wingWeight * zCGWing + canardWeight * zCGCanard + horizontalWeight * zCGHorizontal + verticalWeight * zCGVertical + fuselageWeight * zCGFuselage + boomWeight * zCGBoom + eoirWeight * zCGEOIR + landingGearWeight * zCGLandingGear + controlSurfacesWeight * zCGControlSurfaces + (propulsionGroupWeight + nacelleWeight) * zCGPropulsionGroup + apuWeight * zCGAPU + instrumentsWeight * zCGInstruments + hydraulicAndPneumaticWeight * zCGHydraulicAndPneumatic + electricalGroupWeight * zCGElectricalGroup + avionicGroupWeight * zCGAvionicGroup + furnishingsAndEquipmentWeight * zCGFurnishingsAndEquipment + airConditioningAndAntiIceWeight * zCGAirConditioningAndAntiIce + operatingItemsWeight * zCGOperatingItems + payloadWeight * zCGPayload + crewWeight * zCGCrew + fuelWeight * zCGFuel) / totalAircraftWeight;

            centerOfGravityData = {.xCG = xCGTotalAircraft,
                                   .yCG = yCGTotalAircraft,
                                   .zCG = zCGTotalAircraft};

            return {std::make_tuple(xCGTotalAircraft, yCGTotalAircraft, zCGTotalAircraft)};
        }

        // In COGCalculator, sezione public:
        COG::Weights getWeightsData() const { return weights; }
        COG::COGDATA getCOGData() const { return centerOfGravityData; }
    };
}
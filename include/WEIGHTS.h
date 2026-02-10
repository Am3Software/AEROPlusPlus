#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "Interpolant.h"
#include "RegressionMethod.h"
#include "WINGWEIGHTDATA.h"
#include "FUSELAGEWEIGHTDATA.h"
#include "ENGINEWEIGHTDATA.h"
#include "EnumAircraftCategory.h"
#include "EnumTypeOfComposite.h"
#include "EnumAircraftEngineType.h"
#include "EnumTypeOfStabilizer.h"
#include "EnumTypeOfTail.h"
#include "EnumEnginePosition.h"
#include "EnumUndercarriagePosition.h"
#include "EnumMaterial.h"
#include "EnumWingPosition.h"
#include "EnumFORCE.h"
#include "EnumMASS.h"
#include "EnumLENGTH.h"
#include "EnumAREA.h"
#include "EnumSPEED.h"
#include "ConvMass.h"
#include "ConvForce.h"
#include "ConvLength.h"
#include "ConvArea.h"
#include "ConvVel.h"

class Weight
{

private:
    // Wing related variables
    double Kw = 0.0;
    double Kp = 0.0;
    double referenceSpan = 1.905;
    double wingWeight = 0.0;
    double rhoComposite = 0.0;

    // Canard related variables
    double canardWeight = 0.0;

    // Horizontal tail related variables
    double horizontalTailWeight = 0.0;
    double Kh = 0.0;
    double ratioTorenbeekFncToHor = 0.0;
    double weightToPlanformSurfaceRatioToHor = 0.0;

    // Vertical tail related variables
    double verticalTailWeight = 0.0;
    double Kv = 0.0;
    double ratioTorenbeekFncToVer = 0.0;
    double weightToPlanformSurfaceRatioToVer = 0.0;

    // Fuselage related variables
    double fuselageWeight = 0.0;
    double Kwf = 0.23;
    double frameDensityRatio = 0.0;
    double skinDensityRatio = 0.0;
    double stringersDensityRatio = 0.0;
    double kLambda = 0.0;
    double weightFrames = 0.0;
    double weightSkinPanels = 0.0;
    double weightStringers = 0.0;
    double alluminiumFactor = 0.0;

    // Tail boom related variables
    double tailBoomWeightEstimated = 0.0;
    double sumWidthAndHeightBoomInFeet = 0.0;
    double diveSpeedConvertedInKnots = 0.0;
    double tailArmConvertedInFeet = 0.0;
    double boomWettedAreaConvertedInSqFeet = 0.0;

    // EO/IR related variables
    double eoirWeightEstimated = 0.0;
    double factorToEOIRWeight = 0.0164;
    double factorToDiamEoir = 39.37;

    // Undercarriage related variables
    double undercarriageWeight = 0.0;
    double lndMainWeight = 0.0;
    double lndNoseWeight = 0.0;
    double kUC = 0.0;
    double kLG = 0.0;
    double Amain, Bmain, Cmain, Dmain;
    double Anose, Bnose, Cnose, Dnose;
    double factorLngMainToUAV = 0.6;

    // Control surfaces related variables
    double controlSurfaceWeight = 0.0;

    // Engine related variables
    double engineGroupWeight = 0.0;
    double singleEngineWeight = 0.0;
   

    // Nacelle related variables
    double nacelleGroupWeight = 0.0;

    // APU related variables
    double apuWeightEstimated = 0.0;

    // Instrumentation related variables
    double instrumentationWeight = 0.0;

    // Hydraulic system related variables
    double hydraulicSystemWeightEstimated = 0.0;

    // Electrical system related variables
    double electricalSystemWeightEstimated = 0.0;

    // Avionics related variables
    double avionicsWeightEstimated = 0.0;

    // Furnishings and Equipment related variables
    double furnishingsAndEquipmentWeightEstimated = 0.0;

    // Air conditioning and ice protection related variables
    double airConditioningAndIceProtectionWeightEstimated = 0.0;

    // Operating items related variables
    double operatingItemsWeightEstimated = 0.0;

public:
    Weight() {

    };

    // ================= WING WEIGHT METHODS =================

    /// @brief Wing weight according to Torenbeek method.
    /// @param WTO Maximum take-off weight
    /// @param nUlt Ultimate load factor
    /// @param span Wing span
    /// @param sweepC2 Sweep at 50% chord (in degrees)
    /// @param cRoot Root chord
    /// @param thicknessToRootChordRatio Thickness-to-root-chord ratio (t/c)
    /// @param planformArea Planform area
    /// @param nWingMountedEngines Number of wing-mounted engines
    /// @param undercarriageType Type of undercarriage
    /// @param hasSpoilers Indicates if the wing has spoilers
    /// @param isBracedWing Indicates if the wing is braced
    /// @param hasFowlerFlap Indicates if the wing has Fowler flaps
    /// @return The calculated wing weight
    double wingWeightTorenbeek(WingWeightData wingData, double span, double sweepC2,
                               double cRoot, double planformArea)

    {

        if (wingData.getWTO() <= 5670.0)
        {

            Kw = 4.90 * std::pow(10, -3);
        }

        else
        {

            Kw = 6.67 * std::pow(10, -3);
        }

        double effectiveSpan = span / std::cos(sweepC2 / 57.3);

        wingWeight = (Kw * std::pow(effectiveSpan, 0.75) * (1 + std::sqrt(referenceSpan / effectiveSpan)) * std::pow(wingData.getNUltimateLoad(), 0.55) *
                      std::pow((effectiveSpan / (wingData.getThicknessToRootChordRatioWing() * cRoot)) / (wingData.getWTO() / planformArea), 0.30) * wingData.getWTO());

        // Adjustments for additional features
        if (wingData.getUndercarriagePosition() == UndercarriagePosition::BODY_MOUNTED)
        {

            wingWeight = wingWeight * (1 - 0.05);
        }

        // Increment due to spoilers
        if (wingData.getHasSpoilers())
        {

            wingWeight = wingWeight * (1 + 0.02);
        }

        if (wingData.getIsBracedWing())
        {

            wingWeight = wingWeight * 0.7;
        }

        if (wingData.getHasFowlerFlap())
        {

            wingWeight = wingWeight * 1.02;
        }

        // Reduction due to wing - mounted engines
        wingWeight = wingWeight * (1 - wingData.getNWingMountedEngines() * 0.025);

        return wingWeight;
    }

    double canardWeightTorenbeek(BaseWeightData canardData, double span, double planformArea,double sweepC2)
    {

        if (canardData.getHasCanard())
        {

            Kv = 1;

            std::vector<double> X_DATA = {
                0.2102, 0.2208, 0.2330, 0.2474, 0.2626, 0.2801, 0.2961, 0.3075,
                0.3227, 0.3379, 0.3600, 0.3851, 0.3987, 0.4086, 0.4231, 0.4345,
                0.4474, 0.4611, 0.4770, 0.4877, 0.5028, 0.5248, 0.5430, 0.5605,
                0.5749, 0.5893, 0.6074, 0.6203, 0.6347, 0.6453};

            std::vector<double> Y_DATA = {
                9.9352, 10.6116, 11.2873, 12.2459, 13.2041, 14.2749, 15.1759, 15.9088,
                16.8670, 17.8252, 19.1209, 20.6992, 21.4309, 22.1077, 23.0095, 23.6856,
                24.3609, 25.0358, 25.7096, 26.2156, 26.7761, 27.5036, 28.1194, 28.5084,
                28.7852, 29.0052, 29.3369, 29.5009, 29.5504, 29.4883};

            ratioTorenbeekFncToVer = (std::pow(planformArea, 0.2) * canardData.getDiveSpeed()) / (1000 * std::pow(std::cos(sweepC2 / 57.3), 0.5));

            Interpolant interp1(X_DATA, Y_DATA, 1, RegressionMethod::LINEAR);

            weightToPlanformSurfaceRatioToVer = interp1.getYValueFromRegression(ratioTorenbeekFncToVer);
            verticalTailWeight = Kv * weightToPlanformSurfaceRatioToVer * planformArea;
        }

        return canardWeight;
    }

    /// @brief Wing weight according to Sadraey method.
    /// @param planformArea Planform area (m^2)
    /// @param MAC Mean Aerodynamic Chord (m)
    /// @param thicknessToChordRatio Thickness-to-chord ratio (t/c)
    /// @param aspectRatio Aspect ratio
    /// @param nUlt Ultimate load factor
    /// @param taperRatio Taper ratio
    /// @param sweepC4 Sweep at 25% chord (in degrees)
    /// @param percentageComposite Percentage of composite material
    /// @param typeComposite Type of composite material
    /// @param aircraftCategory Category of the aircraft
    /// @param isEngineInstallatedOnWing Indicates if the engine is installed on the wing
    /// @param isFueklTankInstallatedOnWing Indicates if the fuel tank is installed on the wing
    /// @return The calculated wing weight
    double wingWeightSadraey(WingWeightData wingData, double planformArea, double MAC,
                             double aspectRatio, double taperRatio, double sweepC4)
    {

        switch (wingData.getAircraftCategory())
        {

        case AircraftCategory::GENERAL_AVIATION:
        case AircraftCategory::UAV: // Fallthrough for UAVs

        {

            if (wingData.getIsEngineInstallatedOnWing() && wingData.getIsFuelTankInstallatedOnWing())
            {

                Kp = 0.0033;
            }

            else if (!wingData.getIsEngineInstallatedOnWing() && wingData.getIsFuelTankInstallatedOnWing())
            {

                Kp = 0.0016;
            }

            else if (!wingData.getIsEngineInstallatedOnWing() && !wingData.getIsFuelTankInstallatedOnWing())
            {

                Kp = 0.00275;
            }

            else
            {

                Kp = 0.0012;
            }
        }

        break;

        case AircraftCategory::TRANSPORT_JET:
        {

            if (wingData.getIsEngineInstallatedOnWing())
            {

                Kp = 0.00375;
            }

            else
            {

                Kp = 0.00275;
            }
        }
        break;
        }

        if (wingData.getTypeOfComposite() == TypeOfComposite::FIBERGLASS)
        {

            rhoComposite = 1825.0;
        }

        else
        {

            rhoComposite = 1575.0;
        }

        double rhoMaterial = (wingData.getPercentageComposite() * rhoComposite + (100.0 - wingData.getPercentageComposite()) * 2711.0) / 100.0;

        double factorWeight = std::pow((aspectRatio * wingData.getNUltimateLoad()) / (std::cos(sweepC4 / 57.3)), 0.6);

        wingWeight = planformArea * MAC * wingData.getThicknessToRootChordRatioWing() * rhoMaterial * Kp * std::pow(taperRatio, 0.04) * factorWeight;

        return wingWeight;
    }

    /// @brief Wing weight according to Chiozzotto method.
    /// @param WTO Maximum take-off weight (kg)
    /// @param positiveLimitLoadFactor Positive limit load factor
    /// @param maxOperatingEAS Maximum operating EAS at sea level (m/s)
    /// @param nWingMountedEngines Number of wing-mounted engines
    /// @param isSimplifiedFlapSystem Boolean flag for simple flap systems
    /// @param isWingStrutBraced Boolean flag for wing struts presence
    /// @param wingStrutPosition Relative position of strut as fraction of wing span
    /// @param strutToWingChordRatio Ratio of strut chord to wing chord
    /// @param isCompositeWing Boolean flag for CFRP usage
    /// @param wingPlanformArea Wing planform area (m^2)
    /// @param wingAspectRatio Wing aspect ratio
    /// @param wingTaperRatio Ratio between tip chord and root chord
    /// @param wingKinkThicknessRatio Wing airfoil thickness at kink (0.0-1.0)
    /// @param wingHalfBoxSweepAngle Area-averaged wing sweep angle at half wing box (degrees)
    /// @return The calculated wing weight (kg)
    double wingWeightChiozzotto(
        WingWeightData wingData,
        double wingPlanformArea,
        double wingAspectRatio,
        double wingTaperRatio,
        double wingHalfBoxSweepAngle)
    {

        double strutsParameter;

        if (!wingData.getIsBracedWing())
        {
            wingData.setWingStrutPosition(0.0);
            strutsParameter = 0.0;
        }
        else
        {
            strutsParameter = 1.0 - (std::pow(wingData.getStrutToWingChordRatio(), 0.5) *
                                     std::pow(wingData.getWingStrutPosition(), 2)) /
                                        std::pow(wingAspectRatio, 0.5);
        }

        // Coefficienti per le diverse configurazioni
        double cCovers, eMTOCovers, eWOverSCovers, eARCovers, eSweepCovers;
        double eThickCovers, eVMOCovers, eTaperCovers, eLoadCovers, eEtaStrutCovers;

        double cRibsWebs, eMTORibsWebs, eWOverSRibsWebs, eARRibsWebs, eSweepRibsWebs;
        double eThickRibsWebs, eVMORibsWebs, eTaperRibsWebs, eLoadRibsWebs, eEtaStrutRibsWebs;

        double cStrutsJuries, eMTOStrutsJuries, eWOverSStrutsJuries, eARStrutsJuries;
        double eSweepStrutsJuries, eThickStrutsJuries, eVMOStrutsJuries, eTaperStrutsJuries;
        double eLoadStrutsJuries, eEtaStrutStrutsJuries, eStrutParamStrutsJuries;

        double cAileronEff, eMTOAileronEff, eWOverSAileronEff, eARAileronEff;
        double eSweepAileronEff, eThickAileronEff, eVMOAileronEff, eTaperAileronEff;
        double eLoadAileronEff, eEtaStrutAileronEff, eStrutParamAileronEff;

        double kEngineReliefCovers, kEngineReliefRibsWebs, kEngineReliefStrutsJuries;

        if (wingData.getIsCompositeWing())
        {
            if (wingData.getIsBracedWing())
            {
                if (wingHalfBoxSweepAngle < 0.0)
                {
                    // FS-SB CFRP wing
                    cCovers = 5.940e-4;
                    eMTOCovers = 1.309;
                    eWOverSCovers = -0.865;
                    eARCovers = 1.556;
                    eSweepCovers = -3.396;
                    eThickCovers = -1.054;
                    eVMOCovers = 0.434;
                    eTaperCovers = 0.218;
                    eLoadCovers = 0.658;
                    eEtaStrutCovers = 1.651;

                    cRibsWebs = 1.310e-1;
                    eMTORibsWebs = 1.398;
                    eWOverSRibsWebs = -1.039;
                    eARRibsWebs = 0.353;
                    eSweepRibsWebs = -1.005;
                    eThickRibsWebs = 0.224;
                    eVMORibsWebs = 0.280;
                    eTaperRibsWebs = 0.328;
                    eLoadRibsWebs = 0.443;
                    eEtaStrutRibsWebs = 0.818;

                    cStrutsJuries = 5.030e-6;
                    eMTOStrutsJuries = 1.660;
                    eWOverSStrutsJuries = -1.319;
                    eARStrutsJuries = 1.335;
                    eSweepStrutsJuries = -1.196;
                    eThickStrutsJuries = -0.696;
                    eVMOStrutsJuries = 0.854;
                    eTaperStrutsJuries = 0.414;
                    eLoadStrutsJuries = 0.898;
                    eEtaStrutStrutsJuries = -5.058;
                    eStrutParamStrutsJuries = 53.300;

                    cAileronEff = 0.0;
                    eMTOAileronEff = 0.0;
                    eWOverSAileronEff = 0.0;
                    eARAileronEff = 0.0;
                    eSweepAileronEff = 0.0;
                    eThickAileronEff = 0.0;
                    eVMOAileronEff = 0.0;
                    eTaperAileronEff = 0.0;
                    eLoadAileronEff = 0.0;
                    eEtaStrutAileronEff = 0.0;
                    eStrutParamAileronEff = 0.0;

                    if (wingData.getNWingMountedEngines() == 2)
                    {
                        kEngineReliefCovers = 0.988;
                        kEngineReliefRibsWebs = 0.991;
                        kEngineReliefStrutsJuries = 0.938;
                    }
                    else if (wingData.getNWingMountedEngines() == 4)
                    {
                        kEngineReliefCovers = 0.939;
                        kEngineReliefRibsWebs = 0.915;
                        kEngineReliefStrutsJuries = 0.858;
                    }
                    else
                    {
                        kEngineReliefCovers = 1.000;
                        kEngineReliefRibsWebs = 1.000;
                        kEngineReliefStrutsJuries = 1.000;
                    }
                }
                else
                {
                    // AS-SB CFRP wing
                    cCovers = 2.250 * std::pow(10, -3);
                    eMTOCovers = 1.351;
                    eWOverSCovers = -0.708;
                    eARCovers = 1.190;
                    eSweepCovers = -1.794;
                    eThickCovers = -0.724;
                    eVMOCovers = 0.020;
                    eTaperCovers = 0.603;
                    eLoadCovers = 0.886;
                    eEtaStrutCovers = 1.511;

                    cRibsWebs = 2.090 * std::pow(10, -1);
                    eMTORibsWebs = 1.435;
                    eWOverSRibsWebs = -0.954;
                    eARRibsWebs = 0.200;
                    eSweepRibsWebs = -0.702;
                    eThickRibsWebs = 0.340;
                    eVMORibsWebs = 0.016;
                    eTaperRibsWebs = 0.344;
                    eLoadRibsWebs = 0.686;
                    eEtaStrutRibsWebs = 0.726;

                    cStrutsJuries = 1.010 * std::pow(10, -3);
                    eMTOStrutsJuries = 1.556;
                    eWOverSStrutsJuries = -1.107;
                    eARStrutsJuries = 0.885;
                    eSweepStrutsJuries = -2.516;
                    eThickStrutsJuries = 0.056;
                    eVMOStrutsJuries = 0.106;
                    eTaperStrutsJuries = 1.307;
                    eLoadStrutsJuries = 1.148;
                    eEtaStrutStrutsJuries = -4.295;
                    eStrutParamStrutsJuries = 46.200;

                    cAileronEff = 4.640 * std::pow(10, 2);
                    eMTOAileronEff = -0.011;
                    eWOverSAileronEff = 0.423;
                    eARAileronEff = -0.342;
                    eSweepAileronEff = 2.380;
                    eThickAileronEff = 0.552;
                    eVMOAileronEff = -1.255;
                    eTaperAileronEff = -0.075;
                    eLoadAileronEff = 0.522;
                    eEtaStrutAileronEff = 1.640;
                    eStrutParamAileronEff = -2.634;

                    if (wingData.getNWingMountedEngines() == 2)
                    {
                        kEngineReliefCovers = 0.990;
                        kEngineReliefRibsWebs = 0.984;
                        kEngineReliefStrutsJuries = 0.945;
                    }
                    else if (wingData.getNWingMountedEngines() == 4)
                    {
                        kEngineReliefCovers = 0.953;
                        kEngineReliefRibsWebs = 0.912;
                        kEngineReliefStrutsJuries = 0.864;
                    }
                    else
                    {
                        kEngineReliefCovers = 1.000;
                        kEngineReliefRibsWebs = 1.000;
                        kEngineReliefStrutsJuries = 1.000;
                    }
                }
            }
            else
            {
                if (wingHalfBoxSweepAngle < 0.0)
                {
                    // FS CFRP wing
                    cCovers = 5.140 * std::pow(10, -5);
                    eMTOCovers = 1.391;
                    eWOverSCovers = -1.067;
                    eARCovers = 1.926;
                    eSweepCovers = -3.731;
                    eThickCovers = -1.400;
                    eVMOCovers = 0.694;
                    eTaperCovers = 0.672;
                    eLoadCovers = 0.467;
                    eEtaStrutCovers = 0.0;

                    cRibsWebs = 3.250 * std::pow(10, -2);
                    eMTORibsWebs = 1.423;
                    eWOverSRibsWebs = -0.991;
                    eARRibsWebs = 0.468;
                    eSweepRibsWebs = -0.734;
                    eThickRibsWebs = 0.021;
                    eVMORibsWebs = 0.330;
                    eTaperRibsWebs = 0.195;
                    eLoadRibsWebs = 0.198;
                    eEtaStrutRibsWebs = 0.0;

                    cStrutsJuries = 0.0;
                    eMTOStrutsJuries = 0.0;
                    eWOverSStrutsJuries = 0.0;
                    eARStrutsJuries = 0.0;
                    eSweepStrutsJuries = 0.0;
                    eThickStrutsJuries = 0.0;
                    eVMOStrutsJuries = 0.0;
                    eTaperStrutsJuries = 0.0;
                    eLoadStrutsJuries = 0.0;
                    eEtaStrutStrutsJuries = 0.0;
                    eStrutParamStrutsJuries = 0.0;

                    cAileronEff = 0.0;
                    eMTOAileronEff = 0.0;
                    eWOverSAileronEff = 0.0;
                    eARAileronEff = 0.0;
                    eSweepAileronEff = 0.0;
                    eThickAileronEff = 0.0;
                    eVMOAileronEff = 0.0;
                    eTaperAileronEff = 0.0;
                    eLoadAileronEff = 0.0;
                    eEtaStrutAileronEff = 0.0;
                    eStrutParamAileronEff = 0.0;

                    if (wingData.getNWingMountedEngines() == 2)
                    {
                        kEngineReliefCovers = 0.963;
                        kEngineReliefRibsWebs = 0.953;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                    else if (wingData.getNWingMountedEngines() == 4)
                    {
                        kEngineReliefCovers = 0.878;
                        kEngineReliefRibsWebs = 0.887;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                    else
                    {
                        kEngineReliefCovers = 1.000;
                        kEngineReliefRibsWebs = 1.000;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                }
                else
                {
                    // AS CFRP wing
                    cCovers = 1.170 * std::pow(10, -4);
                    eMTOCovers = 1.401;
                    eWOverSCovers = -0.638;
                    eARCovers = 1.445;
                    eSweepCovers = -1.245;
                    eThickCovers = -1.001;
                    eVMOCovers = 0.065;
                    eTaperCovers = 0.749;
                    eLoadCovers = 0.819;
                    eEtaStrutCovers = 0.0;

                    cRibsWebs = 1.630 * std::pow(10, -2);
                    eMTORibsWebs = 1.447;
                    eWOverSRibsWebs = -0.758;
                    eARRibsWebs = 0.265;
                    eSweepRibsWebs = -0.459;
                    eThickRibsWebs = 0.167;
                    eVMORibsWebs = 0.099;
                    eTaperRibsWebs = 0.149;
                    eLoadRibsWebs = 0.523;
                    eEtaStrutRibsWebs = 0.0;

                    cStrutsJuries = 0.0;
                    eMTOStrutsJuries = 0.0;
                    eWOverSStrutsJuries = 0.0;
                    eARStrutsJuries = 0.0;
                    eSweepStrutsJuries = 0.0;
                    eThickStrutsJuries = 0.0;
                    eVMOStrutsJuries = 0.0;
                    eTaperStrutsJuries = 0.0;
                    eLoadStrutsJuries = 0.0;
                    eEtaStrutStrutsJuries = 0.0;
                    eStrutParamStrutsJuries = 0.0;

                    cAileronEff = 1.600;
                    eMTOAileronEff = -0.006;
                    eWOverSAileronEff = 0.366;
                    eARAileronEff = -0.353;
                    eSweepAileronEff = 1.250;
                    eThickAileronEff = 0.305;
                    eVMOAileronEff = -0.446;
                    eTaperAileronEff = -0.100;
                    eLoadAileronEff = 0.117;
                    eEtaStrutAileronEff = 0.0;
                    eStrutParamAileronEff = 0.0;

                    if (wingData.getNWingMountedEngines() == 2)
                    {
                        kEngineReliefCovers = 0.985;
                        kEngineReliefRibsWebs = 0.969;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                    else if (wingData.getNWingMountedEngines() == 4)
                    {
                        kEngineReliefCovers = 0.914;
                        kEngineReliefRibsWebs = 0.909;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                    else
                    {
                        kEngineReliefCovers = 1.000;
                        kEngineReliefRibsWebs = 1.000;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                }
            }
        }
        else
        {
            // Aluminum wing configurations
            if (wingData.getIsBracedWing())
            {
                if (wingHalfBoxSweepAngle < 0.0)
                {
                    // FS-SB Al wing
                    cCovers = 1.120 * std::pow(10, -3);
                    eMTOCovers = 1.273;
                    eWOverSCovers = -0.871;
                    eARCovers = 1.573;
                    eSweepCovers = -3.743;
                    eThickCovers = -1.101;
                    eVMOCovers = 0.478;
                    eTaperCovers = -0.094;
                    eLoadCovers = 0.497;
                    eEtaStrutCovers = 1.563;

                    cRibsWebs = 4.360;
                    eMTORibsWebs = 1.308;
                    eWOverSRibsWebs = -1.173;
                    eARRibsWebs = 0.185;
                    eSweepRibsWebs = -1.232;
                    eThickRibsWebs = 0.435;
                    eVMORibsWebs = 0.245;
                    eTaperRibsWebs = -0.081;
                    eLoadRibsWebs = 0.175;
                    eEtaStrutRibsWebs = 0.513;

                    cStrutsJuries = 3.590 * std::pow(10, -6);
                    eMTOStrutsJuries = 1.662;
                    eWOverSStrutsJuries = -1.370;
                    eARStrutsJuries = 1.410;
                    eSweepStrutsJuries = -1.605;
                    eThickStrutsJuries = -0.772;
                    eVMOStrutsJuries = 0.944;
                    eTaperStrutsJuries = 0.412;
                    eLoadStrutsJuries = 0.865;
                    eEtaStrutStrutsJuries = -5.134;
                    eStrutParamStrutsJuries = 54.400;

                    cAileronEff = 0.0;
                    eMTOAileronEff = 0.0;
                    eWOverSAileronEff = 0.0;
                    eARAileronEff = 0.0;
                    eSweepAileronEff = 0.0;
                    eThickAileronEff = 0.0;
                    eVMOAileronEff = 0.0;
                    eTaperAileronEff = 0.0;
                    eLoadAileronEff = 0.0;
                    eEtaStrutAileronEff = 0.0;
                    eStrutParamAileronEff = 0.0;

                    if (wingData.getNWingMountedEngines() == 2)
                    {
                        kEngineReliefCovers = 0.989;
                        kEngineReliefRibsWebs = 0.991;
                        kEngineReliefStrutsJuries = 0.938;
                    }
                    else if (wingData.getNWingMountedEngines() == 4)
                    {
                        kEngineReliefCovers = 0.957;
                        kEngineReliefRibsWebs = 0.937;
                        kEngineReliefStrutsJuries = 0.865;
                    }
                    else
                    {
                        kEngineReliefCovers = 1.000;
                        kEngineReliefRibsWebs = 1.000;
                        kEngineReliefStrutsJuries = 1.000;
                    }
                }
                else
                {
                    // AS-SB Al wing
                    cCovers = 1.870 * std::pow(10, -2);
                    eMTOCovers = 1.231;
                    eWOverSCovers = -0.675;
                    eARCovers = 1.190;
                    eSweepCovers = -1.788;
                    eThickCovers = -0.812;
                    eVMOCovers = -0.020;
                    eTaperCovers = 0.186;
                    eLoadCovers = 0.371;
                    eEtaStrutCovers = 1.484;

                    cRibsWebs = 8.610;
                    eMTORibsWebs = 1.328;
                    eWOverSRibsWebs = -1.115;
                    eARRibsWebs = 0.009;
                    eSweepRibsWebs = -0.620;
                    eThickRibsWebs = 0.612;
                    eVMORibsWebs = 0.052;
                    eTaperRibsWebs = 0.111;
                    eLoadRibsWebs = 0.412;
                    eEtaStrutRibsWebs = 0.442;

                    cStrutsJuries = 1.010 * std::pow(10, -3);
                    eMTOStrutsJuries = 1.553;
                    eWOverSStrutsJuries = -1.098;
                    eARStrutsJuries = 0.849;
                    eSweepStrutsJuries = -2.467;
                    eThickStrutsJuries = 0.018;
                    eVMOStrutsJuries = 0.098;
                    eTaperStrutsJuries = 1.163;
                    eLoadStrutsJuries = 1.123;
                    eEtaStrutStrutsJuries = -4.386;
                    eStrutParamStrutsJuries = 46.100;

                    cAileronEff = 5.700 * std::pow(10, 2);
                    eMTOAileronEff = -0.062;
                    eWOverSAileronEff = 0.456;
                    eARAileronEff = -0.460;
                    eSweepAileronEff = 2.115;
                    eThickAileronEff = 0.512;
                    eVMOAileronEff = -1.270;
                    eTaperAileronEff = -0.299;
                    eLoadAileronEff = 0.296;
                    eEtaStrutAileronEff = 1.064;
                    eStrutParamAileronEff = -1.973;

                    if (wingData.getNWingMountedEngines() == 2)
                    {
                        kEngineReliefCovers = 0.996;
                        kEngineReliefRibsWebs = 0.990;
                        kEngineReliefStrutsJuries = 0.947;
                    }
                    else if (wingData.getNWingMountedEngines() == 4)
                    {
                        kEngineReliefCovers = 0.969;
                        kEngineReliefRibsWebs = 0.944;
                        kEngineReliefStrutsJuries = 0.866;
                    }
                    else
                    {
                        kEngineReliefCovers = 1.000;
                        kEngineReliefRibsWebs = 1.000;
                        kEngineReliefStrutsJuries = 1.000;
                    }
                }
            }
            else
            {
                if (wingHalfBoxSweepAngle < 0.0)
                {
                    // FS Al wing
                    cCovers = 2.250 * std::pow(10, -5);
                    eMTOCovers = 1.367;
                    eWOverSCovers = -1.149;
                    eARCovers = 2.158;
                    eSweepCovers = -5.421;
                    eThickCovers = -1.550;
                    eVMOCovers = 0.948;
                    eTaperCovers = 0.738;
                    eLoadCovers = 0.385;
                    eEtaStrutCovers = 0.0;

                    cRibsWebs = 2.340 * std::pow(10, -1);
                    eMTORibsWebs = 1.401;
                    eWOverSRibsWebs = -1.112;
                    eARRibsWebs = 0.348;
                    eSweepRibsWebs = -0.643;
                    eThickRibsWebs = 0.200;
                    eVMORibsWebs = 0.335;
                    eTaperRibsWebs = 0.135;
                    eLoadRibsWebs = 0.169;
                    eEtaStrutRibsWebs = 0.0;

                    cStrutsJuries = 0.0;
                    eMTOStrutsJuries = 0.0;
                    eWOverSStrutsJuries = 0.0;
                    eARStrutsJuries = 0.0;
                    eSweepStrutsJuries = 0.0;
                    eThickStrutsJuries = 0.0;
                    eVMOStrutsJuries = 0.0;
                    eTaperStrutsJuries = 0.0;
                    eLoadStrutsJuries = 0.0;
                    eEtaStrutStrutsJuries = 0.0;
                    eStrutParamStrutsJuries = 0.0;

                    cAileronEff = 0.0;
                    eMTOAileronEff = 0.0;
                    eWOverSAileronEff = 0.0;
                    eARAileronEff = 0.0;
                    eSweepAileronEff = 0.0;
                    eThickAileronEff = 0.0;
                    eVMOAileronEff = 0.0;
                    eTaperAileronEff = 0.0;
                    eLoadAileronEff = 0.0;
                    eEtaStrutAileronEff = 0.0;
                    eStrutParamAileronEff = 0.0;

                    if (wingData.getNWingMountedEngines() == 2)
                    {
                        kEngineReliefCovers = 0.962;
                        kEngineReliefRibsWebs = 0.956;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                    else if (wingData.getNWingMountedEngines() == 4)
                    {
                        kEngineReliefCovers = 0.885;
                        kEngineReliefRibsWebs = 0.907;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                    else
                    {
                        kEngineReliefCovers = 1.000;
                        kEngineReliefRibsWebs = 1.000;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                }
                else
                {
                    // AS Al wing
                    cCovers = 1.180 * std::pow(10, -3);
                    eMTOCovers = 1.305;
                    eWOverSCovers = -0.662;
                    eARCovers = 1.464;
                    eSweepCovers = -1.718;
                    eThickCovers = -1.000;
                    eVMOCovers = 0.036;
                    eTaperCovers = 0.367;
                    eLoadCovers = 0.314;
                    eEtaStrutCovers = 0.0;

                    cRibsWebs = 2.050 * std::pow(10, -1);
                    eMTORibsWebs = 1.410;
                    eWOverSRibsWebs = -0.892;
                    eARRibsWebs = 0.122;
                    eSweepRibsWebs = -0.379;
                    eThickRibsWebs = 0.339;
                    eVMORibsWebs = 0.080;
                    eTaperRibsWebs = -0.013;
                    eLoadRibsWebs = 0.392;
                    eEtaStrutRibsWebs = 0.0;

                    cStrutsJuries = 0.0;
                    eMTOStrutsJuries = 0.0;
                    eWOverSStrutsJuries = 0.0;
                    eARStrutsJuries = 0.0;
                    eSweepStrutsJuries = 0.0;
                    eThickStrutsJuries = 0.0;
                    eVMOStrutsJuries = 0.0;
                    eTaperStrutsJuries = 0.0;
                    eLoadStrutsJuries = 0.0;
                    eEtaStrutStrutsJuries = 0.0;
                    eStrutParamStrutsJuries = 0.0;

                    cAileronEff = 3.360;
                    eMTOAileronEff = -0.036;
                    eWOverSAileronEff = 0.446;
                    eARAileronEff = -0.467;
                    eSweepAileronEff = 1.590;
                    eThickAileronEff = 0.375;
                    eVMOAileronEff = -0.556;
                    eTaperAileronEff = -0.187;
                    eLoadAileronEff = 0.018;
                    eEtaStrutAileronEff = 0.0;
                    eStrutParamAileronEff = 0.0;

                    if (wingData.getNWingMountedEngines() == 2)
                    {
                        kEngineReliefCovers = 0.988;
                        kEngineReliefRibsWebs = 0.975;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                    else if (wingData.getNWingMountedEngines() == 4)
                    {
                        kEngineReliefCovers = 0.929;
                        kEngineReliefRibsWebs = 0.930;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                    else
                    {
                        kEngineReliefCovers = 1.000;
                        kEngineReliefRibsWebs = 1.000;
                        kEngineReliefStrutsJuries = 0.0;
                    }
                }
            }
        }

        // Calcolo etaAileron
        double wingLoading = wingData.getWTO() * 9.81 / wingPlanformArea;
        double cosSweep = std::cos(wingHalfBoxSweepAngle / 57.3); // conversione da gradi a radianti

        double etaAileron = cAileronEff *
                            std::pow(wingData.getWTO(), eMTOAileronEff) *
                            std::pow(wingLoading, eWOverSAileronEff) *
                            std::pow(wingAspectRatio, eARAileronEff) *
                            std::pow(cosSweep, eSweepAileronEff) *
                            std::pow(wingData.getWingKinkThicknessRatio(), eThickAileronEff) *
                            std::pow(wingData.getMaxOperatingEAS(), eVMOAileronEff) *
                            std::pow(1 + wingTaperRatio, eTaperAileronEff) *
                            std::pow(wingData.getNUltimateLoad(), eLoadAileronEff) *
                            std::pow(1 - wingData.getWingStrutPosition(), eEtaStrutAileronEff) *
                            std::pow(strutsParameter, eStrutParamAileronEff);

        // Calcolo kAileron
        double kAileron;
        if (wingHalfBoxSweepAngle < 0.0)
        {
            kAileron = 1.0;
        }
        else
        {
            if (etaAileron < 0.5)
            {
                kAileron = std::pow(etaAileron / 0.5, -1.1);
            }
            else
            {
                kAileron = 1.0;
            }
        }

        // Coefficiente struttura secondaria
        // condizione ? valore_se_vero : valore_se_falso
        double cSecondary = wingData.getIsSimplifiedFlapSystem() ? 0.0338 : 0.0443;

        // Calcolo pesi componenti
        double weightCovers = kEngineReliefCovers * cCovers *
                              std::pow(wingData.getWTO(), eMTOCovers) *
                              std::pow(wingLoading, eWOverSCovers) *
                              std::pow(wingAspectRatio, eARCovers) *
                              std::pow(cosSweep, eSweepCovers) *
                              std::pow(wingData.getWingKinkThicknessRatio(), eThickCovers) *
                              std::pow(wingData.getMaxOperatingEAS(), eVMOCovers) *
                              std::pow(1 + wingTaperRatio, eTaperCovers) *
                              std::pow(wingData.getNUltimateLoad(), eLoadCovers) *
                              std::pow(1 - wingData.getWingStrutPosition(), eEtaStrutCovers);

        double weightRibsWebs = kEngineReliefRibsWebs * cRibsWebs *
                                std::pow(wingData.getWTO(), eMTORibsWebs) *
                                std::pow(wingLoading, eWOverSRibsWebs) *
                                std::pow(wingAspectRatio, eARRibsWebs) *
                                std::pow(cosSweep, eSweepRibsWebs) *
                                std::pow(wingData.getWingKinkThicknessRatio(), eThickRibsWebs) *
                                std::pow(wingData.getMaxOperatingEAS(), eVMORibsWebs) *
                                std::pow(1 + wingTaperRatio, eTaperRibsWebs) *
                                std::pow(wingData.getNUltimateLoad(), eLoadRibsWebs) *
                                std::pow(1 - wingData.getWingStrutPosition(), eEtaStrutRibsWebs);

        double weightStrutsJuries = kEngineReliefStrutsJuries * cStrutsJuries *
                                    std::pow(wingData.getWTO(), eMTOStrutsJuries) *
                                    std::pow(wingLoading, eWOverSStrutsJuries) *
                                    std::pow(wingAspectRatio, eARStrutsJuries) *
                                    std::pow(cosSweep, eSweepStrutsJuries) *
                                    std::pow(wingData.getWingKinkThicknessRatio(), eThickStrutsJuries) *
                                    std::pow(wingData.getMaxOperatingEAS(), eVMOStrutsJuries) *
                                    std::pow(1 + wingTaperRatio, eTaperStrutsJuries) *
                                    std::pow(wingData.getNUltimateLoad(), eLoadStrutsJuries) *
                                    std::pow(1 - wingData.getWingStrutPosition(), eEtaStrutStrutsJuries) *
                                    std::pow(strutsParameter, eStrutParamStrutsJuries);

        double weightSecondaryStructure = cSecondary * wingData.getWTO();

        // Peso totale dell'ala
        wingWeight = kAileron * (weightCovers + weightRibsWebs) +
                     weightSecondaryStructure +
                     weightStrutsJuries;

        return wingWeight;
    }

    // ================= HORIZONTAL WEIGHT METHODS =================

    /// @brief Calculates the horizontal tail weight using Torenbeek method (in kg)
    /// @param aircraftEngineType The type of engine configuration of the aircraft
    /// @param nUltimateLoadFactor The positive ultimate load factor
    /// @param diveSpeed The dive speed of the aircraft (in m/s)
    /// @param planformArea The planform area of the horizontal tail (in m²)
    /// @param sweepC2 The sweep angle at the 50% chord line (in degrees)
    /// @param typeStabilizer The type of stabilizer (fixed or unfixed)
    /// @return The estimated weight of the horizontal tail (in kg)
    double horizontalTailWeightTorenbeek(WingWeightData horizontalData, double planformArea, double sweepC2)
    {

        switch (horizontalData.getAircraftEngineType())
        {
        case AircraftEngineType::SINGLE_ENGINE:
        case AircraftEngineType::TWIN_ENGINE:

            horizontalTailWeight = (0.64 * std::pow(horizontalData.getNUltimateLoad() * std::pow(planformArea, 2), 0.75));

            break;

        case AircraftEngineType::JET:
        case AircraftEngineType::MULTI_PROPELLER_ENGINE:

            if (horizontalData.getTypeOfStabilizer() == TypeOfStabilizer::FIXED)
            {

                Kh = 1.0;
            }

            else
            {

                Kh = 1.1;
            }

            std::vector<double> X_DATA = {
                0.2102, 0.2208, 0.2330, 0.2474, 0.2626, 0.2801, 0.2961, 0.3075,
                0.3227, 0.3379, 0.3600, 0.3851, 0.3987, 0.4086, 0.4231, 0.4345,
                0.4474, 0.4611, 0.4770, 0.4877, 0.5028, 0.5248, 0.5430, 0.5605,
                0.5749, 0.5893, 0.6074, 0.6203, 0.6347, 0.6453};

            std::vector<double> Y_DATA = {
                9.9352, 10.6116, 11.2873, 12.2459, 13.2041, 14.2749, 15.1759, 15.9088,
                16.8670, 17.8252, 19.1209, 20.6992, 21.4309, 22.1077, 23.0095, 23.6856,
                24.3609, 25.0358, 25.7096, 26.2156, 26.7761, 27.5036, 28.1194, 28.5084,
                28.7852, 29.0052, 29.3369, 29.5009, 29.5504, 29.4883};

            ratioTorenbeekFncToHor = (std::pow(planformArea, 0.2) * horizontalData.getDiveSpeed()) / (1000 * std::pow(std::cos(sweepC2 / 57.3), 0.5));

            Interpolant interp1(X_DATA, Y_DATA, 1, RegressionMethod::LINEAR);

            weightToPlanformSurfaceRatioToHor = interp1.getYValueFromRegression(ratioTorenbeekFncToHor);

            horizontalTailWeight = Kh * weightToPlanformSurfaceRatioToHor * planformArea;

            break;
        }

        return horizontalTailWeight;
    }

    // ================= VERTICAL WEIGHT METHODS =================

    /// @brief Calculates the vertical tail weight using Torenbeek method (in kg)
    /// @param aircraftEngineType The type of engine configuration of the aircraft
    /// @param typeTail The type of tail configuration (CONVENTIONAL_TAIL, T_TAIL, V_TAIL, V_REV_TAIL, H_TAIL, U_TAIL)
    /// @param nUltimateLoadFactor The positive ultimate load factor
    /// @param diveSpeed The dive speed of the aircraft (in m/s)
    /// @param sweepC2 The sweep angle at the 50% chord line (in degrees)
    /// @param span The span of the vertical tail (in m)
    /// @param planformArea The planform area of the vertical tail (in m²)
    /// @param zHorPosition The vertical position of the horizontal tail relative to the fuselage centerline (in m)
    /// @param horSurface The area of the horizontal tail (in m²)
    /// @return The estimated weight of the vertical tail (in kg)
    double verticalTailWeightTorenbeek(WingWeightData verticalData, double span, double planformArea,
                                       double sweepC2, double zHorPosition, double horSurface)
    {

        switch (verticalData.getAircraftEngineType())
        {

        case AircraftEngineType::SINGLE_ENGINE:
        case AircraftEngineType::TWIN_ENGINE:

            verticalTailWeight = (0.64 * std::pow((verticalData.getNUltimateLoad() * std::pow(planformArea, 2)), 0.75));

            break;

        case AircraftEngineType::JET:
        case AircraftEngineType::MULTI_PROPELLER_ENGINE:

            if (verticalData.getTypeOfTail() == TypeOfTail::CONVENTIONAL_TAIL)
            {

                Kv = 1;
            }

            else
            {

                Kv = 1 + 0.15 * (horSurface * zHorPosition) / (planformArea * span);
            }

            std::vector<double> X_DATA = {
                0.2102, 0.2208, 0.2330, 0.2474, 0.2626, 0.2801, 0.2961, 0.3075,
                0.3227, 0.3379, 0.3600, 0.3851, 0.3987, 0.4086, 0.4231, 0.4345,
                0.4474, 0.4611, 0.4770, 0.4877, 0.5028, 0.5248, 0.5430, 0.5605,
                0.5749, 0.5893, 0.6074, 0.6203, 0.6347, 0.6453};

            std::vector<double> Y_DATA = {
                9.9352, 10.6116, 11.2873, 12.2459, 13.2041, 14.2749, 15.1759, 15.9088,
                16.8670, 17.8252, 19.1209, 20.6992, 21.4309, 22.1077, 23.0095, 23.6856,
                24.3609, 25.0358, 25.7096, 26.2156, 26.7761, 27.5036, 28.1194, 28.5084,
                28.7852, 29.0052, 29.3369, 29.5009, 29.5504, 29.4883};

            ratioTorenbeekFncToVer = (std::pow(planformArea, 0.2) * verticalData.getDiveSpeed()) / (1000 * std::pow(std::cos(sweepC2 / 57.3), 0.5));

            Interpolant interp1(X_DATA, Y_DATA, 1, RegressionMethod::LINEAR);

            weightToPlanformSurfaceRatioToVer = interp1.getYValueFromRegression(ratioTorenbeekFncToVer);
            verticalTailWeight = Kv * weightToPlanformSurfaceRatioToVer * planformArea;
        }

        return verticalTailWeight;
    }

    // ================= FUSELAGE WEIGHT METHODS =================

    /// @brief Calculates the fuselage weight using Torenbeek method (in kg)
    /// @param diveSpeed Dive speed (in m/s)
    /// @param tailArm Distance between quarter chord points of wing root and horizontal tail root (m)
    /// @param maxFuseWidth Maximum fuselage width (in m)
    /// @param maxFuseDia Maximum fuselage diameter (in m)
    /// @param fuseWettedArea Fuselage wetted area (in m²)
    /// @param isPressurized Whether the fuselage is pressurized
    /// @param enginePosition Engine position configuration
    /// @param undercarriagePosition Undercarriage position configuration
    /// @return The estimated fuselage weight (in kg)
    double fuselageWeightTorenbeek(FuselageWeightData fuselageData, double tailArm, double maxFuseWidth, double maxFuseDia,
                                   double fuseWettedArea)
    {

        fuselageWeight = Kwf * std::sqrt(fuselageData.getDiveSpeed() * (tailArm / (maxFuseDia + maxFuseWidth))) * std::pow(fuseWettedArea, 1.2);

        // Increment due to the pressurization

        if (fuselageData.getIsPressurized())
        {

            fuselageWeight *= 1.08;
        }

        // Increment due to the engine position

        if (fuselageData.getEnginePosition() == EnginePosition::BODY_MOUNTED)
        {

            fuselageWeight *= 1.04;
        }

        // Increment due to the undercarriage position

        if (fuselageData.getUndercarriagePosition() == UndercarriagePosition::BODY_MOUNTED)
        {

            fuselageWeight *= 1.07;
        }

        return fuselageWeight;
    }

    /// @brief Calculates the fuselage weight using Torenbeek method, using materials (in kg)
    /// @param diveSpeed Dive speed (in m/s)
    /// @param tailArm Distance between quarter chord points of wing root and horizontal tail root (m)
    /// @param maxFuseWidth Maximum fuselage width (in m)
    /// @param maxFuseDia Maximum fuselage diameter (in m)
    /// @param fuseWettedArea Fuselage wetted area (in m²)
    /// @param isPressurized Whether the fuselage is pressurized
    /// @param enginePosition Engine position configuration
    /// @param undercarriagePosition Undercarriage position configuration
    /// @param nUltimateLoadFactor The positive ultimate load factor
    /// @param isCargo Whether the aircraft is a cargo configuration
    /// @param framesMaterial Material of the fuselage frames
    /// @param stringersMaterial Material of the fuselage stringers
    /// @param skinMaterial Material of the fuselage skin
    /// @return The estimated fuselage weight (in kg)
    double fuselgeWeightTorenbeekMaterial(FuselageWeightData fuselageData, double tailArm, double maxFuseWidth, double maxFuseDia,
                                          double fuseWettedArea)

    {

        // Basic fuselage alluminium weight

        fuselageWeight = fuselageWeightTorenbeek(fuselageData, tailArm, maxFuseWidth, maxFuseDia, fuseWettedArea);

        // Frames density ratio

        switch (fuselageData.getFrameMaterial())
        {
        case Material::ALLUMINIUM:

            frameDensityRatio = 1.0;
            break;

        case Material::TITANIUM:

            frameDensityRatio = 1.5843;
            break;

        case Material::CARBON_FIBER:

            frameDensityRatio = 0.5635;
            break;

        case Material::GLASS_FIBER:

            frameDensityRatio = 0.6530;
            break;

        case Material::KEVLAR:

            frameDensityRatio = 0.5152;
            break;
        }

        // Skin density ratio

        switch (fuselageData.getSkinMaterial())
        {
        case Material::ALLUMINIUM:

            skinDensityRatio = 1.0;
            break;

        case Material::TITANIUM:

            skinDensityRatio = 1.5843;
            break;

        case Material::CARBON_FIBER:

            skinDensityRatio = 0.5635;
            break;

        case Material::GLASS_FIBER:

            skinDensityRatio = 0.6530;
            break;

        case Material::KEVLAR:

            skinDensityRatio = 0.5152;
            break;
        }

        // Stringers density ratio

        switch (fuselageData.getStringersMaterial())
        {
        case Material::ALLUMINIUM:

            stringersDensityRatio = 1.0;
            break;

        case Material::TITANIUM:

            stringersDensityRatio = 1.5843;
            break;

        case Material::CARBON_FIBER:

            stringersDensityRatio = 0.5635;
            break;

        case Material::GLASS_FIBER:

            stringersDensityRatio = 0.6530;
            break;

        case Material::KEVLAR:

            stringersDensityRatio = 0.5152;
            break;
        }

        // Skin panel mass alluminium

        kLambda = 0.56 * std::pow((tailArm / (maxFuseWidth + maxFuseDia)), 3 / 4);

        double factorfromKLambda = tailArm / (maxFuseWidth + maxFuseDia);

        if (factorfromKLambda > 2.61)
        {

            kLambda = 1.15;
        }

        // Weight of skin panels
        weightSkinPanels = 0.05428 * kLambda * (std::pow(fuseWettedArea, 1.07)) * (std::pow(fuselageData.getDiveSpeed(), 0.743));

        // Weight of stringers
        weightStringers = 0.0117 * kLambda * (std::pow(fuseWettedArea, 1.45)) * (std::pow(fuselageData.getDiveSpeed(), 0.39)) * (std::pow(fuselageData.getNUltimateLoad(), 0.316));

        // Weight of frames

        if (fuselageData.getIsCargo())
        {

            weightFrames = 0.32 * (weightSkinPanels + weightStringers);
        }

        else if ((weightSkinPanels + weightStringers) > 286)
        {

            weightFrames = 0.19 * (weightSkinPanels + weightStringers);
        }

        else
        {

            weightFrames = 0.09111 * (weightSkinPanels + weightStringers);
        }

        alluminiumFactor = fuselageWeight / (weightSkinPanels + weightStringers + weightFrames);

        // Corrected weight for materials

        fuselageWeight = alluminiumFactor * (frameDensityRatio * weightFrames +
                                             stringersDensityRatio * weightStringers +
                                             skinDensityRatio * weightSkinPanels);

        return fuselageWeight;
    }

    // ================= TAIL BOOM WEIGHT METHODS =================

    double tailBoomWeightRoskam(BaseWeightData boomData, double boomWidth, double boomHeight, double boomWettedArea, double tailArm)
    {

        if (boomData.getHasBoom())
        {

            ConvLength convLengthToTailArm(Length::M, Length::FT, tailArm);

            tailArmConvertedInFeet = convLengthToTailArm.getConvertedValues();

            ConvLength convLengthToSumWidthAndHeightBoom(Length::M, Length::FT, (boomWidth + boomHeight));

            sumWidthAndHeightBoomInFeet = convLengthToSumWidthAndHeightBoom.getConvertedValues();

            ConvArea convAreaToBoomWettedArea(Area::SQUARE_METER, Area::SQUARE_FEET, boomWettedArea);

            boomWettedAreaConvertedInSqFeet = convAreaToBoomWettedArea.getConvertedValues();

            ConvVel convVelToDiveSpeed(Speed::M_TO_S, Speed::KTS, boomData.getDiveSpeed());

            diveSpeedConvertedInKnots = convVelToDiveSpeed.getConvertedValues();

            tailBoomWeightEstimated = boomData.getNumberOfBooms() * (0.021 * std::pow(diveSpeedConvertedInKnots * tailArmConvertedInFeet / (sumWidthAndHeightBoomInFeet), 0.5)) *
                                      std::pow(boomWettedAreaConvertedInSqFeet, 0.5);

            ConvMass convMassToKg(Mass::LB, Mass::KG, tailBoomWeightEstimated);

            tailBoomWeightEstimated = convMassToKg.getConvertedValues();
        }

        return tailBoomWeightEstimated;
    }

    // ================= EO/IR WEIGHT METHODS =================

    double eoirWeightGundlach(BaseWeightData eoirData, double eoirDiameter, double eoirFinenessRatio)
    {

        if (eoirData.getHasEOIR())
        {
            eoirWeightEstimated = factorToEOIRWeight * std::pow((factorToDiamEoir * eoirDiameter), 3) * eoirFinenessRatio;

            ConvMass convMassToKg(Mass::LB, Mass::KG, eoirWeightEstimated);

            eoirWeightEstimated = convMassToKg.getConvertedValues();
        }

        return eoirWeightEstimated;
    }

    // ================= LANDING GEAR WEIGHT METHODS =================

    /// @brief Calculates the landing gear weight using Torenbeek method (in kg)
    /// @param WTO Maximum take-off weight (in kg)
    /// @param aircraftCategory Aircraft category (TRANSPORT_JET, GENERAL_AVIATION, UAV)
    /// @param wingPosition Z-Position of the wing (LOW_WING, MID_WING, HIGH_WING)
    /// @param isRetractableGerar Whether the landing gear is retractable
    /// @return The estimated landing gear weight (in kg)
    double landingGearWeightTorenbeek(BaseWeightData landingGearData)
    {

        if (landingGearData.getAircraftCategory() != AircraftCategory::UAV)
        {

            if (landingGearData.getWingPosition() == WingPosition::LOW_WING)
            {

                kUC = 1.0;
            }

            else if (landingGearData.getWingPosition() == WingPosition::MID_WING)
            {

                kUC = 0.5 * (1.08 + 1.0);
            }

            else
            {

                kUC = 1.08;
            }

            if (landingGearData.getAircraftCategory() == AircraftCategory::TRANSPORT_JET)
            {

                Amain = 15.0;
                Anose = 5.4;
                Bmain = 0.033;
                Bnose = 0.049;
                Cmain = 0.021;
                Cnose = 0.0;
                Dmain = 0.0;
                Dnose = 0.0;

                lndMainWeight = kUC * (Amain + Bmain * std::pow(landingGearData.getWTO(), 3 / 4) + Cmain * landingGearData.getWTO() + Dmain * std::pow(landingGearData.getWTO(), 1.5));
                lndNoseWeight = kUC * (Anose + Bnose * std::pow(landingGearData.getWTO(), 3 / 4) + Cnose * landingGearData.getWTO() + Dnose * std::pow(landingGearData.getWTO(), 1.5));

                undercarriageWeight = lndMainWeight + lndNoseWeight;
            }

            else if (landingGearData.getIsRetractableLandingGear())
            {

                Amain = 18.1;
                Anose = 9.1;
                Bmain = 0.131;
                Bnose = 0.082;
                Cmain = 0.019;
                Cnose = 0.0;
                Dmain = 2.23 * std::pow(10, -5);
                Dnose = 2.97 * std::pow(10, -6);
            }

            else
            {

                Amain = 9.1;
                Anose = 11.3;
                Bmain = 0.082;
                Bnose = 0.0;
                Cmain = 0.019;
                Cnose = 0.0024;
                Dmain = 0.0;
                Dnose = 0.0;
            }

            lndMainWeight = kUC * (Amain + Bmain * std::pow(landingGearData.getWTO(), 3 / 4) + Cmain * landingGearData.getWTO() + Dmain * std::pow(landingGearData.getWTO(), 1.5));
            lndNoseWeight = kUC * (Anose + Bnose * std::pow(landingGearData.getWTO(), 3 / 4) + Cnose * landingGearData.getWTO() + Dnose * std::pow(landingGearData.getWTO(), 1.5));

            undercarriageWeight = lndMainWeight + lndNoseWeight;
        }

        else
        {

            kLG = 0.5 * (0.03 + 0.06);
            undercarriageWeight = kLG * landingGearData.getWTO();
            lndMainWeight = factorLngMainToUAV * undercarriageWeight;
            lndNoseWeight = (1 - factorLngMainToUAV) * undercarriageWeight;
        }

        return undercarriageWeight;
    }

    /// @brief Gets the main landing gear weight calculated using Torenbeek method (in kg)
    /// @return The main landing gear weight (in kg)
    double getMainLandingGearWeightTorenbeek()
    {
        return lndMainWeight;
    }

    /// @brief Gets the nose landing gear weight calculated using Torenbeek method (in kg)
    /// @return The nose landing gear weight (in kg)
    double getNoseLandingGearWeightTorenbeek()
    {
        return lndNoseWeight;
    }

    // ================= CONTROL SURFACES WEIGHT METHODS =================

    /// @brief Calculates the control surface weight using Torenbeek method (in kg)
    /// @param WTO Maximum take-off weight (in kg)
    /// @param kSC Control surface factor (Ksc = 0.23 to flight airplanes without duplicated system controls - Ksc = 0.44 to transport airplanes, manually controlled - Ksc = 0.64 to transport airplanes, with powered controls and TED high-lift devices)
    double contrsolSurfaceWeightTorenbeek(BaseWeightData controlSurfaceData)
    {

        controlSurfaceWeight = controlSurfaceData.getFactorToControlSurface() * 0.768 * std::pow(controlSurfaceData.getWTO(), 0.67);

        return controlSurfaceWeight;
    }

    // ================= PROPULSION GROUP WEIGHT METHODS =================

    /// @brief Calculates the propulsion group weight using Torenbeek method (in kg)
    /// @param aircraftCategory The category of the aircraft
    /// @param numberOfEngines The number of engines
    /// @param thrustToPowerRatio The thrust to power ratio
    /// @param thrustLbf The thrust in pounds-force
    /// @param BSHP Brake shaft horsepower (Hp)
    /// @return The estimated propulsion group weight (in kg)
    double propulsionGroupWeight(EngineWeightData engineData)
    {

        if (engineData.getAircraftEngineType() == AircraftEngineType::JET)
        {

            singleEngineWeight = 2.7 * std::pow(engineData.getThrustLbf(), 0.75);

            ConvMass convMassToKg(Mass::LB, Mass::KG, singleEngineWeight);

            singleEngineWeight = convMassToKg.getConvertedValues();

            engineGroupWeight = engineData.getKPG() * engineData.getKTHR() * engineData.getNumberOfEngines() * singleEngineWeight;

            if (engineData.getHasWaterInjectionSystem())
            {

                engineGroupWeight *= 1.015;
            }
        }

        else if (engineData.getAircraftEngineType() == AircraftEngineType::TWIN_ENGINE ||
                 engineData.getAircraftEngineType() == AircraftEngineType::SINGLE_ENGINE ||
                 engineData.getAircraftEngineType() == AircraftEngineType::MULTI_PROPELLER_ENGINE)
        {

            engineData.setThrustLbf(engineData.getThrustToPowerRatio() * engineData.getBSHP());

            singleEngineWeight = 2.7 * std::pow(engineData.getThrustLbf(), 0.75);

            ConvMass convMassToKg(Mass::LB, Mass::KG, singleEngineWeight);

            singleEngineWeight = convMassToKg.getConvertedValues();

            engineGroupWeight = engineData.getKPG() * engineData.getNumberOfEngines() * (singleEngineWeight + 0.109*engineData.getBSHP());

             if (engineData.getHasWaterInjectionSystem())
            {

                engineGroupWeight *= 1.03;
            }
        }

        return engineGroupWeight;
    }

    // ================= NACELLE WEIGHT METHODS =================

    /// @brief Claculates the nacelle weight (in kg)
    /// @param numberOfEngines The number of engines
    /// @param nacWettedSurface The nacelle wetted surface area (in square meters)
    /// @param thrustLbf The thrust in pounds-force
    /// @return Calculated nacelle weight (in kg)
    double nacelleWeight(EngineWeightData engineData, double nacWettedSurface)
    {


        if (engineData.getAircraftEngineType() == AircraftEngineType::JET)
        {

        singleEngineWeight = 2.7 * std::pow(engineData.getThrustLbf(), 0.75);

        nacelleGroupWeight = 35.45 * engineData.getNumberOfEngines() * std::pow((2.33 * (1.1 * singleEngineWeight) * (nacWettedSurface / std::pow(0.3048, 2.0))) / 10000.0, 0.59);

        ConvMass massConverter(Mass::LB, Mass::KG, nacelleGroupWeight);

        nacelleGroupWeight = massConverter.getConvertedValues();

        }

        else if (engineData.getAircraftEngineType() == AircraftEngineType::TWIN_ENGINE ||
                 engineData.getAircraftEngineType() == AircraftEngineType::SINGLE_ENGINE ||
                 engineData.getAircraftEngineType() == AircraftEngineType::MULTI_PROPELLER_ENGINE)
        {

            engineData.setThrustLbf(engineData.getThrustToPowerRatio() * engineData.getBSHP());

            singleEngineWeight = 2.7 * std::pow(engineData.getThrustLbf(), 0.75);

            nacelleGroupWeight = 35.45 * engineData.getNumberOfEngines() * std::pow((2.33 * (1.1 * singleEngineWeight) * (nacWettedSurface / std::pow(0.3048, 2.0))) / 10000.0, 0.59);

            ConvMass massConverter(Mass::LB, Mass::KG, nacelleGroupWeight);

            nacelleGroupWeight = massConverter.getConvertedValues();
        }
        
        return nacelleGroupWeight;
    }

    // ================= APU WEIGHT METHODS =================

    /// @brief Claculates the APU weight using Torenbeek method (in kg)
    /// @param WTO Maximum take-off weight (in kg)
    /// @return Calculated APU weight (in kg)
    double apuWeight(BaseWeightData apuData)
    {

        if (apuData.getHasAPU())
        {
            ConvMass massConverter(Mass::KG, Mass::LB, apuData.getWTO());

            apuWeightEstimated = 8 * std::pow(apuData.getWTO(), 0.4);

            ConvMass convMassToKg(Mass::LB, Mass::KG, apuWeightEstimated);

            apuWeightEstimated = convMassToKg.getConvertedValues();
        }

        return apuWeightEstimated;
    }

    // ================= INSTRUMENTATION WEIGHT METHODS =================

    /// @brief Calculates the instrumentation weight(in kg)
    /// @param WTO The maximum take-off weight (in kg)
    /// @return The estimated instrumentation weight (in kg)
    double instrumentWeight(BaseWeightData instrumentData)
    {

        ConvMass massConverter(Mass::KG, Mass::LB, instrumentData.getWTO());

        instrumentationWeight = 200 + 0.003 * instrumentData.getWTO();

        ConvMass convMassToKg(Mass::LB, Mass::KG, instrumentationWeight);

        instrumentationWeight = convMassToKg.getConvertedValues();

        return instrumentationWeight;
    }

    // ================= HYDRAULIC SYSTEM WEIGHT METHODS =================

    /// @brief Calculates the hydraulic system weight (in kg)
    /// @param WTO The maximum take-off weight (in kg)
    /// @return The estimated hydraulic system weight (in kg)
    double hydraulicSystemWeight(BaseWeightData hydraulicData)
    {

        ConvMass massConverter(Mass::KG, Mass::LB, hydraulicData.getWTO());

        hydraulicSystemWeightEstimated = 0.1 * std::pow(hydraulicData.getWTO(), 0.8);

        ConvMass convMassToKg(Mass::LB, Mass::KG, hydraulicSystemWeightEstimated);

        hydraulicSystemWeightEstimated = convMassToKg.getConvertedValues();

        return hydraulicSystemWeightEstimated;
    }

    // ================= ELECTRICAL GROUP WEIGHT METHODS =================

    /// @brief Calculates the electrical group weight (in kg)
    /// @param WTO The maximum take-off weight (in kg)
    /// @return The estimated electrical group weight (in kg)
    double electricalGroupWeight(BaseWeightData electricalData)
    {

        ConvMass massConverter(Mass::KG, Mass::LB, electricalData.getWTO());

        electricalSystemWeightEstimated = 9.0 * std::pow(electricalData.getWTO(), 0.473);

        ConvMass convMassToKg(Mass::LB, Mass::KG, electricalSystemWeightEstimated);

        electricalSystemWeightEstimated = convMassToKg.getConvertedValues();

        return electricalSystemWeightEstimated;
    }

    // ================= AVIONICS GROUP WEIGHT METHODS =================

    /// @brief Calculates the avionics group weight (in kg)
    /// @param WTO The maximum take-off weight (in kg)
    /// @return The estimated avionics group weight (in kg)
    double avionicsWeight(BaseWeightData avionicsData)
    {

        ConvMass massConverter(Mass::KG, Mass::LB, avionicsData.getWTO());

        avionicsWeightEstimated = 600 + 0.005 * avionicsData.getWTO();

        ConvMass convMassToKg(Mass::LB, Mass::KG, avionicsWeightEstimated);

        avionicsWeightEstimated = convMassToKg.getConvertedValues();

        return avionicsWeightEstimated;
    }

    // ================= FURNISHINGS AND EQUIPMENT WEIGHT METHODS =================

    /// @brief Calculates the furnishings and equipment weight (in kg)
    /// @param WTO The maximum take-off weight (in kg)
    /// @return The estimated furnishings and equipment weight (in kg)
    double furnishingsAndEquipmentWeight(BaseWeightData furnishingsData)
    {

        if (furnishingsData.getHasFurnishinghAndEquipment())
        {
            ConvMass massConverter(Mass::KG, Mass::LB, furnishingsData.getWTO());

            furnishingsAndEquipmentWeightEstimated = 0.001 * furnishingsData.getWTO() + 0.4 * std::pow(furnishingsData.getWTO(), 0.85);

            ConvMass convMassToKg(Mass::LB, Mass::KG, furnishingsAndEquipmentWeightEstimated);

            furnishingsAndEquipmentWeightEstimated = convMassToKg.getConvertedValues();
        }

        return furnishingsAndEquipmentWeightEstimated;
    }

    // ================= AIR CONDITIONING AND ANTI-ICE WEIGHT METHODS =================

    /// @brief Calculates the air conditioning and anti-ice weight (in kg)
    /// @param WTO The maximum take-off weight (in kg)
    /// @return The estimated air conditioning and anti-ice weight (in kg)
    double airConditioningAndAntiIceWeight(BaseWeightData airConditioningData)
    {

        if (airConditioningData.getHasAirConditioningAndAntiIce())
        {

            ConvMass massConverter(Mass::KG, Mass::LB, airConditioningData.getWTO());

            airConditioningAndIceProtectionWeightEstimated = 0.032 * std::pow(airConditioningData.getWTO(), 0.84);

            ConvMass convMassToKg(Mass::LB, Mass::KG, airConditioningAndIceProtectionWeightEstimated);

            airConditioningAndIceProtectionWeightEstimated = convMassToKg.getConvertedValues();
        }

        return airConditioningAndIceProtectionWeightEstimated;
    }

    // ================= OPERATING ITEMS WEIGHT METHODS =================

    /// @brief Calculates the operating items weight (in kg)
    /// @param aircraftCategory The category of the aircraft
    /// @param rangeCovered The range covered by the aircraft (in nm - nautical miles)
    /// @param numberOfPax The number of passengers
    /// @return The estimated operating items weight (in kg)
    double operatingItemsWeight(BaseWeightData operatingItemsData)
    {

        if (operatingItemsData.getAircraftCategory() != AircraftCategory::UAV)
        {

            if (operatingItemsData.getRangeCovered() < 2000)
            {

                operatingItemsWeightEstimated = 8.617 * operatingItemsData.getNumberOfPax();
            }

            else
            {

                operatingItemsWeightEstimated = 14.97 * operatingItemsData.getNumberOfPax();
            }
        }

        else
        {

            operatingItemsWeightEstimated = 0.0;
        }

        return operatingItemsWeightEstimated;
    }
};
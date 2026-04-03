#pragma once
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "Interpolant.h"
#include "RegressionMethod.h"
#include "WINGBASEDATA.h"
#include "FUSELAGEBASEDATA.h"
#include "ENGINEBASEDATA.h"
#include "LANDINGGEARSBASEDATA.h"
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
#include "EnumSkinRoughnessType.h"
#include "EnumWingFairingType.h"
#include "EnumTypeOfFlap.h"
#include "EnumWindScreenType.h"
#include "ConvMass.h"
#include "ConvForce.h"
#include "ConvLength.h"
#include "ConvArea.h"
#include "ConvVel.h"
#include "ATMOSISA.h"
#include "ODE45.h"
#include "PLOT.h"
#include "VSPGEOMETRYEXTRACTOR.h"
#include "VSPScriptGenerator.h"
#include "VSPAeroGenerator.h"
#include "WETTEDAREA.h"
#include "BUILDAIRCRAFT.h"
#include "WETTEDAREA.h"
#include "RestoreSettings.h"
#include "ATMOSISA.h"
#include "Interpolant2D.h"
#include "Interpolant3D.h"
#include "RegressionMethod.h"
#include "ChordCalculator.h"

/// @brief Constructor for the CD0Calculator class. Initializes the calculator with the necessary aircraft data, settings, and geometric information for the various components of the aircraft. The constructor takes in references to the builder object, aircraft data, aerodynamic settings, and geometric data for the wing, horizontal tail, vertical tail, fuselage, nacelle, canard (optional), boom (optional), and EOIR (optional). This allows the calculator to have access to all relevant information needed for accurate CD0 calculations across different components of the aircraft.
/// @param nameOfAircraft The name of the aircraft for which CD0 is being calculated.
/// @param builder A reference to the BuildAircraft object that contains the aircraft's design
/// @param aircraftData A reference to the VSP::Aircraft object that contains the geometric data of the aircraft.
/// @param settings A reference to the VSP::AeroSettings object that contains the aerodynamic settings
/// @param wing A reference to the VSP::Wing object that contains the geometric data of the wing.
/// @param horizontal A reference to the VSP::Wing object that contains the geometric data of the horizontal tail.
/// @param vertical A reference to the VSP::Wing object that contains the geometric data of the vertical tail.
/// @param fus A reference to the VSP::Fuselage object that contains the geometric data of the fuselage.
/// @param nac A reference to the VSP::Nacelle object that contains the geometric data of the nacelle.
/// @param canard An optional reference to the VSP::Wing object that contains the geometric data of the canard. Default is an empty wing object.
/// @param boom An optional reference to the VSP::Boom object that contains the geometric data of the boom. Default is an empty boom object.
/// @param eoir An optional reference to the VSP::EOIR object that contains the geometric data of the EOIR. Default is an empty EOIR object.
class CD0Calculator
{

protected:
    const BuildAircraft &builder;
    const VSP::Aircraft aircraftData;
    VSP::AeroSettings &settings;
    const VSP::Wing &wing;
    const VSP::Wing &canard;
    const VSP::Wing &horizontal;
    const VSP::Wing &vertical;
    const VSP::Fuselage &fus;
    const VSP::Nacelle nac;
    const VSP::Boom boom;
    const VSP::EOIR eoir;

private:
    std::string nameOfAircraft = "";
    double reynoldsNumber = 0.0;
    double divergenceMachNumber = 0.0;
    double criticalMachNumber = 0.0;
    double kFactorForAirfoilClass = 0.95; // Suiggested by Gudmundsson, for NASA-style super-crtical airfoils.
    double totalCD0Aircraft = 0.0;
    double deltaCDWave = 0.0;
    double trimDrag = 0.0;
    double laminarReynolds = 2000000.0; // Threshold for laminar to turbulent transition, can be adjusted based on specific conditions
    double epsReynolds = 1e-9;
    double tourbolentReynolds = 200000.0 + epsReynolds; // Threshold for turbulent to fully turbulent transition, can be adjusted based on specific conditions
    double ficticiuousTransitionPoint = 0.0;
    double kCrudFactor = 0.0;
    double numeratorAverageChordRatio = 0.0;
    double weightsOfWeightedAverageChordRatio = 0.0;
    double averageChordRatio = 0.0;

    // Wing varibales related
    double cd0Wing = 0.0;
    double wettedAreaExposedWing = 0.0;
    double skinRoughnessCoefficientWing = 0.0;
    double formFactorWing = 0.0;
    double interferenceFactorWing = 0.0;
    double interfrenceFactorWinglet = 1.0;
    double cutoffReynoldsNumberWing = 0.0;
    double skinRoughnessFactorWing = 0.0;
    double mixedSkinFrictionWing = 0.0;
    double deltaCd0DueToFlapDeflection = 0.0;
    double contributionDueToFlapChordRatio = 0.0;
    double contributionDueToFlapDeflection = 0.0;
    double etaInboardFlap = 0.0;
    double etaOutboardFlap = 0.0;
    double wingAreaAffectedByFlap = 0.0;

    // Canard variables related
    double cd0Canard = 0.0;
    double wettedAreaExposedCanard = 0.0;
    double skinRoughnessCoefficientCanard = 0.0;
    double formFactorCanard = 0.0;
    double interferenceFactorCanard = 0.0;
    double cutoffReynoldsNumberCanard = 0.0;
    double skinRoughnessFactorCanard = 0.0;
    double mixedSkinFrictionCanard = 0.0;
    double wingAreaAffectedByCanardFlap = 0.0;

    // Horizontal Tail variables related
    double cd0HorizontalTail = 0.0;
    double wettedAreaExposedHorizontalTail = 0.0;
    double skinRoughnessCoefficientHorizontalTail = 0.0;
    double formFactorHorizontalTail = 0.0;
    double interferenceFactorHorizontalTail = 0.0;
    double cutoffReynoldsNumberHorizontalTail = 0.0;
    double skinRoughnessFactorHorizontalTail = 0.0;
    double mixedSkinFrictionHorizontalTail = 0.0;
    double wingAreaAffectedByElevator = 0.0;
    double planformAreaHorizontal = 0.0;

    // Vertical Tail variables related
    double cd0VerticalTail = 0.0;
    double wettedAreaExposedVerticalTail = 0.0;
    double skinRoughnessCoefficientVerticalTail = 0.0;
    double formFactorVerticalTail = 0.0;
    double interferenceFactorVerticalTail = 0.0;
    double cutoffReynoldsNumberVerticalTail = 0.0;
    double skinRoughnessFactorVerticalTail = 0.0;
    double mixedSkinFrictionVerticalTail = 0.0;
    double planformAreaVertical = 0.0;
    double wingAreaAffectedByRudder = 0.0;
    double deltaCd0DueToRudderDeflection = 0.0;

    // FusiForm body variables related
    double cd0FusiFormBody = 0.0;
    double wettedAreaExposedFusiFormBody = 0.0;
    double cutoffReynoldsNumberFusiFormBody = 0.0;
    double formFactorFusiFormBody = 0.0;
    double skinRoughnessFactorFusiFormBody = 0.0;
    double factorDueToCompressibilityFusiFormBody = 0.0;
    double skinRoughnessCoefficientFusiFormBody = 0.0;
    double fullyTurbulentSkinFrictionFusiFormBody = 0.0;
    double maximumExposedDiameterFusiFormBody = 0.0;
    double wettedAreaExposedFusiFormBodyFuselage = 0.0;

    // Fuselage variables related
    double cd0Fuselage = 0.0;
    double deltaCd0DueToWindScreenType = 0.0;

    // Nacelle variables related
    double cd0Nacelle = 0.0;

    // Boom variables related
    double cd0Boom = 0.0;

    // Landing Gears variables related
    double wheelNoseFrontalArea = 0.0;
    double wheelMainFrontalArea = 0.0;
    double strutAreaNose = 0.0;
    double strutAreaMain = 0.0;
    double cd0NoseWheel = 0.0;
    double cd0MainWheel = 0.0;
    double cd0NoseStrut = 0.0;
    double cd0MainStrut = 0.0;
    double kFactorDueToFlapDeflection = 0.0;
    double cd0LandingGears = 0.0;
    double circularRodCDLaminarFlowSadraey = 1.2;   // CD for circular rod in laminar flow, from Sadraey's book
    double circularRodCDTurbulentFlowSadraey = 0.3; // CD for circular rod in turbulent flow, from Sadraey's book

    // ============================================================
    // DELTA 1 — Dipendenza dal Flap Chord Ratio (cf/c)
    // ============================================================

    // Slotted Flap, t/c = 0.12
    const std::vector<double> x_delta1_slotted_tc012 = {
        0.00159403, 0.0430673, 0.0877139, 0.130226, 0.165823, 0.218411,
        0.269378, 0.319278, 0.339972, 0.358527, 0.372816, 0.380750, 0.388679};
    const std::vector<double> y_delta1_slotted_tc012 = {
        0.00901016, 0.168349, 0.384670, 0.612953, 0.823170, 1.15947,
        1.55868, 1.96388, 2.15593, 2.37194, 2.60588, 2.74683, 2.89978};

    // Slotted Flap, t/c = 0.21
    const std::vector<double> x_delta1_slotted_tc021 = {
        0.00213218, 0.0505095, 0.0919714, 0.137138, 0.178577, 0.231162,
        0.252926, 0.283184, 0.315027, 0.345282, 0.367564, 0.398341};
    const std::vector<double> y_delta1_slotted_tc021 = {
        -0.00596847, 0.201406, 0.390715, 0.640009, 0.889258, 1.23454,
        1.41463, 1.66075, 1.93985, 2.19496, 2.41401, 2.69610};

    // Plain Flap, t/c = 0.30
    const std::vector<double> x_delta1_plain_tc030 = {
        0.00212651, 0.0446647, 0.0930330, 0.168485, 0.223197,
        0.261431, 0.295424, 0.324621, 0.364455, 0.401097};
    const std::vector<double> y_delta1_plain_tc030 = {
        0.00901654, 0.168368, 0.399718, 0.823202, 1.17751,
        1.45369, 1.67887, 1.91599, 2.18620, 2.44738};

    // Plain Flap, t/c = 0.12
    const std::vector<double> x_delta1_plain_tc012 = {
        0.00159744, 0.0499850, 0.103127, 0.160515, 0.225852, 0.275260,
        0.303410, 0.325195, 0.346450, 0.370365, 0.399600};
    const std::vector<double> y_delta1_plain_tc012 = {
        1.91502e-05, 0.180420, 0.459780, 0.778152, 1.19552, 1.49282,
        1.68197, 1.80511, 1.92224, 2.04541, 2.18362};

    // Plain Flap, t/c = 0.21
    const std::vector<double> x_delta1_plain_tc021 = {
        0.00478891, 0.0382920, 0.0728475, 0.121201, 0.163705, 0.208865,
        0.255085, 0.291208, 0.324690, 0.353920, 0.382090, 0.401764};
    const std::vector<double> y_delta1_plain_tc021 = {
        0.00904846, 0.123337, 0.270605, 0.540916, 0.790178, 1.05745,
        1.33673, 1.56494, 1.73317, 1.88337, 2.01858, 2.09374};

    // ============================================================
    // DELTA 2 — Dipendenza dalla Deflessione del Flap (gradi)
    // ============================================================

    // Split Flap, t/c = 0.12
    const std::vector<double> x_delta2_split_tc012 = {
        0.343643, 5.26919, 10.3093, 17.1821, 22.6804, 30.0115, 38.9462,
        47.6518, 54.7537, 63.2302, 71.3631, 77.0905, 81.4433, 83.8488, 86.8270, 89.6907};
    const std::vector<double> y_delta2_split_tc012 = {
        0.000994036, 0.00646123, 0.0156561, 0.0315606, 0.0467197, 0.0708250, 0.0979125,
        0.129722, 0.153579, 0.180417, 0.204026, 0.218439, 0.227386, 0.230865, 0.234095, 0.234841};

    // Split Flap, t/c = 0.30
    const std::vector<double> x_delta2_split_tc030 = {
        0.458190, 5.26919, 9.85109, 16.2658, 21.5349, 32.4170, 38.9462,
        45.0172, 53.7228, 61.8557, 69.4158, 75.1432, 79.8396, 83.6197, 87.1707, 89.3471};
    const std::vector<double> y_delta2_split_tc030 = {
        0.000994036, 0.00149105, 0.00397614, 0.0124254, 0.0216203, 0.0424950, 0.0581511,
        0.0770378, 0.104622, 0.130716, 0.152087, 0.169483, 0.182406, 0.190855, 0.197565, 0.200547};

    // Split Flap, t/c = 0.21
    const std::vector<double> x_delta2_split_tc021 = {
        0.343643, 5.95647, 11.4548, 16.8385, 24.1695, 30.1260, 37.4570,
        44.6735, 52.9210, 59.6793, 63.5739, 68.6140, 73.3104, 77.0905,
        80.6415, 83.2761, 86.1397, 90.1489};
    const std::vector<double> y_delta2_split_tc021 = {
        -0.000497018, 0.00273360, 0.00869781, 0.0191352, 0.0333002, 0.0509443, 0.0745527,
        0.0999006, 0.128976, 0.153579, 0.164761, 0.179175, 0.194085, 0.204026,
        0.211233, 0.216203, 0.219433, 0.223658};

    // Slotted Flap, t/c = 0.30
    const std::vector<double> x_delta2_slotted_tc030 = {
        0.458190, 6.41466, 13.1730, 20.8477, 27.9496, 34.1352, 42.2680,
        48.1100, 55.4410, 61.9702, 67.4685, 72.1649, 77.0905, 82.8179, 87.7434, 92.7835};
    const std::vector<double> y_delta2_slotted_tc030 = {
        0.000248509, 0.000994036, 0.00521869, 0.0109344, 0.0206262, 0.0333002, 0.0506958,
        0.0636183, 0.0792744, 0.0926938, 0.102634, 0.109841, 0.116551, 0.123012, 0.126243, 0.127237};

    // Slotted Flap, t/c = 0.21
    const std::vector<double> x_delta2_slotted_tc021 = {
        0.229095, 7.21650, 13.2875, 20.2749, 29.2096, 38.0298, 46.9645,
        55.6701, 60.7102, 67.4685, 73.5395, 77.8923, 82.5888, 85.6816,
        88.0871, 90.1489, 93.3562};
    const std::vector<double> y_delta2_slotted_tc021 = {
        0.000248509, 0.00149105, 0.00472167, 0.0106859, 0.0218688, 0.0357853, 0.0497018,
        0.0636183, 0.0705765, 0.0805169, 0.0889662, 0.0959245, 0.102137, 0.104125,
        0.104871, 0.104871, 0.103877};

    // Slotted Flap, t/c = 0.12
    const std::vector<double> x_delta2_slotted_tc012 = {
        0.229095, 7.21650, 11.6838, 15.5785, 19.8167, 25.0859, 29.6678,
        33.2188, 37.6861, 42.0389, 47.3081, 54.7537, 60.2520, 64.7194,
        71.0195, 77.4341, 82.9324, 86.9416, 89.6907, 92.6690};
    const std::vector<double> y_delta2_slotted_tc012 = {
        0.000248509, 0.000248509, 0.00149105, 0.00298211, 0.00571571, 0.0109344, 0.0168986,
        0.0246024, 0.0328032, 0.0424950, 0.0534294, 0.0666004, 0.0765408, 0.0844930,
        0.0934394, 0.101640, 0.108101, 0.112326, 0.114314, 0.114066};

    // Plain Flap, tutti i t/c
    const std::vector<double> x_delta2_plain_all_tc = {
        0.114548, 3.78007, 8.59107, 14.7766, 21.3440, 30.1260,
        39.5189, 46.7736, 51.8137, 57.6174, 62.0466, 64.1848};
    const std::vector<double> y_delta2_plain_all_tc = {
        0.000248509, 0.00132538, 0.00712392, 0.0188867, 0.0324718, 0.0569914,
        0.0849901, 0.108516, 0.127402, 0.145129, 0.156395, 0.162856};

public:

    /// @brief Constructor for the CD0Calculator class. Initializes the calculator with the necessary aircraft data, settings, and geometric information for the various components of the aircraft. The constructor takes in references to the builder object, aircraft data, aerodynamic settings, and geometric data for the wing, horizontal tail, vertical tail, fuselage, nacelle, canard (optional), boom (optional), and EOIR (optional). This allows the calculator to have access to all relevant information needed for accurate CD0 calculations across different components of the aircraft.
    /// @param nameOfAircraft The name of the aircraft for which CD0 is being calculated.
    /// @param builder A reference to the BuildAircraft object that contains the aircraft's design
    /// @param aircraftData A reference to the VSP::Aircraft object that contains the geometric data of the aircraft.
    /// @param settings A reference to the VSP::AeroSettings object that contains the aerodynamic settings
    /// @param wing A reference to the VSP::Wing object that contains the geometric data of the wing.
    /// @param horizontal A reference to the VSP::Wing object that contains the geometric data of the horizontal tail.
    /// @param vertical A reference to the VSP::Wing object that contains the geometric data of the vertical tail.
    /// @param fus A reference to the VSP::Fuselage object that contains the geometric data of the fuselage.
    /// @param nac A reference to the VSP::Nacelle object that contains the geometric data of the nacelle.
    /// @param canard An optional reference to the VSP::Wing object that contains the geometric data of the canard. Default is an empty wing object.
    /// @param boom An optional reference to the VSP::Boom object that contains the geometric data of the boom. Default is an empty boom object.
    /// @param eoir An optional reference to the VSP::EOIR object that contains the geometric data of the EOIR. Default is an empty EOIR object.    
    CD0Calculator(std::string nameOfAircraft,
                  const BuildAircraft &builder,
                  const VSP::Aircraft &aircraftData,
                  VSP::AeroSettings &settings,
                  const VSP::Wing &wing,
                  const VSP::Wing &horizontal,
                  const VSP::Wing &vertical,
                  const VSP::Fuselage &fus,
                  const VSP::Nacelle &nac,
                  const VSP::Wing &canard = VSP::Wing(),
                  const VSP::Boom &boom = VSP::Boom(),
                  const VSP::EOIR &eoir = VSP::EOIR())
        : nameOfAircraft(nameOfAircraft),
          builder(builder),
          aircraftData(aircraftData),
          settings(settings),
          wing(wing),
          horizontal(horizontal),
          vertical(vertical),
          fus(fus),
          nac(nac),
          canard(canard),
          boom(boom),
          eoir(eoir)
    {
    }

    /// @brief Calculates the wing CD0.
    /// @param transitionPoint The transition point for the wing xtr/c
    /// @return The calculated zero-lift drag coefficient (CD0) for the wing.
    double calculateCD0Wing(double designCruiseLiftCoefficient = 0.5 * (0.4 + 0.7), double transitionPoint = 0.0)
    {

        RestoreSettings settingsRestorer; // Create an instance of RestoreSettings to automatically restore settings when going out of scope

        settingsRestorer.setSavePrevoiusSettings(settings);

        if (builder.getCommonData().getAircraftCategory() == AircraftCategory::TRANSPORT_JET)
        {

            // The Kron relation for swept Wing - Gudmundsson, Genaeral Aviations Aircraft Design, pag. 697
            divergenceMachNumber = (kFactorForAirfoilClass / std::cos(wing.sweepC2 / 57.3)) - (wing.averageThicknessToChordRatio / std::pow(std::cos(wing.sweepC2 / 57.3), 2)) - (designCruiseLiftCoefficient / (10 * std::pow(std::cos(wing.sweepC2 / 57.3), 3)));
            criticalMachNumber = divergenceMachNumber - 0.1077;

            deltaCDWave = 20 * std::pow((settings.Mach - criticalMachNumber), 4);
        }

        // Skin roughness factor calculation

        if (builder.getCommonData().getSkinRoughnessTypeWing() == SkinRoughnessType::CAMOUFLAGE_OR_ALUMINIUM)
        {
            skinRoughnessCoefficientWing = 3.33 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeWing() == SkinRoughnessType::SMOOTH_PAINT)
        {
            skinRoughnessCoefficientWing = 2.08 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeWing() == SkinRoughnessType::PRODUCTION_SHEET_METAL)
        {
            skinRoughnessCoefficientWing = 1.33 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeWing() == SkinRoughnessType::POLISHED_SHEET_METAL)
        {
            skinRoughnessCoefficientWing = 0.5 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeWing() == SkinRoughnessType::SMOOTH_MOLDED_COMPOSITE)
        {
            skinRoughnessCoefficientWing = 0.17 * 0.3048 * 1e-5;
        }

        // Cutoff Re number calculation
        if (settings.Mach <= 0.6)
        {

            cutoffReynoldsNumberWing = 38.21 * std::pow(wing.MAC / skinRoughnessCoefficientWing, 1.053);
        }

        else
        {

            cutoffReynoldsNumberWing = 44.62 * std::pow(wing.MAC / skinRoughnessCoefficientWing, 1.053) * std::pow(settings.Mach, 1.16);
        }

        // Deriving a possible transition point based on Gudmundsson suggestion and Raymer

        if (builder.getCommonData().getAircraftCategory() == AircraftCategory::TRANSPORT_JET)
        {

            kCrudFactor = 1.10; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94

            if (transitionPoint == 0.0)
            {
                transitionPoint = 0.5 * (0.1 + 0.2); // Assumed transition point for transport jets, can be adjusted based on specific conditions
            }
        }

        else if (builder.getCommonData().getAircraftCategory() == AircraftCategory::GENERAL_AVIATION ||
                 builder.getCommonData().getAircraftCategory() == AircraftCategory::REGIONAL_TURBOPROP ||
                 builder.getCommonData().getAircraftCategory() == AircraftCategory::UAV)
        {

            kCrudFactor = 1.20; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94

            if (transitionPoint == 0.0)
            {
                transitionPoint = 0.5 * (0.1 + 0.2); // Assumed transition point for general aviation aircraft, can be adjusted based on specific conditions
            }
        }

        // Calculation of Skin Friction Coeffcient

        if (settings.ReCref > cutoffReynoldsNumberWing)
        {

            reynoldsNumber = cutoffReynoldsNumberWing;

            ficticiuousTransitionPoint = 36.9 * std::pow(transitionPoint, 0.625) * std::pow(reynoldsNumber, -0.375);

            // Mixed Laminar-Turbulent Skin Griction - Young's Method
            mixedSkinFrictionWing = 0.074 / std::pow(reynoldsNumber, 0.2) * std::pow(1 - (transitionPoint - ficticiuousTransitionPoint), 0.8);
        }

        else
        {

            reynoldsNumber = settings.ReCref;

            ficticiuousTransitionPoint = 36.9 * std::pow(transitionPoint, 0.625) * std::pow(reynoldsNumber, -0.375);

            // Mixed Laminar-Turbulent Skin Griction - Young's Method
            mixedSkinFrictionWing = 0.074 / std::pow(reynoldsNumber, 0.2) * std::pow(1 - (transitionPoint - ficticiuousTransitionPoint), 0.8);
        }

        // Form Factor calculation - Shevell's Method

        formFactorWing = 1 + ((2 - std::pow(settings.Mach, 2.0)) * std::cos(wing.sweepC2 / 57.3)) / (std::sqrt(1 - std::pow(settings.Mach, 2.0) * std::pow(std::cos(wing.sweepC2 / 57.3), 2.0))) * (wing.averageThicknessToChordRatio) + 100 * std::pow(wing.averageThicknessToChordRatio, 4.0);

        // Interference Factor calculation - Gundundsson - General Aviation Aircrfat Design, Aplied Methods and Procedures, pag. 771

        if (builder.getWingData().getWingPosition() == WingPosition::HIGH_WING ||
            builder.getWingData().getWingPosition() == WingPosition::MID_WING)
        {
            interferenceFactorWing = 1.0; // carefully designed fairing per definizione
        }

        else if (builder.getWingData().getWingPosition() == WingPosition::LOW_WING)
        {
            if (builder.getWingData().getWingFairingType() == WingFairingType::UNFILLETED)
            {
                interferenceFactorWing = 0.5 * (1.1 + 1.4); // worst case
            }
            else if (builder.getWingData().getWingFairingType() == WingFairingType::FILLETED_FAIRING)
            {
                interferenceFactorWing = 1.1; // conservativo con carenatura
            }
            else if (builder.getWingData().getWingFairingType() == WingFairingType::CAREFULLY_DESIGNED)
            {
                interferenceFactorWing = 1.0; // transport jet come A320neo
            }
        }

        if (wing.blending)
        {

            interfrenceFactorWinglet = 1.04; // Supposed "Airbus" style winglet
        }

        // Swet wing calculation

        WETTEDAREA::WettedArea wettedAreaCalculatorWing(nameOfAircraft + "_GetGeomOfAircraft.vspscript", aircraftData);

        wettedAreaCalculatorWing.getAllGeoms(nameOfAircraft, false);

        wettedAreaCalculatorWing.executeAndCaptureGeomIds();

        wettedAreaCalculatorWing.CalculateWettedArea(nameOfAircraft, "WettedAreaWing.vspscript", wing.id);

        wettedAreaCalculatorWing.executeAndCaptureWettedArea("WettedAreaWing.vspscript");

        // Access and display results
        const auto &results = wettedAreaCalculatorWing.getWettedAreaResults();

        // OpenVsp gives us only the enatire half-wetted area of the wing
        wettedAreaExposedWing = 2 * (results.WET_AREA - 0.5 * fus.diameter);

        // Contribution to the parasite drag due to flap deflecction

        for (size_t n = 0; n < wing.mov.type.size(); n++)
        {

            if (wing.mov.type[n] == 'f')
            {

                // Larghezza del segmento n-esimo
                double segmentSpan = (wing.mov.eta_outer[n] - wing.mov.eta_inner[n]) * wing.totalProjectedSpan;

                // cf/c medio del segmento = media tra inner e outer
                double cfcMean = 0.5 * (wing.mov.cf_c_inner[n] + wing.mov.cf_c_outer[n]);

                numeratorAverageChordRatio += cfcMean * segmentSpan;
                weightsOfWeightedAverageChordRatio += segmentSpan;

                etaInboardFlap = wing.mov.eta_inner[n];
                etaOutboardFlap = wing.mov.eta_outer[n];

                double inboardChord = ChordCalculator(wing).getChordAtEtaStation(etaInboardFlap, wing.typeOfWing);
                double outboardChord = ChordCalculator(wing).getChordAtEtaStation(etaOutboardFlap, wing.typeOfWing);

                // Half-flap-affected wing area.
                wingAreaAffectedByFlap += 0.5 * (inboardChord + outboardChord) * segmentSpan;
            }
        }

        averageChordRatio = numeratorAverageChordRatio / weightsOfWeightedAverageChordRatio;

        for (size_t i = 0; i < wing.mov.type.size(); i++)
        {

            if (wing.mov.type[i] == 'f')
            {

                if (wing.mov.defl[i] > 0)
                {

                    if (builder.getWingData().getTypeOfFlap() == TypeOfFlap::SLOTTED_FLAP)
                    {
                        // Contribution due to flap-chord-ratio
                        Interpolant2D flapChordRatioEffectInterpolator(4, RegressionMethod::POLYNOMIAL);

                        flapChordRatioEffectInterpolator.addCurve(0.12, x_delta1_slotted_tc012, y_delta1_slotted_tc012);
                        flapChordRatioEffectInterpolator.addCurve(0.21, x_delta1_slotted_tc021, y_delta1_slotted_tc021);
                        flapChordRatioEffectInterpolator.addCurve(0.30, x_delta1_slotted_tc021, y_delta1_slotted_tc021);

                        contributionDueToFlapChordRatio = flapChordRatioEffectInterpolator.interpolate(averageChordRatio, wing.averageThicknessToChordRatio);

                        // Contribution due to flap deflection

                        Interpolant2D flapDeflectionEffectInterpolator(6, RegressionMethod::POLYNOMIAL);

                        flapDeflectionEffectInterpolator.addCurve(0.12, x_delta2_slotted_tc012, y_delta2_slotted_tc012);
                        flapDeflectionEffectInterpolator.addCurve(0.21, x_delta2_slotted_tc021, y_delta2_slotted_tc021);
                        flapDeflectionEffectInterpolator.addCurve(0.30, x_delta2_slotted_tc030, y_delta2_slotted_tc030);

                        contributionDueToFlapDeflection = flapDeflectionEffectInterpolator.interpolate(wing.mov.defl[i], wing.averageThicknessToChordRatio);
                    }
                    else if (builder.getWingData().getTypeOfFlap() == TypeOfFlap::PLAIN_FLAP ||
                             builder.getWingData().getTypeOfFlap() == TypeOfFlap::SPLIT_FLAP)
                    {
                        // Contribution due to flap-chord-ratio
                        Interpolant2D flapChordRatioEffectInterpolator(4, RegressionMethod::POLYNOMIAL);

                        flapChordRatioEffectInterpolator.addCurve(0.12, x_delta1_plain_tc012, y_delta1_plain_tc012);
                        flapChordRatioEffectInterpolator.addCurve(0.21, x_delta1_plain_tc021, y_delta1_plain_tc021);
                        flapChordRatioEffectInterpolator.addCurve(0.30, x_delta1_plain_tc030, y_delta1_plain_tc030);

                        contributionDueToFlapChordRatio = flapChordRatioEffectInterpolator.interpolate(averageChordRatio, wing.averageThicknessToChordRatio);

                        // Contribution due to flap deflection

                        if (builder.getWingData().getTypeOfFlap() == TypeOfFlap::PLAIN_FLAP)
                        {

                            Interpolant2D flapDeflectionEffectInterpolator(3, RegressionMethod::POLYNOMIAL);

                            flapDeflectionEffectInterpolator.addCurve(0.12, x_delta2_plain_all_tc, y_delta2_plain_all_tc);
                            flapDeflectionEffectInterpolator.addCurve(0.21, x_delta2_plain_all_tc, y_delta2_plain_all_tc);
                            flapDeflectionEffectInterpolator.addCurve(0.30, x_delta2_plain_all_tc, y_delta2_plain_all_tc);

                            contributionDueToFlapDeflection = flapDeflectionEffectInterpolator.interpolate(wing.mov.defl[i], wing.averageThicknessToChordRatio);
                        }

                        else
                        {

                            Interpolant2D flapDeflectionEffectInterpolator(3, RegressionMethod::POLYNOMIAL);
                            flapDeflectionEffectInterpolator.addCurve(0.12, x_delta2_split_tc012, y_delta2_split_tc012);
                            flapDeflectionEffectInterpolator.addCurve(0.21, x_delta2_split_tc021, y_delta2_split_tc021);
                            flapDeflectionEffectInterpolator.addCurve(0.30, x_delta2_split_tc030, y_delta2_split_tc030);

                            contributionDueToFlapDeflection = flapDeflectionEffectInterpolator.interpolate(wing.mov.defl[i], wing.averageThicknessToChordRatio);
                        }
                    }
                }

                deltaCd0DueToFlapDeflection = contributionDueToFlapChordRatio * contributionDueToFlapDeflection * (2 * wingAreaAffectedByFlap / wing.planformArea);

                break;
            }
        }

        if (settings.Mach > criticalMachNumber)
        {

            cd0Wing = kCrudFactor * mixedSkinFrictionWing * formFactorWing * interferenceFactorWing * interfrenceFactorWinglet * wettedAreaExposedWing / wing.planformArea + deltaCDWave + deltaCd0DueToFlapDeflection;
        }

        else
        {

            cd0Wing = kCrudFactor * mixedSkinFrictionWing * formFactorWing * interferenceFactorWing * interfrenceFactorWinglet * wettedAreaExposedWing / wing.planformArea + deltaCd0DueToFlapDeflection;
        }

        settings = settingsRestorer.getSettingsToRestore(); // Restore previous settings

        return cd0Wing;
        // TODO
    }

    // ======================= CANARD CD0 CALCULATION =======================

    /// @brief Calculates CD0 of the canard
    /// @param designCruiseLiftCoefficient The Clcruise design of the canarda, default is not considered
    /// @param transitionPoint The transition point for the canard, default is not considered (small laminar region)
    double calculateCD0Canard(double designCruiseLiftCoefficient = 0.0, double transitionPoint = 0.0)
    {

        RestoreSettings settingsRestorer; // Create an instance of RestoreSettings to automatically restore settings when going out of scope

        settingsRestorer.setSavePrevoiusSettings(settings);

    
         // Re-definiotn of the Reynoldns number based on the reference length of the body we are analysing
        auto [T, a, P, rho] = Atmosphere::atmosisa(settings.altitude); // Atmosisa like in MATLAB
        settings.ReCref = settings.Mach * a * canard.MAC / Atmosphere::ISA::kinematicViscosity(settings.altitude);



        if (builder.getCommonData().getAircraftCategory() == AircraftCategory::TRANSPORT_JET)
        {

            // The Kron relation for swept Wing - Gudmundsson, Genaeral Aviations Aircraft Design, pag. 697
            divergenceMachNumber = (kFactorForAirfoilClass / std::cos(canard.sweepC2 / 57.3)) - (canard.averageThicknessToChordRatio / std::pow(std::cos(canard.sweepC2 / 57.3), 2)) - (designCruiseLiftCoefficient / (10 * std::pow(std::cos(canard.sweepC2 / 57.3), 3)));
            criticalMachNumber = divergenceMachNumber - 0.1077;

            deltaCDWave = 20 * std::pow((settings.Mach - criticalMachNumber), 4);
        }

        // Skin roughness factor calculation

        if (builder.getCommonData().getSkinRoughnessTypeCanard() == SkinRoughnessType::CAMOUFLAGE_OR_ALUMINIUM)
        {
            skinRoughnessCoefficientCanard = 3.33 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeCanard() == SkinRoughnessType::SMOOTH_PAINT)
        {
            skinRoughnessCoefficientCanard = 2.08 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeCanard() == SkinRoughnessType::PRODUCTION_SHEET_METAL)
        {
            skinRoughnessCoefficientCanard = 1.33 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeCanard() == SkinRoughnessType::POLISHED_SHEET_METAL)
        {
            skinRoughnessCoefficientCanard = 0.5 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeCanard() == SkinRoughnessType::SMOOTH_MOLDED_COMPOSITE)
        {
            skinRoughnessCoefficientCanard = 0.17 * 0.3048 * 1e-5;
        }

        // Cutoff Re number calculation
        if (settings.Mach <= 0.6)
        {

            cutoffReynoldsNumberHorizontalTail = 38.21 * std::pow(canard.MAC / skinRoughnessCoefficientCanard, 1.053);
        }

        else
        {

            cutoffReynoldsNumberHorizontalTail = 44.62 * std::pow(canard.MAC / skinRoughnessCoefficientCanard, 1.053) * std::pow(settings.Mach, 1.16);
        }

        // Deriving a possible transition point based on Gudmundsson suggestion and Raymer

        if (builder.getCommonData().getAircraftCategory() == AircraftCategory::TRANSPORT_JET)
        {

            kCrudFactor = 1.10; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94

            if (transitionPoint == 0.0)
            {
                transitionPoint = 0.5 * (0.1 + 0.2); // Assumed transition point for transport jets, can be adjusted based on specific conditions
            }
        }

        else if (builder.getCommonData().getAircraftCategory() == AircraftCategory::GENERAL_AVIATION ||
                 builder.getCommonData().getAircraftCategory() == AircraftCategory::REGIONAL_TURBOPROP ||
                 builder.getCommonData().getAircraftCategory() == AircraftCategory::UAV)
        {

            kCrudFactor = 1.20; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94

            if (transitionPoint == 0.0)
            {
                transitionPoint = 0.5 * (0.1 + 0.2); // Assumed transition point for general aviation aircraft, can be adjusted based on specific conditions
            }
        }

        // Calculation of Skin Friction Coeffcient

        if (settings.ReCref > cutoffReynoldsNumberCanard)
        {

            reynoldsNumber = cutoffReynoldsNumberCanard;

            ficticiuousTransitionPoint = 36.9 * std::pow(transitionPoint, 0.625) * std::pow(reynoldsNumber, -0.375);

            // Mixed Laminar-Turbulent Skin Griction - Young's Method
            mixedSkinFrictionCanard = 0.074 / std::pow(reynoldsNumber, 0.2) * std::pow(1 - (transitionPoint - ficticiuousTransitionPoint), 0.8);
        }

        else
        {

            reynoldsNumber = settings.ReCref;

            ficticiuousTransitionPoint = 36.9 * std::pow(transitionPoint, 0.625) * std::pow(reynoldsNumber, -0.375);

            // Mixed Laminar-Turbulent Skin Griction - Young's Method
            mixedSkinFrictionCanard = 0.074 / std::pow(reynoldsNumber, 0.2) * std::pow(1 - (transitionPoint - ficticiuousTransitionPoint), 0.8);
        }

        // Form Factor calculation - Shevell's Method

        formFactorCanard = 1 + ((2 - std::pow(settings.Mach, 2.0)) * std::cos(canard.sweepC2 / 57.3)) / (std::sqrt(1 - std::pow(settings.Mach, 2.0) * std::pow(std::cos(canard.sweepC2 / 57.3), 2.0))) * (canard.averageThicknessToChordRatio) + 100 * std::pow(canard.averageThicknessToChordRatio, 4.0);

        // Interference Factor calculation - Gundundsson - General Aviation Aircrfat Design, Aplied Methods and Procedures, pag. 771

        if (builder.getCommonData().getCanardPosition() == WingPosition::HIGH_WING ||
            builder.getCommonData().getCanardPosition() == WingPosition::MID_WING)
        {
            interferenceFactorCanard = 1.0; // carefully designed fairing per definizione
        }

        else if (builder.getCommonData().getCanardPosition() == WingPosition::LOW_WING)
        {
            if (builder.getCommonData().getCanardFairingType() == WingFairingType::UNFILLETED)
            {
                interferenceFactorCanard = 0.5 * (1.1 + 1.4); // worst case
            }
            else if (builder.getCommonData().getCanardFairingType() == WingFairingType::FILLETED_FAIRING)
            {
                interferenceFactorCanard = 1.1; // conservativo con carenatura
            }
            else if (builder.getCommonData().getCanardFairingType() == WingFairingType::CAREFULLY_DESIGNED)
            {
                interferenceFactorCanard = 1.0; // transport jet come A320neo
            }
        }

        // Swet wing calculation

        WETTEDAREA::WettedArea wettedAreaCalculatorWing(nameOfAircraft + "_GetGeomOfAircraft.vspscript", aircraftData);

        wettedAreaCalculatorWing.getAllGeoms(nameOfAircraft, false);

        wettedAreaCalculatorWing.executeAndCaptureGeomIds();

        wettedAreaCalculatorWing.CalculateWettedArea(nameOfAircraft, "WettedAreaCanard.vspscript", canard.id);

        wettedAreaCalculatorWing.executeAndCaptureWettedArea("WettedAreaCanard.vspscript");

        // Access and display results
        const auto &results = wettedAreaCalculatorWing.getWettedAreaResults();

        VSPGEOMTRYEXTRACTOR::GeometryExtractor geomExtractor;
        geomExtractor.extractAllGeoms(builder.getCommonData().getNameOfAircraft(),
                                      builder.getCommonData().getNameOfAircraft() + "_AllGeomsToCd0.vspscript");

        VSPGEOMTRYEXTRACTOR::AircraftGeometryData allGeomData = geomExtractor.getGeometryData();

        // VSPGEOMTRYEXTRACTOR::GeomInfo fuselageType;

        VSPGEOMTRYEXTRACTOR::DiametersExtractor fuseExtractor;

        VSPGEOMTRYEXTRACTOR::FuselageDiametersAndXStation fuseData = fuseExtractor.extractFuselageDiameters(
            builder.getCommonData().getNameOfAircraft(),
            allGeomData,
            fus.length);

        double diameterAtHorizontalLocation;

        Interpolant fusDiameterInterpolator(fuseData.xStation, fuseData.allFuselageWidth, 1, RegressionMethod::LINEAR);

        diameterAtHorizontalLocation = fusDiameterInterpolator.getYValueFromRegression(horizontal.xloc);

        // OpenVsp gives us only the enatire half-wetted area of the wing
        wettedAreaExposedCanard = 2 * (results.WET_AREA - 0.5 * diameterAtHorizontalLocation);

        // Contribution to the parasite drag due to flap deflecction

        for (size_t n = 0; n < horizontal.mov.type.size(); n++)
        {

            if (canard.mov.type[n] == 'c')
            {

                // Larghezza del segmento n-esimo
                double segmentSpan = (canard.mov.eta_outer[n] - canard.mov.eta_inner[n]) * canard.totalProjectedSpan;

                // cf/c medio del segmento = media tra inner e outer
                double cfcMean = 0.5 * (canard.mov.cf_c_inner[n] + canard.mov.cf_c_outer[n]);

                numeratorAverageChordRatio += cfcMean * segmentSpan;
                weightsOfWeightedAverageChordRatio += segmentSpan;

                etaInboardFlap = canard.mov.eta_inner[n];
                etaOutboardFlap = canard.mov.eta_outer[n];

                double inboardChord = ChordCalculator(canard).getChordAtEtaStation(etaInboardFlap, canard.typeOfWing);
                double outboardChord = ChordCalculator(canard).getChordAtEtaStation(etaOutboardFlap, canard.typeOfWing);

                // Half-flap-affected wing area.
                wingAreaAffectedByCanardFlap += 0.5 * (inboardChord + outboardChord) * segmentSpan;
            }
        }

        averageChordRatio = numeratorAverageChordRatio / weightsOfWeightedAverageChordRatio;

        for (size_t i = 0; i < canard.mov.type.size(); i++)
        {

            if (canard.mov.type[i] == 'c')
            {

                if (canard.mov.defl[i] > 0)
                {

                    trimDrag = canard.planformArea / wing.planformArea * (0.9 * std::pow(averageChordRatio, 1.38) * std::pow(std::sin(canard.mov.defl[i] * 57.3), 2.0));
                }

                break;
            }
        }

        if (settings.Mach > criticalMachNumber)
        {

            cd0HorizontalTail = kCrudFactor * mixedSkinFrictionCanard * formFactorCanard * interferenceFactorCanard * wettedAreaExposedCanard / wing.planformArea + deltaCDWave + trimDrag;
        }

        else
        {

            cd0HorizontalTail = kCrudFactor * mixedSkinFrictionCanard * formFactorCanard * interferenceFactorCanard * wettedAreaExposedCanard / wing.planformArea + trimDrag;
        }

        settings = settingsRestorer.getSettingsToRestore(); // Restore previous settings

        return cd0HorizontalTail;
    }

    // ======================= HORIZONTAL TAIL CD0 CALCULATION =======================

    /// @brief Calculates the horizontal tail CD0
    /// @param designCruiseLiftCoefficient The Clcruise design of the horizontal tail, default is not considered
    /// @param transitionPoint The transition point for the horizontal tail, default is not considered (small laminar region)
    double calculateCD0HorizontalTail(double designCruiseLiftCoefficient = 0.0, double transitionPoint = 0.0)
    {

        RestoreSettings settingsRestorer; // Create an instance of RestoreSettings to automatically restore settings when going out of scope

        settingsRestorer.setSavePrevoiusSettings(settings);

        // Re-definiotn of the Reynoldns number based on the reference length of the body we are analysing
        auto [T, a, P, rho] = Atmosphere::atmosisa(settings.altitude); // Atmosisa like in MATLAB
        settings.ReCref = settings.Mach * a * horizontal.MAC / Atmosphere::ISA::kinematicViscosity(settings.altitude);

        if (builder.getCommonData().getAircraftCategory() == AircraftCategory::TRANSPORT_JET)
        {

            // The Kron relation for swept Wing - Gudmundsson, Genaeral Aviations Aircraft Design, pag. 697
            divergenceMachNumber = (kFactorForAirfoilClass / std::cos(horizontal.sweepC2 / 57.3)) - (horizontal.averageThicknessToChordRatio / std::pow(std::cos(horizontal.sweepC2 / 57.3), 2)) - (designCruiseLiftCoefficient / (10 * std::pow(std::cos(horizontal.sweepC2 / 57.3), 3)));
            criticalMachNumber = divergenceMachNumber - 0.1077;

            deltaCDWave = 20 * std::pow((settings.Mach - criticalMachNumber), 4);
        }

        // Skin roughness factor calculation

        if (builder.getCommonData().getSkinRoughnessTypeHorizontalTail() == SkinRoughnessType::CAMOUFLAGE_OR_ALUMINIUM)
        {
            skinRoughnessCoefficientHorizontalTail = 3.33 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeHorizontalTail() == SkinRoughnessType::SMOOTH_PAINT)
        {
            skinRoughnessCoefficientHorizontalTail = 2.08 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeHorizontalTail() == SkinRoughnessType::PRODUCTION_SHEET_METAL)
        {
            skinRoughnessCoefficientHorizontalTail = 1.33 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeHorizontalTail() == SkinRoughnessType::POLISHED_SHEET_METAL)
        {
            skinRoughnessCoefficientHorizontalTail = 0.5 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeHorizontalTail() == SkinRoughnessType::SMOOTH_MOLDED_COMPOSITE)
        {
            skinRoughnessCoefficientHorizontalTail = 0.17 * 0.3048 * 1e-5;
        }

        // Cutoff Re number calculation
        if (settings.Mach <= 0.6)
        {

            cutoffReynoldsNumberHorizontalTail = 38.21 * std::pow(horizontal.MAC / skinRoughnessCoefficientHorizontalTail, 1.053);
        }

        else
        {

            cutoffReynoldsNumberHorizontalTail = 44.62 * std::pow(horizontal.MAC / skinRoughnessCoefficientHorizontalTail, 1.053) * std::pow(settings.Mach, 1.16);
        }

        // Deriving a possible transition point based on Gudmundsson suggestion and Raymer

        if (builder.getCommonData().getAircraftCategory() == AircraftCategory::TRANSPORT_JET)
        {

            kCrudFactor = 1.10; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94

            if (transitionPoint == 0.0)
            {
                transitionPoint = 0.5 * (0.1 + 0.2); // Assumed transition point for transport jets, can be adjusted based on specific conditions
            }
        }

        else if (builder.getCommonData().getAircraftCategory() == AircraftCategory::GENERAL_AVIATION ||
                 builder.getCommonData().getAircraftCategory() == AircraftCategory::REGIONAL_TURBOPROP ||
                 builder.getCommonData().getAircraftCategory() == AircraftCategory::UAV)
        {

            kCrudFactor = 1.20; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94

            if (transitionPoint == 0.0)
            {
                transitionPoint = 0.5 * (0.1 + 0.2); // Assumed transition point for general aviation aircraft, can be adjusted based on specific conditions
            }
        }

        // Calculation of Skin Friction Coeffcient

        if (settings.ReCref > cutoffReynoldsNumberHorizontalTail)
        {

            reynoldsNumber = cutoffReynoldsNumberHorizontalTail;

            ficticiuousTransitionPoint = 36.9 * std::pow(transitionPoint, 0.625) * std::pow(reynoldsNumber, -0.375);

            // Mixed Laminar-Turbulent Skin Griction - Young's Method
            mixedSkinFrictionHorizontalTail = 0.074 / std::pow(reynoldsNumber, 0.2) * std::pow(1 - (transitionPoint - ficticiuousTransitionPoint), 0.8);
        }

        else
        {

            reynoldsNumber = settings.ReCref;

            ficticiuousTransitionPoint = 36.9 * std::pow(transitionPoint, 0.625) * std::pow(reynoldsNumber, -0.375);

            // Mixed Laminar-Turbulent Skin Griction - Young's Method
            mixedSkinFrictionHorizontalTail = 0.074 / std::pow(reynoldsNumber, 0.2) * std::pow(1 - (transitionPoint - ficticiuousTransitionPoint), 0.8);
        }

        // Form Factor calculation - Shevell's Method

        formFactorHorizontalTail = 1 + ((2 - std::pow(settings.Mach, 2.0)) * std::cos(horizontal.sweepC2 / 57.3)) / (std::sqrt(1 - std::pow(settings.Mach, 2.0) * std::pow(std::cos(horizontal.sweepC2 / 57.3), 2.0))) * (horizontal.averageThicknessToChordRatio) + 100 * std::pow(horizontal.averageThicknessToChordRatio, 4.0);

        // Interference Factor calculation - Gundundsson - General Aviation Aircrfat Design, Aplied Methods and Procedures, pag. 771

        if (builder.getEngineData().getTypeOfTail() == TypeOfTail::CONVENTIONAL_TAIL)
        {

            planformAreaHorizontal = horizontal.planformArea;

            interferenceFactorHorizontalTail = 0.5 * (1.04 + 1.05);
        }

        else if (builder.getEngineData().getTypeOfTail() == TypeOfTail::CRUCIFORM_TAIL)
        {

            planformAreaHorizontal = horizontal.planformArea;

            interferenceFactorHorizontalTail = 1.06;
        }

        else if (builder.getEngineData().getTypeOfTail() == TypeOfTail::V_TAIL ||
                 builder.getEngineData().getTypeOfTail() == TypeOfTail::V_REV_TAIL)
        {

            planformAreaHorizontal = horizontal.planformArea * std::pow(std::cos(horizontal.averageDihedral / 57.3), 2.0);

            interferenceFactorHorizontalTail = 1.03;
        }

        else if (builder.getEngineData().getTypeOfTail() == TypeOfTail::H_TAIL)
        {

            planformAreaHorizontal = horizontal.planformArea;

            interferenceFactorHorizontalTail = (1.08 + 1.13 + 1.06) / 3.0;
        }

        else if (builder.getEngineData().getTypeOfTail() == TypeOfTail::T_TAIL)
        {

            planformAreaHorizontal = horizontal.planformArea;

            interferenceFactorHorizontalTail = 1.04;
        }

        else if (builder.getEngineData().getTypeOfTail() == TypeOfTail::TRIPLE_TAIL)
        {

            planformAreaHorizontal = horizontal.planformArea;

            interferenceFactorHorizontalTail = 1.10;
        }

        // Swet wing calculation

        if (builder.getCommonData().getTypeOfTail() != TypeOfTail::V_TAIL ||
            builder.getCommonData().getTypeOfTail() != TypeOfTail::V_REV_TAIL)
        {
            WETTEDAREA::WettedArea wettedAreaCalculatorWing(nameOfAircraft + "_GetGeomOfAircraft.vspscript", aircraftData);

            wettedAreaCalculatorWing.getAllGeoms(nameOfAircraft, false);

            wettedAreaCalculatorWing.executeAndCaptureGeomIds();

            wettedAreaCalculatorWing.CalculateWettedArea(nameOfAircraft, "WettedAreaHorizontal.vspscript", horizontal.id);

            wettedAreaCalculatorWing.executeAndCaptureWettedArea("WettedAreaHorizontal.vspscript");

            // Access and display results
            const auto &results = wettedAreaCalculatorWing.getWettedAreaResults();

            VSPGEOMTRYEXTRACTOR::GeometryExtractor geomExtractor;
            geomExtractor.extractAllGeoms(builder.getCommonData().getNameOfAircraft(),
                                          builder.getCommonData().getNameOfAircraft() + "_AllGeomsToCd0.vspscript");

            VSPGEOMTRYEXTRACTOR::AircraftGeometryData allGeomData = geomExtractor.getGeometryData();

            // VSPGEOMTRYEXTRACTOR::GeomInfo fuselageType;

            VSPGEOMTRYEXTRACTOR::DiametersExtractor fuseExtractor;

            VSPGEOMTRYEXTRACTOR::FuselageDiametersAndXStation fuseData = fuseExtractor.extractFuselageDiameters(
                builder.getCommonData().getNameOfAircraft(),
                allGeomData,
                fus.length);

            double diameterAtHorizontalLocation;

            Interpolant fusDiameterInterpolator(fuseData.xStation, fuseData.allFuselageWidth, 1, RegressionMethod::LINEAR);

            diameterAtHorizontalLocation = fusDiameterInterpolator.getYValueFromRegression(horizontal.xloc);

            // OpenVsp gives us only the enatire half-wetted area of the wing
            wettedAreaExposedHorizontalTail = 2 * (results.WET_AREA - 0.5 * diameterAtHorizontalLocation);
        }

        else
        {

            wettedAreaExposedHorizontalTail = 2 * horizontal.planformArea * std::pow(std::cos(horizontal.averageDihedral / 57.3), 2.0);
        }

        // Contribution to the parasite drag due to flap deflecction

        for (size_t n = 0; n < horizontal.mov.type.size(); n++)
        {

            if (horizontal.mov.type[n] == 'e')
            {

                // Larghezza del segmento n-esimo
                double segmentSpan = (horizontal.mov.eta_outer[n] - horizontal.mov.eta_inner[n]) * horizontal.totalProjectedSpan;

                // cf/c medio del segmento = media tra inner e outer
                double cfcMean = 0.5 * (horizontal.mov.cf_c_inner[n] + horizontal.mov.cf_c_outer[n]);

                numeratorAverageChordRatio += cfcMean * segmentSpan;
                weightsOfWeightedAverageChordRatio += segmentSpan;

                etaInboardFlap = horizontal.mov.eta_inner[n];
                etaOutboardFlap = horizontal.mov.eta_outer[n];

                double inboardChord = ChordCalculator(horizontal).getChordAtEtaStation(etaInboardFlap, horizontal.typeOfWing);
                double outboardChord = ChordCalculator(horizontal).getChordAtEtaStation(etaOutboardFlap, horizontal.typeOfWing);

                // Half-flap-affected wing area.
                wingAreaAffectedByElevator += 0.5 * (inboardChord + outboardChord) * segmentSpan;
            }
        }

        averageChordRatio = numeratorAverageChordRatio / weightsOfWeightedAverageChordRatio;

        for (size_t i = 0; i < horizontal.mov.type.size(); i++)
        {

            if (horizontal.mov.type[i] == 'e')
            {

                if (horizontal.mov.defl[i] > 0)
                {
                    trimDrag = planformAreaHorizontal / wing.planformArea * (0.9 * std::pow(averageChordRatio, 1.38) * std::pow(std::sin(horizontal.mov.defl[i] * 57.3), 2.0));
                }

                break;
            }
        }

        if (settings.Mach > criticalMachNumber)
        {

            cd0HorizontalTail = kCrudFactor * mixedSkinFrictionHorizontalTail * formFactorHorizontalTail * interferenceFactorHorizontalTail * wettedAreaExposedHorizontalTail / wing.planformArea + deltaCDWave + trimDrag;
        }

        else
        {

            cd0HorizontalTail = kCrudFactor * mixedSkinFrictionHorizontalTail * formFactorHorizontalTail * interferenceFactorHorizontalTail * wettedAreaExposedHorizontalTail / wing.planformArea + trimDrag;
        }

        settings = settingsRestorer.getSettingsToRestore(); // Restore previous settings

        return cd0HorizontalTail;
    }

    // ======================= VERTICAL TAIL CD0 CALCULATION =======================

    /// @brief Calculates the vertical tail CD0
    /// @param designCruiseLiftCoefficient The Clcruise design of the vertical tail, default is not considered
    /// @param transitionPoint The transition point for the vertical tail, default is not considered (small laminar region) 
    double calculateCD0VerticalTail(double transitionPoint = 0.0)
    {

        RestoreSettings settingsRestorer; // Create an instance of RestoreSettings to automatically restore settings when going out of scope

        settingsRestorer.setSavePrevoiusSettings(settings);

        // Re-definiotn of the Reynoldns number based on the reference length of the body we are analysing
        auto [T, a, P, rho] = Atmosphere::atmosisa(settings.altitude); // Atmosisa like in MATLAB
        settings.ReCref = settings.Mach * a * vertical.MAC / Atmosphere::ISA::kinematicViscosity(settings.altitude);

        // Skin roughness factor calculation

        if (builder.getCommonData().getSkinRoughnessTypeVerticalTail() == SkinRoughnessType::CAMOUFLAGE_OR_ALUMINIUM)
        {
            skinRoughnessCoefficientVerticalTail = 3.33 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeVerticalTail() == SkinRoughnessType::SMOOTH_PAINT)
        {
            skinRoughnessCoefficientVerticalTail = 2.08 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeVerticalTail() == SkinRoughnessType::PRODUCTION_SHEET_METAL)
        {
            skinRoughnessCoefficientVerticalTail = 1.33 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeVerticalTail() == SkinRoughnessType::POLISHED_SHEET_METAL)
        {
            skinRoughnessCoefficientVerticalTail = 0.5 * 0.3048 * 1e-5;
        }

        else if (builder.getCommonData().getSkinRoughnessTypeVerticalTail() == SkinRoughnessType::SMOOTH_MOLDED_COMPOSITE)
        {
            skinRoughnessCoefficientVerticalTail = 0.17 * 0.3048 * 1e-5;
        }

        // Cutoff Re number calculation
        if (settings.Mach <= 0.6)
        {

            cutoffReynoldsNumberVerticalTail = 38.21 * std::pow(vertical.MAC / skinRoughnessCoefficientVerticalTail, 1.053);
        }

        else
        {

            cutoffReynoldsNumberVerticalTail = 44.62 * std::pow(vertical.MAC / skinRoughnessCoefficientVerticalTail, 1.053) * std::pow(settings.Mach, 1.16);
        }

        // Deriving a possible transition point based on Gudmundsson suggestion and Raymer

        if (builder.getCommonData().getAircraftCategory() == AircraftCategory::TRANSPORT_JET)
        {

            kCrudFactor = 1.10; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94

            if (transitionPoint == 0.0)
            {
                transitionPoint = 0.5 * (0.1 + 0.2); // Assumed transition point for transport jets, can be adjusted based on specific conditions
            }
        }

        else if (builder.getCommonData().getAircraftCategory() == AircraftCategory::GENERAL_AVIATION ||
                 builder.getCommonData().getAircraftCategory() == AircraftCategory::REGIONAL_TURBOPROP ||
                 builder.getCommonData().getAircraftCategory() == AircraftCategory::UAV)
        {

            kCrudFactor = 1.20; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94

            if (transitionPoint == 0.0)
            {
                transitionPoint = 0.5 * (0.1 + 0.2); // Assumed transition point for general aviation aircraft, can be adjusted based on specific conditions
            }
        }

        // Calculation of Skin Friction Coeffcient

        if (settings.ReCref > cutoffReynoldsNumberVerticalTail)
        {

            reynoldsNumber = cutoffReynoldsNumberVerticalTail;

            ficticiuousTransitionPoint = 36.9 * std::pow(transitionPoint, 0.625) * std::pow(reynoldsNumber, -0.375);

            // Mixed Laminar-Turbulent Skin Griction - Young's Method
            mixedSkinFrictionVerticalTail = 0.074 / std::pow(reynoldsNumber, 0.2) * std::pow(1 - (transitionPoint - ficticiuousTransitionPoint), 0.8);
        }

        else
        {

            reynoldsNumber = settings.ReCref;

            ficticiuousTransitionPoint = 36.9 * std::pow(transitionPoint, 0.625) * std::pow(reynoldsNumber, -0.375);

            // Mixed Laminar-Turbulent Skin Griction - Young's Method
            mixedSkinFrictionVerticalTail = 0.074 / std::pow(reynoldsNumber, 0.2) * std::pow(1 - (transitionPoint - ficticiuousTransitionPoint), 0.8);
        }

        // Form Factor calculation - Shevell's Method

        formFactorVerticalTail = 1 + ((2 - std::pow(settings.Mach, 2.0)) * std::cos(vertical.sweepC2 / 57.3)) / (std::sqrt(1 - std::pow(settings.Mach, 2.0) * std::pow(std::cos(vertical.sweepC2 / 57.3), 2.0))) * (vertical.averageThicknessToChordRatio) + 100 * std::pow(vertical.averageThicknessToChordRatio, 4.0);

        // Interference Factor calculation - Gundundsson - General Aviation Aircrfat Design, Aplied Methods and Procedures, pag. 771

        if (builder.getCommonData().getTypeOfTail() == TypeOfTail::CONVENTIONAL_TAIL)
        {

            planformAreaVertical = vertical.planformArea;

            interferenceFactorVerticalTail = 0.5 * (1.04 + 1.05);
        }

        else if (builder.getCommonData().getTypeOfTail() == TypeOfTail::CRUCIFORM_TAIL)
        {

            planformAreaVertical = vertical.planformArea;

            interferenceFactorVerticalTail = 1.06;
        }

        else if (builder.getCommonData().getTypeOfTail() == TypeOfTail::V_TAIL ||
                 builder.getCommonData().getTypeOfTail() == TypeOfTail::V_REV_TAIL)
        {

            planformAreaVertical = vertical.planformArea * std::pow(std::sin(vertical.averageDihedral / 57.3), 2.0);

            interferenceFactorVerticalTail = 1.03;
        }

        else if (builder.getCommonData().getTypeOfTail() == TypeOfTail::H_TAIL)
        {

            planformAreaVertical = 2 * vertical.planformArea;

            interferenceFactorHorizontalTail = 2.0 * (1.08 + 1.13 + 1.06) / 3.0;
        }

        else if (builder.getCommonData().getTypeOfTail() == TypeOfTail::T_TAIL)
        {

            planformAreaVertical = vertical.planformArea;

            interferenceFactorVerticalTail = 1.04;
        }

        else if (builder.getCommonData().getTypeOfTail() == TypeOfTail::TRIPLE_TAIL)
        {

            planformAreaVertical = 3 * vertical.planformArea;

            interferenceFactorVerticalTail = 3.30;
        }

        // Swet wing calculation

        WETTEDAREA::WettedArea wettedAreaCalculatorWing(nameOfAircraft + "_GetGeomOfAircraft.vspscript", aircraftData);

        wettedAreaCalculatorWing.getAllGeoms(nameOfAircraft, false);

        wettedAreaCalculatorWing.executeAndCaptureGeomIds();

        wettedAreaCalculatorWing.CalculateWettedArea(nameOfAircraft, "WettedAreaVertical.vspscript", vertical.id);

        wettedAreaCalculatorWing.executeAndCaptureWettedArea("WettedAreaVertical.vspscript");

        // Access and display results
        const auto &results = wettedAreaCalculatorWing.getWettedAreaResults();

        if (builder.getCommonData().getTypeOfTail() == TypeOfTail::TRIPLE_TAIL)
        {

            // OpenVsp gives us only the enatire half-wetted area of the wing
            wettedAreaExposedVerticalTail = 3 * (results.WET_AREA);
        }

        else if (builder.getCommonData().getTypeOfTail() == TypeOfTail::V_TAIL ||
                 builder.getCommonData().getTypeOfTail() == TypeOfTail::V_REV_TAIL)
        {

            wettedAreaExposedVerticalTail = 2 * vertical.planformArea * std::pow(std::cos(vertical.averageDihedral / 57.3), 2.0);
        }

        else
        {

            wettedAreaExposedVerticalTail = 2 * vertical.planformArea;
        }

        // Contribution to the parasite drag due to flap deflecction

        for (size_t n = 0; n < vertical.mov.type.size(); n++)
        {

            if (vertical.mov.type[n] == 'r')
            {

                // Larghezza del segmento n-esimo
                double segmentSpan = (vertical.mov.eta_outer[n] - vertical.mov.eta_inner[n]) * vertical.totalProjectedSpan;

                // cf/c medio del segmento = media tra inner e outer
                double cfcMean = 0.5 * (vertical.mov.cf_c_inner[n] + vertical.mov.cf_c_outer[n]);

                numeratorAverageChordRatio += cfcMean * segmentSpan;
                weightsOfWeightedAverageChordRatio += segmentSpan;

                etaInboardFlap = vertical.mov.eta_inner[n];
                etaOutboardFlap = vertical.mov.eta_outer[n];

                double inboardChord = ChordCalculator(vertical).getChordAtEtaStation(etaInboardFlap, vertical.typeOfWing);
                double outboardChord = ChordCalculator(vertical).getChordAtEtaStation(etaOutboardFlap, vertical.typeOfWing);

                // Half-flap-affected wing area.
                wingAreaAffectedByRudder += 0.5 * (inboardChord + outboardChord) * segmentSpan;
            }
        }

        averageChordRatio = numeratorAverageChordRatio / weightsOfWeightedAverageChordRatio;

        for (size_t i = 0; i < vertical.mov.type.size(); i++)
        {

            if (vertical.mov.type[i] == 'r')
            {

                if (vertical.mov.defl[i] > 0)
                {

                    if (builder.getWingData().getTypeOfFlap() == TypeOfFlap::SLOTTED_FLAP)
                    {
                        // Contribution due to flap-chord-ratio
                        Interpolant2D flapChordRatioEffectInterpolator(4, RegressionMethod::POLYNOMIAL);

                        flapChordRatioEffectInterpolator.addCurve(0.12, x_delta1_slotted_tc012, y_delta1_slotted_tc012);
                        flapChordRatioEffectInterpolator.addCurve(0.21, x_delta1_slotted_tc021, y_delta1_slotted_tc021);
                        flapChordRatioEffectInterpolator.addCurve(0.30, x_delta1_slotted_tc021, y_delta1_slotted_tc021);

                        contributionDueToFlapChordRatio = flapChordRatioEffectInterpolator.interpolate(averageChordRatio, vertical.averageThicknessToChordRatio);

                        // Contribution due to flap deflection

                        Interpolant2D flapDeflectionEffectInterpolator(6, RegressionMethod::POLYNOMIAL);

                        flapDeflectionEffectInterpolator.addCurve(0.12, x_delta2_slotted_tc012, y_delta2_slotted_tc012);
                        flapDeflectionEffectInterpolator.addCurve(0.21, x_delta2_slotted_tc021, y_delta2_slotted_tc021);
                        flapDeflectionEffectInterpolator.addCurve(0.30, x_delta2_slotted_tc030, y_delta2_slotted_tc030);

                        contributionDueToFlapDeflection = flapDeflectionEffectInterpolator.interpolate(std::abs(vertical.mov.defl[i]), vertical.averageThicknessToChordRatio);
                    }
                    else if (builder.getWingData().getTypeOfFlap() == TypeOfFlap::PLAIN_FLAP ||
                             builder.getWingData().getTypeOfFlap() == TypeOfFlap::SPLIT_FLAP)
                    {
                        // Contribution due to flap-chord-ratio
                        Interpolant2D flapChordRatioEffectInterpolator(4, RegressionMethod::POLYNOMIAL);

                        flapChordRatioEffectInterpolator.addCurve(0.12, x_delta1_plain_tc012, y_delta1_plain_tc012);
                        flapChordRatioEffectInterpolator.addCurve(0.21, x_delta1_plain_tc021, y_delta1_plain_tc021);
                        flapChordRatioEffectInterpolator.addCurve(0.30, x_delta1_plain_tc030, y_delta1_plain_tc030);

                        contributionDueToFlapChordRatio = flapChordRatioEffectInterpolator.interpolate(averageChordRatio, vertical.averageThicknessToChordRatio);

                        // Contribution due to flap deflection

                        if (builder.getWingData().getTypeOfFlap() == TypeOfFlap::PLAIN_FLAP)
                        {

                            Interpolant2D flapDeflectionEffectInterpolator(3, RegressionMethod::POLYNOMIAL);

                            flapDeflectionEffectInterpolator.addCurve(0.12, x_delta2_plain_all_tc, y_delta2_plain_all_tc);
                            flapDeflectionEffectInterpolator.addCurve(0.21, x_delta2_plain_all_tc, y_delta2_plain_all_tc);
                            flapDeflectionEffectInterpolator.addCurve(0.30, x_delta2_plain_all_tc, y_delta2_plain_all_tc);

                            contributionDueToFlapDeflection = flapDeflectionEffectInterpolator.interpolate(std::abs(vertical.mov.defl[i]), vertical.averageThicknessToChordRatio);
                        }

                        else
                        {

                            Interpolant2D flapDeflectionEffectInterpolator(3, RegressionMethod::POLYNOMIAL);
                            flapDeflectionEffectInterpolator.addCurve(0.12, x_delta2_split_tc012, y_delta2_split_tc012);
                            flapDeflectionEffectInterpolator.addCurve(0.21, x_delta2_split_tc021, y_delta2_split_tc021);
                            flapDeflectionEffectInterpolator.addCurve(0.30, x_delta2_split_tc030, y_delta2_split_tc030);

                            contributionDueToFlapDeflection = flapDeflectionEffectInterpolator.interpolate(std::abs(vertical.mov.defl[i]), vertical.averageThicknessToChordRatio);
                        }
                    }

                    deltaCd0DueToRudderDeflection = contributionDueToFlapChordRatio * contributionDueToFlapDeflection * (2 * wingAreaAffectedByRudder / wing.planformArea);
                }

                break;
            }
        }

        cd0VerticalTail = kCrudFactor * mixedSkinFrictionVerticalTail * formFactorVerticalTail * interferenceFactorVerticalTail * wettedAreaExposedVerticalTail / wing.planformArea + deltaCd0DueToRudderDeflection;

        settings = settingsRestorer.getSettingsToRestore(); // Restore previous settings

        return cd0VerticalTail;
    }

    // ======================= FUSIFORM BODY TAIL CD0 CALCULATION =======================

    /// @brief Calculates the CD0 of the fusiform body, including the contribution of the windscreen and the skin roughness
    /// @param typeOfBody The type of body we are analysing, can be "fuselage", "nacelle" or "boom"
    /// @param transitionPoint The transition point for the fusiform body, default is not considered (fully turbulent flow)
    double calculateCD0FusiFormBody(std::string typeOfBody, double transitionPoint = 0.0)
    {

        RestoreSettings settingsRestorer; // Create an instance of RestoreSettings to automatically restore settings when going out of scope

        settingsRestorer.setSavePrevoiusSettings(settings);


        // Re-definiotn of the Reynoldns number based on the reference length of the body we are analysing
        auto [T, a, P, rho] = Atmosphere::atmosisa(settings.altitude); // Atmosisa like in MATLAB

        if (typeOfBody == fus.id)
        {

            settings.ReCref = settings.Mach * a * fus.length / Atmosphere::ISA::kinematicViscosity(settings.altitude);
        }
        else if (typeOfBody == nac.id)
        {

            settings.ReCref = settings.Mach * a * nac.length / Atmosphere::ISA::kinematicViscosity(settings.altitude);
        }

        else if (typeOfBody == boom.id)
        {

            settings.ReCref = settings.Mach * a * boom.length.front() / Atmosphere::ISA::kinematicViscosity(settings.altitude);
        }

        // Skin roughness factor calculation

        if (typeOfBody == fus.id)
        {

            VSPGEOMTRYEXTRACTOR::GeometryExtractor geomExtractor;
            geomExtractor.extractAllGeoms(builder.getCommonData().getNameOfAircraft(),
                                          builder.getCommonData().getNameOfAircraft() + "_AllGeomsToCd0.vspscript");

            VSPGEOMTRYEXTRACTOR::AircraftGeometryData allGeomData = geomExtractor.getGeometryData();

            // VSPGEOMTRYEXTRACTOR::GeomInfo fuselageType;

            VSPGEOMTRYEXTRACTOR::DiametersExtractor fuseExtractor;

            VSPGEOMTRYEXTRACTOR::FuselageDiametersAndXStation fuseData = fuseExtractor.extractFuselageDiameters(
                builder.getCommonData().getNameOfAircraft(),
                allGeomData,
                fus.length);

            maximumExposedDiameterFusiFormBody = *std::max_element(fuseData.allFuselageWidth.begin(), fuseData.allFuselageWidth.end());

             // Wind screen deltaCd0 calculation

            if (builder.getCommonData().getWindScreenType() == WindScreenType::FLAT_WINDSCREEN_WITH_PROTRUDING_FRAME)
            {
                deltaCd0DueToWindScreenType = 0.016 * M_PI * std::pow(maximumExposedDiameterFusiFormBody/2, 2.0) / wing.planformArea;
            }
            else if (builder.getCommonData().getWindScreenType() == WindScreenType::FLAT_WINDSCREEN_WITH_FLUSH_FRAME)
            {
                deltaCd0DueToWindScreenType = 0.011 * M_PI * std::pow(maximumExposedDiameterFusiFormBody/2, 2.0) / wing.planformArea;
            }

            else if (builder.getCommonData().getWindScreenType() == WindScreenType::CURVED_WINDSCREEN_WITH_SHARP_UPPER_EDGE)
            {
                deltaCd0DueToWindScreenType = 0.005 * M_PI * std::pow(maximumExposedDiameterFusiFormBody/2, 2.0) / wing.planformArea;
            }

            else if (builder.getCommonData().getWindScreenType() == WindScreenType::CURVED_WINDSCREEN_WITH_ROUNDED_UPPER_EDGE)
            {
                deltaCd0DueToWindScreenType = 0.002 * M_PI * std::pow(maximumExposedDiameterFusiFormBody/2, 2.0) / wing.planformArea;
            }


        

            if (builder.getCommonData().getSkinRoughnessTypeFuselage() == SkinRoughnessType::CAMOUFLAGE_OR_ALUMINIUM)
            {
                skinRoughnessCoefficientFusiFormBody = 3.33 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeFuselage() == SkinRoughnessType::SMOOTH_PAINT)
            {
                skinRoughnessCoefficientFusiFormBody = 2.08 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeFuselage() == SkinRoughnessType::PRODUCTION_SHEET_METAL)
            {
                skinRoughnessCoefficientFusiFormBody = 1.33 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeFuselage() == SkinRoughnessType::POLISHED_SHEET_METAL)
            {
                skinRoughnessCoefficientFusiFormBody = 0.5 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeFuselage() == SkinRoughnessType::SMOOTH_MOLDED_COMPOSITE)
            {
                skinRoughnessCoefficientFusiFormBody = 0.17 * 0.3048 * 1e-5;
            }

            // Cutoff Re number calculation
            if (settings.Mach <= 0.6)
            {

                cutoffReynoldsNumberFusiFormBody = 38.21 * std::pow(fus.length / skinRoughnessCoefficientFusiFormBody, 1.053);
            }

            else
            {

                cutoffReynoldsNumberFusiFormBody = 44.62 * std::pow(fus.length / skinRoughnessCoefficientFusiFormBody, 1.053) * std::pow(settings.Mach, 1.16);
            }
        }

        else if (typeOfBody == nac.id)
        {

            if (nac.nacellePresetName == "GeneralTurbofan")
            {
                maximumExposedDiameterFusiFormBody = nac.diameter;
            }

            else
            {

                VSPGEOMTRYEXTRACTOR::GeometryExtractor geomExtractor;
                geomExtractor.extractAllGeoms(builder.getCommonData().getNameOfAircraft(),
                                              builder.getCommonData().getNameOfAircraft() + "_AllGeomsToCd0.vspscript");

                VSPGEOMTRYEXTRACTOR::AircraftGeometryData allGeomData = geomExtractor.getGeometryData();

                VSPGEOMTRYEXTRACTOR::DiametersExtractor nacelleExtractor;

                VSPGEOMTRYEXTRACTOR::NacelleDiametersAndXStation nacelleData = nacelleExtractor.extractNacelleDiameters(
                    builder.getCommonData().getNameOfAircraft(),
                    allGeomData,
                    nac.length);

                maximumExposedDiameterFusiFormBody = *std::max_element(nacelleData.allNacelleWidth.begin(), nacelleData.allNacelleWidth.end());
            }

            if (builder.getCommonData().getSkinRoughnessTypeNacelle() == SkinRoughnessType::CAMOUFLAGE_OR_ALUMINIUM)
            {
                skinRoughnessCoefficientFusiFormBody = 3.33 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeNacelle() == SkinRoughnessType::SMOOTH_PAINT)
            {
                skinRoughnessCoefficientFusiFormBody = 2.08 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeNacelle() == SkinRoughnessType::PRODUCTION_SHEET_METAL)
            {
                skinRoughnessCoefficientFusiFormBody = 1.33 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeNacelle() == SkinRoughnessType::POLISHED_SHEET_METAL)
            {
                skinRoughnessCoefficientFusiFormBody = 0.5 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeNacelle() == SkinRoughnessType::SMOOTH_MOLDED_COMPOSITE)
            {
                skinRoughnessCoefficientFusiFormBody = 0.17 * 0.3048 * 1e-5;
            }

            // Cutoff Re number calculation
            if (settings.Mach <= 0.6)
            {

                cutoffReynoldsNumberFusiFormBody = 38.21 * std::pow(nac.length / skinRoughnessCoefficientFusiFormBody, 1.053);
            }

            else
            {

                cutoffReynoldsNumberFusiFormBody = 44.62 * std::pow(nac.length / skinRoughnessCoefficientFusiFormBody, 1.053) * std::pow(settings.Mach, 1.16);
            }
        }

        else if (typeOfBody == boom.id)
        {

            maximumExposedDiameterFusiFormBody = boom.length.front()/boom.fineratio.front();

            if (builder.getCommonData().getSkinRoughnessTypeBoom() == SkinRoughnessType::CAMOUFLAGE_OR_ALUMINIUM)
            {
                skinRoughnessCoefficientFusiFormBody = 3.33 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeBoom() == SkinRoughnessType::SMOOTH_PAINT)
            {
                skinRoughnessCoefficientFusiFormBody = 2.08 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeBoom() == SkinRoughnessType::PRODUCTION_SHEET_METAL)
            {
                skinRoughnessCoefficientFusiFormBody = 1.33 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeBoom() == SkinRoughnessType::POLISHED_SHEET_METAL)
            {
                skinRoughnessCoefficientFusiFormBody = 0.5 * 0.3048 * 1e-5;
            }

            else if (builder.getCommonData().getSkinRoughnessTypeBoom() == SkinRoughnessType::SMOOTH_MOLDED_COMPOSITE)
            {
                skinRoughnessCoefficientFusiFormBody = 0.17 * 0.3048 * 1e-5;
            }

            // Cutoff Re number calculation
            if (settings.Mach <= 0.6)
            {

                cutoffReynoldsNumberFusiFormBody = 38.21 * std::pow(boom.length.front() / skinRoughnessCoefficientFusiFormBody, 1.053);
            }

            else
            {

                cutoffReynoldsNumberFusiFormBody = 44.62 * std::pow(boom.length.front() / skinRoughnessCoefficientFusiFormBody, 1.053) * std::pow(settings.Mach, 1.16);
            }
        }

        // Deriving a possible transition point based on Gudmundsson suggestion and Raymer

        if (builder.getCommonData().getAircraftCategory() == AircraftCategory::TRANSPORT_JET)
        {

            kCrudFactor = 1.10; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94

            if (transitionPoint == 0.0)
            {
                transitionPoint = 0.5 * (0.1 + 0.2); // Assumed transition point for transport jets, can be adjusted based on specific conditions
            }
        }

        else if (builder.getCommonData().getAircraftCategory() == AircraftCategory::GENERAL_AVIATION ||
                 builder.getCommonData().getAircraftCategory() == AircraftCategory::REGIONAL_TURBOPROP ||
                 builder.getCommonData().getAircraftCategory() == AircraftCategory::UAV)
        {

            kCrudFactor = 1.20; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94

            if (transitionPoint == 0.0)
            {
                transitionPoint = 0.5 * (0.1 + 0.2); // Assumed transition point for general aviation aircraft, can be adjusted based on specific conditions
            }
        }

        // Calculation of Skin Friction Coeffcient

        if (settings.ReCref > cutoffReynoldsNumberFusiFormBody)
        {

            reynoldsNumber = cutoffReynoldsNumberFusiFormBody;

            if (reynoldsNumber < 1e7)
            {
                // Fully Turbulent Skin Friction
                fullyTurbulentSkinFrictionFusiFormBody = 0.0315 / std::pow(reynoldsNumber, 1.0 / 7.0);
            }

            else if (reynoldsNumber >= 1e7 && reynoldsNumber <= 1e9)
            {

                // Prandtl-Schlichting - più accurata per Re > 10^7
                fullyTurbulentSkinFrictionFusiFormBody = 0.455 / std::pow(std::log10(reynoldsNumber), 2.58);
            }
        }

        else
        {

            reynoldsNumber = settings.ReCref;

            if (reynoldsNumber < 1e7)
            {
                // Fully Turbulent Skin Friction
                fullyTurbulentSkinFrictionFusiFormBody = 0.0315 / std::pow(reynoldsNumber, 1.0 / 7.0);
            }

            else if (reynoldsNumber >= 1e7 && reynoldsNumber <= 1e9)
            {

                // Prandtl-Schlichting - più accurata per Re > 10^7
                fullyTurbulentSkinFrictionFusiFormBody = 0.455 / std::pow(std::log10(reynoldsNumber), 2.58);
            }
        }

        // From Factor due to fineness ratio - Nicolai, Raymer, Roskam

        if (typeOfBody == fus.id)
        {

            formFactorFusiFormBody = 1 + 60 * std::pow(fus.length / fus.diameter, -3.0) + fus.length / (400 * fus.diameter);
        }

        else if (typeOfBody == boom.id)
        {

            formFactorFusiFormBody = 1 + 60 * std::pow(boom.fineratio.front(), -3.0) + boom.fineratio.front() / 400;
        }

        else if (typeOfBody == nac.id)
        {

            // Rayamer suggestion

            if (nac.nacellePresetName == "GeneralTurbofan")
            {
                formFactorFusiFormBody = 1 + 0.35 * std::pow(nac.length / nac.diameter, -1.0);
            }

            else
            {

                formFactorFusiFormBody = 1 + 0.35 * std::pow(maximumExposedDiameterFusiFormBody / nac.length, -1.0);
            }
        }

        // Form factor due to compressibility effect

        factorDueToCompressibilityFusiFormBody = 1 - (0.08 * std::pow(settings.Mach, 1.45));

        // Swet fusi-form body calculation

        WETTEDAREA::WettedArea wettedAreaCalculatorWing(nameOfAircraft + "_GetGeomOfAircraft.vspscript", aircraftData);

        wettedAreaCalculatorWing.getAllGeoms(nameOfAircraft, false);

        wettedAreaCalculatorWing.executeAndCaptureGeomIds();

        if (typeOfBody == fus.id)
        {

            wettedAreaCalculatorWing.CalculateWettedArea(nameOfAircraft, "WettedAreaFuselage.vspscript", fus.id);

            wettedAreaCalculatorWing.executeAndCaptureWettedArea("WettedAreaFuselage.vspscript");

            // Access and display results
            const auto &results = wettedAreaCalculatorWing.getWettedAreaResults();
            wettedAreaExposedFusiFormBodyFuselage = results.WET_AREA;
        }

        else if (typeOfBody == boom.id)
        {

            wettedAreaCalculatorWing.CalculateWettedArea(nameOfAircraft, "WettedAreaBoom.vspscript", boom.id);

            wettedAreaCalculatorWing.executeAndCaptureWettedArea("WettedAreaBoom.vspscript");

            // Access and display results
            const auto &results = wettedAreaCalculatorWing.getWettedAreaResults();
            wettedAreaExposedFusiFormBodyFuselage = builder.getCommonData().getNumberOfBooms() * results.WET_AREA;
        }
        else if (typeOfBody == nac.id)
        {

            wettedAreaCalculatorWing.CalculateWettedArea(nameOfAircraft, "WettedAreaNacelle.vspscript", nac.id + "_" + std::to_string(builder.getEngineData().getNumberOfEngines()));

            wettedAreaCalculatorWing.executeAndCaptureWettedArea("WettedAreaNacelle.vspscript");

            // Access and display results
            const auto &results = wettedAreaCalculatorWing.getWettedAreaResults();
            wettedAreaExposedFusiFormBodyFuselage = builder.getEngineData().getNumberOfEngines() * results.WET_AREA;
        }

       

        cd0FusiFormBody = kCrudFactor * fullyTurbulentSkinFrictionFusiFormBody * formFactorFusiFormBody * factorDueToCompressibilityFusiFormBody * wettedAreaExposedFusiFormBodyFuselage / wing.planformArea + deltaCd0DueToWindScreenType;

        settings = settingsRestorer.getSettingsToRestore(); // Restore previous settings

        return cd0FusiFormBody;
    }

    double calculateCD0LandingGears(double MTOM)
    {

        if (builder.getCommonData().getAircraftCategory() == AircraftCategory::TRANSPORT_JET)
        {

            kCrudFactor = 1.10; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94
        }

        else
        {

            kCrudFactor = 1.20; // Sadraey Aircrfat Performance, An Engineering Approach, pag.94
        }

        // Derived from ESDU 79015

        if (builder.getLandingGearsData().getIsRetractableLandingGear())
        {

            for (size_t i = 0; i < wing.mov.type.size(); i++)
            {

                if (wing.mov.type[i] == 'f')
                {

                    if (wing.mov.defl[i] > 0)
                    {

                        kFactorDueToFlapDeflection = (0.57 - 0.26 * wing.mov.defl[i] / builder.getWingData().getMaximumFalpDeflection()) * 1e-3;

                      

                            cd0LandingGears = kCrudFactor * kFactorDueToFlapDeflection * std::pow(MTOM, 0.785) / wing.planformArea;
                       

                            cd0LandingGears = kCrudFactor * kFactorDueToFlapDeflection * std::pow(MTOM, 0.785) / wing.planformArea;
                        
                    }

                    else
                    {

                        cd0LandingGears = kCrudFactor *0.57 * 1e-3 * std::pow(MTOM, 0.785) / wing.planformArea;
                    }

                    break;
                }
            }
        }
        else
        {

            // Sadraey suggestion : Aircrfat Performance an Engineriing Approach
            // Assumptions: The strut are cicular rods, the wheels are approximated as flat plates with frontal area equal to diameter*width, and the flow is assumed to be either fully laminar or fully turbulent based on the Reynolds number. The CD values for the struts are taken from Sadraey's book, which provides empirical data for circular rods in different flow regimes.

            wheelNoseFrontalArea = builder.getLandingGearsData().getNoseWheelDiameter() * builder.getLandingGearsData().getNoseWheelWidth();
            wheelMainFrontalArea = builder.getLandingGearsData().getMainWheelDiameter() * builder.getLandingGearsData().getMainWheelWidth();
            strutAreaNose = 2 * M_PI * 0.5 * builder.getLandingGearsData().getStrutNoseDiameter() * builder.getLandingGearsData().getStrutLengthNose();
            strutAreaMain = 2 * M_PI * 0.5 * builder.getLandingGearsData().getStrutMainDiameter() * builder.getLandingGearsData().getStrutLengthMain();

            if (settings.ReCref > laminarReynolds)
            {

                cd0NoseWheel = builder.getLandingGearsData().getNumberOfNoseWheels() * wheelNoseFrontalArea / wing.planformArea;
                cd0MainWheel = builder.getLandingGearsData().getNumberOfMainWheels() * wheelMainFrontalArea / wing.planformArea;
                cd0NoseStrut = builder.getLandingGearsData().getNumberOfNoseStruts() * circularRodCDTurbulentFlowSadraey * strutAreaNose / wing.planformArea;
                cd0MainStrut = builder.getLandingGearsData().getNumberOfMainStruts() * circularRodCDTurbulentFlowSadraey * strutAreaMain / wing.planformArea;
            }

            else
            {

                cd0NoseWheel = builder.getLandingGearsData().getNumberOfNoseWheels() * wheelNoseFrontalArea / wing.planformArea;
                cd0MainWheel = builder.getLandingGearsData().getNumberOfMainWheels() * wheelMainFrontalArea / wing.planformArea;
                cd0NoseStrut = builder.getLandingGearsData().getNumberOfNoseStruts() * circularRodCDLaminarFlowSadraey * strutAreaNose / wing.planformArea;
                cd0MainStrut = builder.getLandingGearsData().getNumberOfMainStruts() * circularRodCDLaminarFlowSadraey * strutAreaMain / wing.planformArea;
            }

            cd0LandingGears = kCrudFactor*(cd0NoseWheel + cd0MainWheel + cd0NoseStrut + cd0MainStrut);
        }

        return cd0LandingGears;
    }

    /// @brief Calculates the total CD0 of the aircraft by summing the contributions of all the components, including the wing, canard, horizontal tail, vertical tail, fuselage, nacelle, boom and landing gears (if deployed)
    /// @param isLandingGearDeployed A boolean flag to indicate whether the landing gears are deployed or not, default is false (landing gears retracted)
    double getTotalCD0Aircraft (bool isLandingGearDeployed = false ) {

        cd0Wing = calculateCD0Wing();

        if (builder.getCommonData().getHasCanard())
        {
            cd0Canard = calculateCD0Canard();
        }
        cd0HorizontalTail = calculateCD0HorizontalTail();
        cd0VerticalTail = calculateCD0VerticalTail();
        cd0Fuselage = calculateCD0FusiFormBody(fus.id);
        cd0Nacelle = calculateCD0FusiFormBody(nac.id);

        if (builder.getCommonData().getHasBoom())
        {
            cd0Boom = calculateCD0FusiFormBody(boom.id);
        }

        if (isLandingGearDeployed)
        {
            cd0LandingGears = calculateCD0LandingGears(builder.getCommonData().getWTO());
        }

        totalCD0Aircraft = cd0Wing + cd0Canard + cd0HorizontalTail + cd0VerticalTail + cd0Fuselage + cd0Nacelle + cd0Boom + cd0LandingGears;

        return totalCD0Aircraft;


    }
};
#pragma once
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#include <string>
#include <algorithm>
#include "VSPScriptGenerator.h"
#include "VSPGEOMETRYEXTRACTOR.h"
#include "DELTAXANDDIAMETERS.h"
#include "Interpolant.h"
#include "Interpolant2D.h"
#include "PLOT.h"
#include "ODE45.h"
#include "BASEAIRCRAFTDATA.h"
#include "WINGBASEDATA.h"
#include "FUSELAGEBASEDATA.h"
#include "ENGINEBASEDATA.h"
#include "BUILDAIRCRAFT.h"
#include "COGCALCULATOR.h"
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
#include "RegressionMethod.h"
#include "ConvDensity.h"
#include "ConvLength.h"
#include "ConvArea.h"
#include "ConvPressure.h"
#include "RestoreSettings.h"
#include "SILENTORCOMPONENT.h"

namespace LONGITUDINAL_STABILITY
{

    struct LongitudinalStabilityDerivatives
    {
        double deltaCmDeltaAlphaAircraft = 0.0;
        double deltaCmDeltaClAircraft = 0.0;
        double deltaCmDeltaElevatorDeflection = 0.0;
        double deltaCmDeltaIncidenceHorizontalTail = 0.0;
        double deltaCLDeltaElevatorDeflection = 0.0;
        double deltaCLDeltaIncidenceHorizontalTail = 0.0;

        double liftSlopeAircraft = 0.0;

        double neutralPointAsFractionOfMAC = 0.0;
    };

    struct LongitudinalAerodynamicCoefficients
    {

        double pitchingMomentAtZeroAoA = 0.0;
        double liftCoefficientAtZeroAoA = 0.0;
    };

    struct LongitudinalStabilityDerivativesToSingleComponent
    {

        double deltaCmClWingContribution = 0.0;
        double deltaCmClCanardContribution = 0.0;
        double deltaCmClHorizontalTailContribution = 0.0;
        double deltaCmClFuselageContribution = 0.0;
        double deltaCmClNacelleContribution = 0.0;

        double deltaCmDeltaAlphaWing = 0.0;
        double deltaCmDeltaAlphaCanard = 0.0;
        double deltaCmDeltaAlphaHorizontalTail = 0.0;
        double deltaCmDeltaAlphaFuselage = 0.0;
        double deltaCmDeltaAlphaNacelle = 0.0;
    };

    class LongitudinalStabilityCalculator
    {

    protected:
        BuildAircraft &builder;
        COG::COGDATA cogData;
        COG::COGDATA cogComponents;

    private:
        VSP::Wing &wing;  // Reference to the wing object
        VSP::Wing *canard;
        VSP::Wing &horizontalTail;
        VSP::Wing *verticalTail;
        VSP::Fuselage fuselage;
        VSP::Nacelle nacelle;
        VSP::Disk disk;
        VSP::Aircraft aircraftInfo;
        VSP::AeroSettings settings;
        RegressionMethod regressionMethod;
        LONGITUDINAL_STABILITY::LongitudinalStabilityDerivatives aircraftLongitudinalDerivatives;
        LONGITUDINAL_STABILITY::LongitudinalStabilityDerivativesToSingleComponent singleComponentsDerivatives;
        LONGITUDINAL_STABILITY::LongitudinalAerodynamicCoefficients longitudinalAerodynamicCoefficients;
        std::string nameOfAircraft;

        double deltaCmClWingContribution = 0.0;
        double deltaCmClHorizontalTailContribution = 0.0;
        double deltaCmClCanardContribution = 0.0;
        double deltaCmClFuselageContribution = 0.0;
        double deltaCmClNacelleContribution = 0.0;

        double deltaCmDeltaAlphaWing = 0.0;
        double deltaCmDeltaAlphaHorizontalTail = 0.0;
        double deltaCmDeltaAlphaCanard = 0.0;
        double deltaCmDeltaAlphaFuselage = 0.0;
        double deltaCmDeltaAlphaNacelle = 0.0;

        double deltaCmDeltaAlphaAircraft = 0.0;
        double deltaCmDeltaClAircraft = 0.0;
        double deltaCmDeltaElevatorDeflection = 0.0;
        double deltaCmDeltaIncidenceHorizontalTail = 0.0;
        double deltaCLDeltaElevatorDeflection = 0.0;
        double deltaCLDeltaIncidenceHorizontalTail = 0.0;

        double liftSlopeAircraft = 0.0;

        double pitchingMomentAtZeroAoA = 0.0;
        double liftCoefficientAtZeroAoA = 0.0;

        int foundIndexAtZeroAoA = -1;

        std::vector<double> pitchingMomentAtZeroAoAVector;
        std::vector<double> liftCoefficientAtZeroAoAVector;

        double xTEWing = 0.0;
        double etaInner = 0.0;
        double etaOuter = 0.0;
        std::vector<double> flapSpanFactorKb;
        double numeratorAverageChordRatio = 0.0;
        double weightsOfWeightedAverageChordRatio = 0.0;
        double factorToAccountHorizontalTailContributionAndDownwash = 0.0;
        double averageChordRatio = 0.0;
        double factorKbInner = 0.0;
        double factorKbOuter = 0.0;
        double thetaFactor = 0.0;
        double twoDimensionalFlapEffectivness = 0.0; // alphaCl - DATCOM -Roskam method
        double tauFlap = 0.0;
        std::vector<double> ratioEffectivnessFactorKc;
        double wingMACFeet = 0.0;
        double xCGWingBar = 0.0;
        double xACWingBar = 0.0;
        double xCGCanardBar = 0.0;
        double xCGHorizontalTailBar = 0.0;
        double factorRToHorizontalTail = 0.0;
        double projectedVeeTailSurfaceToEquivalentHorizontal = 0.0;
        std::vector<double> downWashAngle;
        double downWashGradient = 0.0;
        double downWashGradientOnWingDueToCanard = 0.0;
        double distanceFromQuarterRootChordWingToCanard = 0.0;
        double wingAspectRatioToLambdaFunction = 0.0;
        double y0 = 0.0;
        double integrationStep = 0.0;
        double volumetricRatio = 0.0;
        double volumetricRatioCanard = 0.0;
        double tailArmCanard = 0.0;
        double etaMinimumConventionalTail = 0.85;
        double etaMaximumConventionalTail = 0.95;
        double summationFuselageContribution = 0.0;
        double gradientToLocalFlowAngleDueToUpWash = 0.0; // dBeta/dAlpha used in Perkins that is an upwash gradient for all the body ahead of the wing, and becomes a downwash gradient for all bodies behind the wing.
        double distanceFromeTEWingToQuarterChordMacHorizontal = 0.0;
        int idxNoseEnd = -1;   // Last nose section index
        int idxTailStart = -1; // First tail section index
        std::vector<double> xCoordinatePerkinsFront;
        std::vector<double> xCoordinatePerkinsRear;
        std::vector<double> diametersFront; // Fornt diamters of the fuselage, ordered from nose to LE wing
        std::vector<double> diametersRear;  // Rear diameters of the fuselage, ordered from TE wing to tail end
        std::vector<double> x1CentroidDistancesFront;
        std::vector<double> x1CentroidDistancesRear;
        int numSubdivisionsFront = 6;
        int numSubdivisionsRear = 6;
        double distanceNoseToWing = 0.0;
        double stepFront = 0.0;
        double distanceWingToTail = 0.0;
        double stepRear = 0.0;
        double dynamicPressureFreeStream = 0.0;
        double psfDynamicPressure = 0.0;
        double contributionOfFuselageWidthOnTheWingPitchingMoment = 0.0;
        double rootChordFeet = 0.0;
        double fuselageDiameterFeet = 0.0;
        double planformWingAreaSquareFeet = 0.0;
        double stepFrontFeet = 0.0;
        double stepRearFeet = 0.0;
        std::vector<double> deltaXNacellePerkinsFront;
        std::vector<double> x1CentroidDistancesNacelleFront;
        std::vector<double> diametersNacelleFront; // Per tenere traccia dei diametri corrispondenti
        std::vector<double> deltaXNacellePerkinsRear;
        std::vector<double> x1CentroidDistancesNacelleRear;
        std::vector<double> diametersNacelleRear;                // Per tenere traccia dei diametri corrispondenti
        double gradientToLocalFlowAngleDueToUpWashNacelle = 0.0; // dBeta/dAlpha used in Perkins that is an upwash gradient for all the body ahead of the wing, and becomes a downwash gradient for all bodies behind the wing.
        int indexFoundLastSectionNacelleFront = -1;
        int indexFoundLastSectionNacelleRear = -1;
        std::vector<double> frontNacelleDiamterFeet;
        std::vector<double> frontNacelleDeltaXFeet;
        std::vector<double> rearNacelleDiamterFeet;
        std::vector<double> rearNacelleDeltaXFeet;
        double summationNacelleContribution = 0.0;
        double nacelleMomentFactor = 0.0;
        double maxNacelleWidthFeet = 0.0;
        double nacelleLengthFeet = 0.0;
        double dynamicPressurRatioHorizontal = 0.0;
        double y1StationOnTheOrizonrtalTail = 0.0;
        double y2StationOnTheOrizonrtalTail = 0.0;
        double chordValueAtX1StationOnTheOrizonrtalTail = 0.0;
        double chordValueAtX2StationOnTheOrizonrtalTail = 0.0;
        std::vector<double> chordsHorizontalTail;
        std::vector<double> etaStationHorizontalTail;
        double horizontalTailAreaInTheSlip = 0.0;
        double steadyStateSpeed = 0.0;
        double diamterDiskFeet = 0.0;
        double liftSlope2DCorrectedCompressibilityWing = 0.0;
        double liftSlope2DCorrectedCompressibilityCanard = 0.0;
        double liftSlope2DCorrectedCompressibilityHorizontal = 0.0;
        double beta = 0.0;
        double kFactorWing = 0.0;
        double kFactorCanard = 0.0;
        double kFactorHorizontal = 0.0;
        double xCGAsFractionOfMAC = 0.0;
        double neutralPointAsFractionOfMAC = 0.0;

        //  Define endplate effect data for H-tail and U-tail configurations
        // These vectors represent the relationship between vertical tail height ratio
        // and the efficiency factor for the horizontal tail
        std::vector<double> endplateHeighToHorizontalSpanRatio = {
            4.06276e-05, 0.00813568, 0.0169297, 0.0281854, 0.0436553,
            0.0573680, 0.0752977, 0.0907630, 0.109042, 0.122046,
            0.139617, 0.158946, 0.179324, 0.198296, 0.220077,
            0.240100, 0.259770, 0.288572, 0.319826, 0.348973,
            0.379170, 0.400238};

        std::vector<double> factorRToHorizontalTailContribution = {
            0.997912, 0.979178, 0.962534, 0.941731, 0.918168,
            0.896681, 0.870349, 0.850263, 0.825324, 0.808702,
            0.788627, 0.765084, 0.745023, 0.726346, 0.706293,
            0.689012, 0.673120, 0.650322, 0.631014, 0.613085,
            0.596553, 0.584841};

        // AR = 4.0
        std::vector<double>
            x_AR4 = {1.987690, 1.882740, 1.774640, 1.618870, 1.488560, 1.392180,
                     1.254540, 1.178300, 1.091490, 1.023750, 0.960280, 0.882022,
                     0.805917, 0.738358, 0.668714, 0.618095, 0.563305, 0.516943,
                     0.464390, 0.422469, 0.387092, 0.355955, 0.331126, 0.302226,
                     0.283739, 0.271578, 0.261451};
        std::vector<double> y_AR4 = {0.050866, 0.048525, 0.049383, 0.053791, 0.061171, 0.069339,
                                     0.087354, 0.096420, 0.109800, 0.120915, 0.136225, 0.157999,
                                     0.183987, 0.218368, 0.256997, 0.289128, 0.329754, 0.363968,
                                     0.419383, 0.478945, 0.561724, 0.644469, 0.720818, 0.818352,
                                     0.892536, 0.962438, 1.021750};

        // AR = 6.0
        std::vector<double> x_AR6 = {1.990130, 1.913810, 1.848090, 1.792470, 1.708250, 1.624030,
                                     1.538210, 1.474690, 1.400020, 1.326960, 1.255480, 1.191990,
                                     1.112620, 1.036470, 0.949220, 0.874730, 0.816087, 0.762289,
                                     0.721171, 0.664194, 0.610485, 0.577303, 0.539452, 0.492178,
                                     0.444966, 0.410536, 0.377657, 0.343454, 0.318751, 0.291008,
                                     0.271138, 0.254182};
        std::vector<double> y_AR6 = {0.092095, 0.090585, 0.088992, 0.092077, 0.097504, 0.102932,
                                     0.106786, 0.115222, 0.122160, 0.130673, 0.139172, 0.152368,
                                     0.167276, 0.186918, 0.211407, 0.240555, 0.263232, 0.295388,
                                     0.322685, 0.354867, 0.398129, 0.423776, 0.462152, 0.514882,
                                     0.575543, 0.644036, 0.707757, 0.804805, 0.897018, 1.006710,
                                     1.106810, 1.173580};

        // AR = 9.0
        std::vector<double> x_AR9 = {1.978750, 1.913580, 1.842040, 1.762560, 1.662440, 1.551230,
                                     1.447980, 1.351100, 1.247900, 1.166990, 1.094010, 1.011530,
                                     0.929097, 0.851491, 0.778604, 0.701086, 0.639517, 0.577934,
                                     0.525979, 0.482010, 0.434975, 0.403750, 0.370997, 0.347771,
                                     0.318248, 0.295022, 0.275038, 0.256643};
        std::vector<double> y_AR9 = {0.127089, 0.127606, 0.128173, 0.130390, 0.134357, 0.144758,
                                     0.155097, 0.168557, 0.185241, 0.206507, 0.226124, 0.250576,
                                     0.279787, 0.316892, 0.347613, 0.395823, 0.450253, 0.503096,
                                     0.566968, 0.635536, 0.718408, 0.790048, 0.869633, 0.947555,
                                     1.033460, 1.111380, 1.197210, 1.283030};

        // AR = 12.0
        std::vector<double> x_AR12 = {1.985290, 1.910570, 1.808870, 1.689720, 1.592820, 1.473690,
                                      1.384750, 1.295880, 1.199070, 1.116580, 1.051570, 0.988131,
                                      0.919988, 0.851845, 0.780598, 0.723658, 0.668384, 0.613199,
                                      0.554859, 0.502967, 0.462216, 0.426157, 0.396660, 0.367087,
                                      0.336013, 0.314452, 0.285081, 0.272894, 0.254563};
        std::vector<double> y_AR12 = {0.149248, 0.149840, 0.155407, 0.165871, 0.176159, 0.189796,
                                      0.201607, 0.221350, 0.242742, 0.265607, 0.286748, 0.306289,
                                      0.333800, 0.361311, 0.398366, 0.435307, 0.481754, 0.539306,
                                      0.600056, 0.671860, 0.745162, 0.808908, 0.897986, 0.977546,
                                      1.068220, 1.155650, 1.260590, 1.327320, 1.421070};

        // A_curve - Fuselage contribution to longitudinal stability (segments 1-5)
        std::vector<double> x_A_curve = {
            0.605743, 0.671295, 0.748693, 0.825047, 0.917079,
            1.019570, 1.125180, 1.233930, 1.329070, 1.425260,
            1.513080, 1.599860, 1.684540, 1.758770, 1.839270,
            1.919760, 1.993980};
        std::vector<double> y_A_curve = {
            1.319030, 1.290740, 1.262570, 1.234400, 1.206390,
            1.175850, 1.153250, 1.133320, 1.118510, 1.106350,
            1.096740, 1.081850, 1.074840, 1.070350, 1.063290,
            1.058870, 1.057020};

        // B_curve - Fuselage contribution to longitudinal stability (segments 7-14)
        std::vector<double> x_B_curve = {
            0.151462, 0.157874, 0.168501, 0.179112, 0.190771,
            0.202427, 0.218281, 0.232049, 0.252097, 0.271095,
            0.294279, 0.316374, 0.333221, 0.352154, 0.375281,
            0.400495, 0.421518, 0.449872, 0.478221, 0.505508,
            0.535926, 0.571579, 0.606174, 0.648094, 0.682662,
            0.724561, 0.764343, 0.804133, 0.848092, 0.893087,
            0.931801, 0.951670};
        std::vector<double> y_B_curve = {
            4.004220, 3.919980, 3.814710, 3.719970, 3.622610,
            3.527890, 3.422670, 3.314790, 3.201720, 3.091260,
            2.978210, 2.891500, 2.817910, 2.746970, 2.668180,
            2.592040, 2.521130, 2.442380, 2.366280, 2.300700,
            2.237790, 2.169660, 2.109430, 2.044010, 1.999580,
            1.947330, 1.910860, 1.869130, 1.835340, 1.806830,
            1.783530, 1.778470};

        // Kf factor - fuselage longitudinal
        // X = Position of 1/4 root chord on body or on nacelle  [% body length -  % nacelle length]
        // Y = Kf

        std::vector<double> Kf_x = {
            9.94695, 11.9966, 14.1773, 16.358, 18.6262,
            20.1093, 21.3744, 23.25, 24.9951, 26.4349,
            27.7438, 28.9363, 29.9544, 31.4091, 32.4273,
            33.6491, 34.6965, 36.1803, 37.5479, 38.8572,
            39.9923, 41.5055, 42.4369, 43.6592, 44.6781,
            45.7549, 46.6283, 47.5889, 48.6371, 49.4525,
            50.1803, 51.0538, 52.044, 53.005, 53.7916,
            54.578, 55.4227, 56.093, 56.8213, 57.521,
            58.1622, 58.8616, 59.5025, 60.0272};

        std::vector<double> Kf_y = {
            0.00430785, 0.00454115, 0.00493495, 0.00532875, 0.00596375,
            0.00644088, 0.00683837, 0.00739444, 0.00819258, 0.00885775,
            0.00949661, 0.0100823, 0.0106149, 0.011629, 0.012269,
            0.0130156, 0.0137628, 0.0148304, 0.0159521, 0.0169667,
            0.0180894, 0.0194253, 0.0203877, 0.0216174, 0.0228479,
            0.0238634, 0.0249334, 0.0260568, 0.0274482, 0.0286795,
            0.0296428, 0.0307665, 0.0323192, 0.0338183, 0.0352644,
            0.0364958, 0.0379417, 0.0393346, 0.0407273, 0.0424422,
            0.0437816, 0.0452817, 0.04646, 0.0476925};

        // Vee Tail solope of normal force - NACA report TN-775

        std::vector<double> aspectRatioOfVeeTail = {
            0.285597, 0.406789, 0.552166, 0.687835, 0.817453,
            1.04627, 1.22066, 1.43492, 1.62732, 1.83428,
            2.07749, 2.34244, 2.59284, 2.93755, 3.20605,
            3.47816, 3.76110, 3.93884, 4.11292, 4.27614,
            4.44658, 4.68954, 4.85634, 5.07031, 5.29874,
            5.56709, 5.78101, 5.90791, 5.99851};

        std::vector<double> slopeOfNormalForce = { // Values in 1/deg
            0.0232337, 0.0255985, 0.0281375, 0.0304137, 0.0326242,
            0.0357734, 0.0384645, 0.0412192, 0.0433162, 0.0458078,
            0.0483631, 0.0509830, 0.0532085, 0.0560873, 0.0582458,
            0.0603383, 0.0622324, 0.0633420, 0.0643201, 0.0652989,
            0.0661454, 0.0673171, 0.0680980, 0.0692055, 0.0701804,
            0.0714824, 0.0723264, 0.0728461, 0.0729726};

        // K_b chart - taper_ratio = 0.0
        std::vector<double> x_Kb_taper0 = {
            0.00E+00, 6.08E-02, 1.22E-01, 1.84E-01, 2.43E-01, 3.24E-01,
            4.07E-01, 5.08E-01, 5.53E-01, 6.34E-01, 6.99E-01, 7.60E-01,
            8.18E-01, 8.83E-01, 9.21E-01};

        std::vector<double> y_Kb_taper0 = {
            2.18E-03, 9.83E-02, 1.92E-01, 2.77E-01, 3.65E-01, 4.72E-01,
            5.70E-01, 6.88E-01, 7.34E-01, 8.08E-01, 8.62E-01, 9.08E-01,
            9.41E-01, 9.74E-01, 9.89E-01};

        // K_b chart - taper_ratio = 0.5
        std::vector<double> x_Kb_taper05 = {
            0.00E+00, 5.85E-02, 1.21E-01, 1.62E-01, 2.65E-01, 3.46E-01,
            4.29E-01, 5.13E-01, 5.76E-01, 6.54E-01, 7.15E-01, 7.78E-01,
            9.19E-01, 9.39E-01, 9.62E-01, 9.84E-01};

        std::vector<double> y_Kb_taper05 = {
            -2.18E-03, 8.52E-02, 1.68E-01, 2.21E-01, 3.52E-01, 4.52E-01,
            5.50E-01, 6.44E-01, 7.07E-01, 7.88E-01, 8.41E-01, 8.89E-01,
            9.72E-01, 9.80E-01, 9.91E-01, 9.98E-01};

        // K_b chart - taper_ratio = 1.0
        std::vector<double> x_Kb_taper1 = {
            0.00E+00, 4.05E-02, 1.03E-01, 1.84E-01, 2.65E-01, 3.48E-01,
            4.50E-01, 5.31E-01, 6.11E-01, 7.17E-01, 7.98E-01, 8.41E-01,
            9.01E-01, 9.42E-01, 9.64E-01, 9.84E-01};

        std::vector<double> y_Kb_taper1 = {
            0.00E+00, 5.24E-02, 1.29E-01, 2.31E-01, 3.30E-01, 4.26E-01,
            5.46E-01, 6.35E-01, 7.18E-01, 8.19E-01, 8.84E-01, 9.19E-01,
            9.54E-01, 9.78E-01, 9.87E-01, 9.93E-01};

        // Ratio flap effectiveness alphaCL/alphaCl - cf/c = 0.1
        std::vector<double> x_RatioFlapEff_cf01 = {
            1.35949, 1.46280, 1.52965, 1.64724, 1.81544, 1.98381, 2.16047,
            2.39659, 2.58204, 2.77607, 3.05486, 3.34220, 3.62134, 4.01930,
            4.34114, 4.73078, 5.17996, 5.55287, 5.95130, 6.45159, 7.01142,
            7.54586, 8.04649, 8.64891, 9.15807, 9.70132, 9.99854};

        std::vector<double> y_RatioFlapEff_cf01 = {
            1.99953, 1.95765, 1.92582, 1.88727, 1.83867, 1.79509, 1.74564,
            1.69618, 1.65510, 1.61653, 1.57291, 1.53096, 1.49739, 1.46126,
            1.43437, 1.40327, 1.37466, 1.35109, 1.32836, 1.30392, 1.28197,
            1.26254, 1.24814, 1.22952, 1.21596, 1.20573, 1.20396};

        // Ratio flap effectiveness alphaCL/alphaCl - cf/c = 0.2
        std::vector<double> x_RatioFlapEff_cf02 = {
            0.546869, 0.587865, 0.637211, 0.720417, 0.837250, 0.970985,
            1.11385, 1.28219, 1.45042, 1.67805, 1.91446, 2.19328,
            2.48940, 2.73476, 3.05640, 3.36133, 3.61548, 3.91201,
            4.25947, 4.64088, 5.05646, 5.57385, 6.10820, 6.74468,
            7.25390, 8.04338, 8.70557, 9.35091, 10.0557};

        std::vector<double> y_RatioFlapEff_cf02 = {
            1.99813, 1.95543, 1.90854, 1.85829, 1.79798, 1.73516,
            1.69075, 1.64633, 1.59856, 1.54910, 1.50800, 1.46522,
            1.43164, 1.40393, 1.37118, 1.34681, 1.32747, 1.30561,
            1.28289, 1.25932, 1.24161, 1.22051, 1.19857, 1.18245,
            1.17056, 1.15774, 1.14831, 1.14307, 1.13865};

        // Ratio flap effectiveness alphaCL/alphaCl - cf/c = 0.3
        std::vector<double> x_RatioFlapEff_cf03 = {
            0.215702, 0.231391, 0.263519, 0.295733, 0.353169, 0.436114,
            0.527929, 0.619772, 0.711933, 0.846244, 0.964030, 1.08185,
            1.25909, 1.41946, 1.65592, 1.82493, 2.03646, 2.28186,
            2.51047, 2.78998, 3.06955, 3.37460, 3.68829, 4.04436,
            4.39208, 4.82479, 5.47820, 6.16564, 6.79380, 7.33716,
            8.05875, 8.83136, 9.46827, 10.0118};

        std::vector<double> y_RatioFlapEff_cf03 = {
            2.00075, 1.96308, 1.90950, 1.85843, 1.79982, 1.74203,
            1.69513, 1.64906, 1.61220, 1.56612, 1.53343, 1.50159,
            1.46888, 1.43953, 1.40011, 1.37494, 1.35143, 1.32457,
            1.30356, 1.28087, 1.25985, 1.23882, 1.22198, 1.20261,
            1.18742, 1.17388, 1.15609, 1.13995, 1.12886, 1.12198,
            1.11086, 1.10139, 1.09783, 1.09598};

        // Ratio flap effectiveness alphaCL/alphaCl - cf/c = 0.4
        std::vector<double> x_RatioFlapEff_cf04 = {
            0.0188759, 0.0336701, 0.0494166, 0.0989646, 0.115000, 0.147301,
            0.204563, 0.246050, 0.329429, 0.413038, 0.505026, 0.647946,
            0.774111, 0.951295, 1.09465, 1.23806, 1.39860, 1.61005,
            1.84678, 2.28712, 2.72769, 3.30431, 3.94050, 4.44111,
            5.12008, 5.76528, 6.59720, 7.48872, 8.00669, 9.03422,
            10.0278};

        std::vector<double> y_RatioFlapEff_cf04 = {
            1.95813, 1.89451, 1.85851, 1.81748, 1.78985, 1.74129,
            1.67766, 1.64918, 1.60396, 1.56542, 1.52354, 1.48080,
            1.44477, 1.41039, 1.38021, 1.35170, 1.32737, 1.30135,
            1.26947, 1.23082, 1.19886, 1.17105, 1.14656, 1.13132,
            1.11603, 1.10661, 1.09294, 1.08343, 1.07907, 1.07287,
            1.06584};

        // Ratio flap effectiveness alphaCL/alphaCl - cf/c = 0.5
        std::vector<double> x_RatioFlapEff_cf05 = {
            0.0344589, 0.0504073, 0.0835163, 0.133122, 0.191135, 0.266049,
            0.366559, 0.500870, 0.635442, 0.795494, 0.998188, 1.24321,
            1.55643, 1.96297, 2.29351, 2.64108, 3.03124, 3.58273,
            4.09191, 5.02561, 6.05289, 7.02934, 8.02290, 8.99941,
            10.0186};

        std::vector<double> y_RatioFlapEff_cf05 = {
            1.67102, 1.64088, 1.61576, 1.57640, 1.53453, 1.49014,
            1.44909, 1.40301, 1.36446, 1.32590, 1.29235, 1.25460,
            1.22436, 1.19075, 1.16971, 1.15034, 1.13431, 1.11654,
            1.10382, 1.08593, 1.07219, 1.06266, 1.05646, 1.04860,
            1.04659};

        // Ratio flap effectiveness alphaCL/alphaCl - cf/c = 0.6
        std::vector<double> x_RatioFlapEff_cf06 = {
            0.0117000, 0.0614212, 0.127899, 0.202726, 0.295089, 0.429631,
            0.564289, 0.724774, 0.936106, 1.17312, 1.49496, 1.79154,
            2.19011, 2.63115, 3.01293, 3.40326, 4.02295, 4.52385,
            5.01625, 5.69546, 6.32379, 7.07958, 8.03070, 9.02446,
            10.0011};

        std::vector<double> y_RatioFlapEff_cf06 = {
            1.50362, 1.46761, 1.42490, 1.37800, 1.34700, 1.30761,
            1.27157, 1.24557, 1.21620, 1.19269, 1.16579, 1.14560,
            1.12706, 1.10849, 1.09581, 1.08480, 1.07455, 1.06768,
            1.06082, 1.05222, 1.04615, 1.04171, 1.03637, 1.03604,
            1.03153};

        // Ratio flap effectiveness alphaCL/alphaCl - cf/c = 0.7
        std::vector<double> x_RatioFlapEff_cf07 = {
            0.0152880, 0.0564866, 0.148561, 0.240952, 0.426341, 0.578448,
            0.781460, 1.02717, 1.38310, 1.75615, 2.23119, 2.66397,
            3.20722, 3.86085, 5.02405, 6.05999, 7.00265, 8.02180,
            8.98999, 10.0093};

        std::vector<double> y_RatioFlapEff_cf07 = {
            1.36132, 1.32448, 1.28510, 1.25494, 1.21219, 1.18954,
            1.16519, 1.14753, 1.12398, 1.10460, 1.08686, 1.07500,
            1.06477, 1.05367, 1.04073, 1.03201, 1.02751, 1.02466,
            1.02182, 1.02316};

        // Ratio flap effectiveness alphaCL/alphaCl - cf/c = 0.8
        std::vector<double> x_RatioFlapEff_cf08 = {
            0.0180679, 0.161537, 0.364548, 0.609999, 0.915252, 1.29677,
            1.78037, 2.34046, 2.94313, 3.41861, 4.06387, 5.04046,
            5.96610, 7.03619, 7.99603, 8.99829, 10.0090};

        std::vector<double> y_RatioFlapEff_cf08 = {
            1.19558, 1.16875, 1.14441, 1.11921, 1.10405, 1.08383,
            1.06776, 1.05335, 1.04226, 1.03708, 1.02933, 1.02398,
            1.01865, 1.01494, 1.01630, 1.01596, 1.01395};

        // Ratio flap effectiveness alphaCL/alphaCl - cf/c = 0.9
        std::vector<double> x_RatioFlapEff_cf09 = {
            0.0152399, 0.133488, 0.277303, 0.540058, 0.904940,
            1.25298, 1.66888, 2.04243, 2.54324};

        std::vector<double> y_RatioFlapEff_cf09 = {
            1.11355, 1.09426, 1.07747, 1.06148, 1.05131,
            1.04534, 1.03683, 1.03168, 1.02231};

        // Slopes related variables
        double finiteWingLiftSlope = 0.0;
        double finiteHorizontalTailLiftSlope = 0.0;
        double finiteCanardLiftSlope = 0.0;
        double veeTailFiniteSlope = 0.0;

    public:
        LongitudinalStabilityCalculator(
            BuildAircraft &builder,
            COG::COGDATA cogData,
            VSP::Aircraft aircraftInfo,
            VSP::AeroSettings settings,
            VSP::Wing &wing,  // Needed if I wanto to modify or add some variables to the wing class
            VSP::Wing &horizontalTail,
            VSP::Fuselage fuselage,
            VSP::Nacelle nacelle,
            VSP::Disk disk,
            VSP::Wing *canard = nullptr,
            VSP::Wing *verticalTail = nullptr)
            : builder(builder),
              nameOfAircraft(nameOfAircraft),
              cogData(cogData),
              aircraftInfo(aircraftInfo),
              settings(settings),
              wing(wing),
              horizontalTail(horizontalTail),
              fuselage(fuselage),
              nacelle(nacelle),
              disk(disk),
              canard(canard),
              verticalTail(verticalTail),
              integrationStep(1.0 / 99.0)
        {
        }

        ~LongitudinalStabilityCalculator() = default;

        void calculateLongitudinalStability()
        {


            // ========================================================================
            // STEP 0: Calculate lift slope corrected by compressibility effect
            // ========================================================================

            beta = std::sqrt(1 - std::pow(settings.Mach,2));


            liftSlope2DCorrectedCompressibilityWing = builder.getCommonData().getMeanAirfoilSlopeWing()/beta;

            if (builder.getCommonData().getHasCanard())
            {
                liftSlope2DCorrectedCompressibilityCanard = builder.getCommonData().getMeanAirfoilSlopeCanard()/beta;
            }

            liftSlope2DCorrectedCompressibilityHorizontal = builder.getCommonData().getMeanAirfoilSlopeHorizontalTail()/beta;

            // ========================================================================
            // STEP 1: Calculate Wing Contribution to Longitudinal Stability Derivative
            // ========================================================================

            // Calculate the dimensionless center of gravity position relative to wing leading edge
            // xCGWingBar = (xCG - xLE_wing - deltaXtoLEWing) / MAC_wing
            xCGWingBar = (cogData.xCG - wing.xloc - wing.deltaXtoLEMAC) / wing.MAC;

            // Get the dimensionless aerodynamic center position from aircraft common data
            xACWingBar = builder.getCommonData().getAdimAerodynamicCenter();

            // Calculate wing contribution: difference between CG and aerodynamic center positions
            // This represents the moment arm contribution from the wing
            deltaCmClWingContribution = xCGWingBar - xACWingBar;

            // ========================================================================
            // STEP 2: Calculate Horizontal Tail Contribution
            // ========================================================================

            // Check if aircraft has a horizontal tail configuration (H-tail or U-tail)
            if (builder.getCommonData().getTypeOfTail() == TypeOfTail::H_TAIL ||
                builder.getCommonData().getTypeOfTail() == TypeOfTail::U_TAIL)
            {
              
                // STEP 2.1: Interpolate to find endplate efficiency factor
                // Create polynomial interpolation of order 2 to find factor R
                Interpolant interp1(endplateHeighToHorizontalSpanRatio, factorRToHorizontalTailContribution, 2, RegressionMethod::POLYNOMIAL);

                // Calculate factor R based on vertical-to-horizontal tail span ratio
                factorRToHorizontalTail = interp1.getYValueFromRegression(verticalTail->totalSpan / horizontalTail.totalProjectedSpan);

                // STEP 2.2: Calculate finite horizontal tail lift slope with endplate effect
                // Formula: CLα_h = CLα_h0 / (1 + (57.3 * R * CLα_h0) / (π * AR_h))
                // finiteHorizontalTailLiftSlope = builder.getCommonData().getMeanAirfoilSlopeHorizontalTail() /
                //                                 (1.0 + (57.3 * factorRToHorizontalTail * builder.getCommonData().getMeanAirfoilSlopeHorizontalTail()) /
                //                                            (M_PI * horizontalTail.aspectRatio));

                kFactorHorizontal = liftSlope2DCorrectedCompressibilityWing/(2*M_PI);

                finiteHorizontalTailLiftSlope = 2*M_PI*horizontalTail.aspectRatio /(2 + std::sqrt(std::pow(horizontalTail.aspectRatio,2) * std::pow(beta,2) / 
                                                      (std::pow(kFactorHorizontal,2))*(1+std::pow(std::tan(horizontalTail.sweepC2/57.3),2)/std::pow(beta,2))+4));

                horizontalTail.finiteSlope = finiteHorizontalTailLiftSlope;
                                                                 
            }

            else
            {
                // STEP 2.3: For other tail types (T-tail, conventional, etc.)
                // No endplate effect, factor R = 1.0
                factorRToHorizontalTail = 1.0;

                // Calculate finite horizontal tail lift slope without endplate effect
                // finiteHorizontalTailLiftSlope = builder.getCommonData().getMeanAirfoilSlopeHorizontalTail() /
                //                                 (1.0 + (57.3 * factorRToHorizontalTail * builder.getCommonData().getMeanAirfoilSlopeHorizontalTail()) /
                //                                            (M_PI * horizontalTail.aspectRatio));

                kFactorHorizontal = liftSlope2DCorrectedCompressibilityWing / (2 * M_PI);

                finiteHorizontalTailLiftSlope = 2 * M_PI * horizontalTail.aspectRatio / 
                                                (2 + std::sqrt(std::pow(horizontalTail.aspectRatio, 2) * std::pow(beta, 2) / 
                                                (std::pow(kFactorHorizontal, 2)) * (1 + std::pow(std::tan(horizontalTail.sweepC2/57.3), 2) / std::pow(beta, 2)) + 4));

                 horizontalTail.finiteSlope = finiteHorizontalTailLiftSlope;
            }

            // Calculate finite wing lift slope considering finite aspect ratio effects
            // Formula: CLα_w = CLα_w0 / (1 + (57.3 * CLα_w0) / (π * AR_w))
            // finiteWingLiftSlope = builder.getCommonData().getMeanAirfoilSlopeWing() /
            //                       (1.0 + (57.3 * builder.getCommonData().getMeanAirfoilSlopeWing()) /
            //                                  (M_PI * wing.aspectRatio));

            kFactorWing = liftSlope2DCorrectedCompressibilityWing / (2 * M_PI);

            finiteWingLiftSlope = 2 * M_PI * wing.aspectRatio / 
                                   (2 + std::sqrt(std::pow(wing.aspectRatio, 2) * std::pow(beta, 2) / 
                                   (std::pow(kFactorWing, 2)) * (1 + std::pow(std::tan(wing.sweepC2/57.3), 2) / std::pow(beta, 2)) + 4));

            wing.finiteSlope = finiteWingLiftSlope;
            // STEP 2.5: Calculate downwash gradient (dε/dα)
            // This represents how much the downwash angle changes with angle of attack
            // Formula: dε/dα = (2 * CLα_h) / (π * AR_h) * (180/π) = 114.6 * CLα_w / (π * AR_w)
            downWashGradient = 114.6 * finiteWingLiftSlope / (M_PI * wing.aspectRatio);

            // ========================================================================
            // STEP 3: Calculate Wing Finite Lift Slope
            // ========================================================================

            // ========================================================================
            // STEP 4: Setup ODE Initial Condition for Downwash Angle Integration
            // ========================================================================

            // Calculate initial downwash angle at the first angle of attack
            // ε₀ = (2 * CLα_w * α₀) / (π * AR_w) * (180/π) = 114.6 * CLα_w * α₀ / (π * AR_w)
            y0 = 114.6 * finiteWingLiftSlope * settings.AoA.front() / (M_PI * wing.aspectRatio);

            // ========================================================================
            // STEP 5: Calculate Horizontal Tail Volumetric Ratio
            // ========================================================================

            // Check tail configuration to apply appropriate efficiency factor
            if (builder.getCommonData().getTypeOfTail() == TypeOfTail::H_TAIL ||
                builder.getCommonData().getTypeOfTail() == TypeOfTail::U_TAIL)
            {
                // For H-tail and U-tail: apply efficiency factor of 0.95 suggested by Raymer
                // V_h = 0.95 * (S_h * L_h) / (S_w * MAC_w)
                volumetricRatio = 0.95 * (horizontalTail.planformArea * fuselage.tailArm) / (wing.planformArea * wing.MAC);
            }
            else if (builder.getCommonData().getTypeOfTail() == TypeOfTail::V_TAIL ||
                     builder.getCommonData().getTypeOfTail() == TypeOfTail::V_REV_TAIL)
            {
                projectedVeeTailSurfaceToEquivalentHorizontal = horizontalTail.planformArea * std::pow(std::cos(horizontalTail.averageDihedral / 57.3), 2.0);
                volumetricRatio = (projectedVeeTailSurfaceToEquivalentHorizontal * fuselage.tailArm) / (wing.planformArea * wing.MAC);
            }

            else
            {

                // For T-tail configuration: assume full dynamic pressure at tail (η ≈ 1.0)
                // V_h = (S_h * L_h) / (S_w * MAC_w)
                volumetricRatio = (horizontalTail.planformArea * fuselage.tailArm) / (wing.planformArea * wing.MAC);
            }

            // ========================================================================
            // STEP 6: Solve ODE for Downwash Angle vs Angle of Attack
            // ========================================================================

            // Store wing aspect ratio for use in ODE solver lambda function
            wingAspectRatioToLambdaFunction = wing.aspectRatio;

            // Create ODE45 solver to integrate downwash angle over angle of attack range
            // dε/dα = 114.6 * CLα_w / (π * AR_w)
            // Integration from AoA_min to AoA_max with initial condition y0
            ODE45 odeSolver45([=](double x, double y)
                              { return 114.6 * finiteWingLiftSlope / (M_PI * wingAspectRatioToLambdaFunction); },
                              settings.AoA.front(),
                              settings.AoA.back(),
                              y0,
                              integrationStep);

            // Get solution arrays (angle of attack and downwash angle)
            odeSolver45.getT();
            odeSolver45.getY();

            // STEP 6.1: Store downwash angles for all angle of attack values
            for (const auto &val : odeSolver45.getY())
            {
                downWashAngle.emplace_back(val);
            }

            // ========================================================================
            // STEP 7: Calculate Horizontal Tail Stability Derivative Based on Tail Type
            // ========================================================================

            ConvVel speedConverter(Speed::M_TO_S, Speed::FT_TO_S, settings.Vinf);

            steadyStateSpeed = speedConverter.getConvertedValues();

            ConvLength diamterConverterDisk(Length::M, Length::FT, disk.diameter.front());

            diamterDiskFeet = diamterConverterDisk.getConvertedValues();

            dynamicPressureFreeStream = 0.5 * settings.rho * std::pow(settings.Vinf, 2.0);

            ConvPressure pressureConverter(Pressure::PASCALS, Pressure::PSF, dynamicPressureFreeStream);
            psfDynamicPressure = pressureConverter.getConvertedValues();

            // // Calculate the etaH for the only propeller driven aircraft
            // if (builder.getCommonData().getAircraftCategory() == AircraftCategory::GENERAL_AVIATION ||
            //     builder.getCommonData().getAircraftCategory() == AircraftCategory::UAV)
            // {
            //     if (disk.yloc.front() < 0.5 * horizontalTail.totalProjectedSpan)
            //     {

            //         y1StationOnTheOrizonrtalTail = disk.yloc.front() - 0.5 * disk.diameter.front();
            //         y2StationOnTheOrizonrtalTail = disk.yloc.front() + 0.5 * disk.diameter.front();

            //         if (y2StationOnTheOrizonrtalTail > 0.5 * horizontalTail.totalProjectedSpan)
            //         {
            //             y2StationOnTheOrizonrtalTail = 0.5 * horizontalTail.totalProjectedSpan;
            //         }

            //         for (size_t j = 0; j < horizontalTail.croot.size(); j++)
            //         {

            //             chordsHorizontalTail.push_back(horizontalTail.croot[j]);

            //             if (j == 0)
            //             {

            //                 etaStationHorizontalTail.push_back(0.0);
            //             }

            //             else
            //             {

            //                 etaStationHorizontalTail.push_back(2 * horizontalTail.span[j] / horizontalTail.totalProjectedSpan);
            //             }

            //             if (j == horizontalTail.croot.size() - 1)
            //             {

            //                 chordsHorizontalTail.push_back(horizontalTail.ctip.back());
            //                 etaStationHorizontalTail.push_back(1.0);
            //             }
            //         }

            //         // Find chord values at x1 and x2 stations on the horizontal tail

            //         Interpolant interpChordHorizontalTail(etaStationHorizontalTail, chordsHorizontalTail, 1, RegressionMethod::LINEAR);

            //         chordValueAtX1StationOnTheOrizonrtalTail = interpChordHorizontalTail.getYValueFromRegression(2 * y1StationOnTheOrizonrtalTail / horizontalTail.totalProjectedSpan);
            //         chordValueAtX2StationOnTheOrizonrtalTail = interpChordHorizontalTail.getYValueFromRegression(2 * y2StationOnTheOrizonrtalTail / horizontalTail.totalProjectedSpan);

            //         horizontalTailAreaInTheSlip = (y2StationOnTheOrizonrtalTail - y1StationOnTheOrizonrtalTail) * (chordValueAtX1StationOnTheOrizonrtalTail + chordValueAtX2StationOnTheOrizonrtalTail);

            //         dynamicPressurRatioHorizontal = 1 + (horizontalTailAreaInTheSlip / horizontalTail.planformArea) *
            //                                             (2200*builder.getEngineData().getBSHP() /(psfDynamicPressure * steadyStateSpeed *
            //                                                 M_PI * std::pow(diamterDiskFeet, 2.0)));

            //         etaMinimumConventionalTail = dynamicPressurRatioHorizontal;
            //         etaMMaximumConventionalTail = dynamicPressurRatioHorizontal;
            //     }
            // }

            switch (builder.getCommonData().getTypeOfTail())
            {
            case TypeOfTail::CONVENTIONAL_TAIL:
            case TypeOfTail::H_TAIL:
            case TypeOfTail::U_TAIL:
                // For conventional and similar tail configurations:
                // Apply dynamic pressure ratio efficiency factor (average of min and max)
                // dCm/dCL_h = -(CLα_h / CLα_w) * V_h * η * (1 - dε/dα)
                // where η is the average efficiency factor for dynamic pressure at the tail
                deltaCmClHorizontalTailContribution = -(finiteHorizontalTailLiftSlope / finiteWingLiftSlope) * volumetricRatio *
                                                      0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) * (1 - downWashGradient);
                break;
            case TypeOfTail::V_TAIL:
            case TypeOfTail::V_REV_TAIL:
            {
                // From NACA report TN-775
                Interpolant slopeOfNormalForceInterpolator(aspectRatioOfVeeTail, slopeOfNormalForce, 1, RegressionMethod::POWER);
                veeTailFiniteSlope = slopeOfNormalForceInterpolator.getYValueFromRegression(horizontalTail.aspectRatio);

                deltaCmClHorizontalTailContribution = -(veeTailFiniteSlope / finiteWingLiftSlope) * volumetricRatio *
                                                      0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) * (1 - downWashGradient);
            }
            break;

            case TypeOfTail::T_TAIL:
                // For T-tail configuration:
                // Assume full dynamic pressure at tail (η ≈ 1.0) due to position above wake
                // dCm/dCL_h = -(CLα_h / CLα_w) * V_h * (1 - dε/dα)
                deltaCmClHorizontalTailContribution = -(finiteHorizontalTailLiftSlope / finiteWingLiftSlope) * volumetricRatio * (1 - downWashGradient);
                break;
            }

            // ========================================================================
            // STEP 8: Calculate Canard  Stability, if present
            // ========================================================================

            if (builder.getCommonData().getHasCanard())
            {

                // Calculate canard contribution to stability derivative
                // This is a simplified approach and may require more detailed modeling
                // dCm/dCL_c = (S_c * L_c) / (S_w * MAC_w) * (xCG - xAC_canard) / MAC_w

                tailArmCanard = canard->xloc + canard->deltaXtoLEMAC + 0.25 * canard->MAC - (wing.xloc + wing.deltaXtoLEMAC + 0.25 * wing.MAC);

                distanceFromQuarterRootChordWingToCanard = (wing.xloc + 0.25 * wing.croot.front()) - (canard->xloc + 0.25 * canard->croot.front());

                // finiteCanardLiftSlope = builder.getCommonData().getMeanAirfoilSlopeCanard() /
                //                         (1.0 + (57.3 * builder.getCommonData().getMeanAirfoilSlopeCanard()) /
                //                                    (M_PI * canard->aspectRatio));

                kFactorCanard = liftSlope2DCorrectedCompressibilityCanard / (2 * M_PI);

                finiteCanardLiftSlope = 2 * M_PI * canard->aspectRatio / 
                                        (2 + std::sqrt(std::pow(canard->aspectRatio, 2) * std::pow(beta, 2) / 
                                        (std::pow(kFactorCanard, 2)) * (1 + std::pow(std::tan(canard->sweepC2/57.3), 2) / std::pow(beta, 2)) + 4));

                canard->finiteSlope = finiteCanardLiftSlope;

                volumetricRatioCanard = (canard->planformArea * tailArmCanard) / (wing.planformArea * wing.MAC);

                // Initialize interpolator with power regression method
                Interpolant2D upwashInterpolator(1, RegressionMethod::POWER);

                // Add all curves
                upwashInterpolator.addCurve(4.0, x_AR4, y_AR4);
                upwashInterpolator.addCurve(6.0, x_AR6, y_AR6);
                upwashInterpolator.addCurve(9.0, x_AR9, y_AR9);
                upwashInterpolator.addCurve(12.0, x_AR12, y_AR12);

                downWashGradientOnWingDueToCanard = upwashInterpolator.interpolate(distanceFromQuarterRootChordWingToCanard, canard->aspectRatio);

                // Suggested by Roskam
                deltaCmClCanardContribution = -(finiteCanardLiftSlope / finiteWingLiftSlope) * volumetricRatioCanard * (1 + downWashGradientOnWingDueToCanard);
            }

            // ========================================================================
            // STEP 9: Calculate Fuselage  Stability
            // ========================================================================

            VSPGEOMTRYEXTRACTOR::GeometryExtractor geomExtractor;
            geomExtractor.extractAllGeoms(builder.getCommonData().getNameOfAircraft(),
                                          builder.getCommonData().getNameOfAircraft() + "_AllGeoms.vspscript");

            VSPGEOMTRYEXTRACTOR::AircraftGeometryData allGeomData = geomExtractor.getGeometryData();

            // VSPGEOMTRYEXTRACTOR::GeomInfo fuselageType;

            VSPGEOMTRYEXTRACTOR::DiametersExtractor fuseExtractor;

            VSPGEOMTRYEXTRACTOR::FuselageDiametersAndXStation fuseData = fuseExtractor.extractFuselageDiameters(
                builder.getCommonData().getNameOfAircraft(),
                allGeomData,
                fuselage.length);

            // Coordinate di riferimento dell'ala
            xTEWing = wing.xloc + wing.croot.front(); // Trailing edge

            ConvLength meterToFtConverterXTEWing(Length::M, Length::FT, wing.MAC);
            wingMACFeet = meterToFtConverterXTEWing.getConvertedValues();

            distanceFromeTEWingToQuarterChordMacHorizontal = (horizontalTail.xloc + horizontalTail.deltaXtoLEMAC + 0.25 * horizontalTail.MAC) - xTEWing;

            ConvLength meterToFtConverterDistanceTEWingToQuarterChordHorizontal(Length::M, Length::FT, distanceFromeTEWingToQuarterChordMacHorizontal);

            distanceFromeTEWingToQuarterChordMacHorizontal = meterToFtConverterDistanceTEWingToQuarterChordHorizontal.getConvertedValues();

            ConvLength meterToFtConverterRootChordWing(Length::M, Length::FT, wing.croot.front());
            rootChordFeet = meterToFtConverterRootChordWing.getConvertedValues();

            ConvLength meterToFtConverterFuselageDiam(Length::M, Length::FT, fuselage.diameter);
            fuselageDiameterFeet = meterToFtConverterFuselageDiam.getConvertedValues();

            ConvArea areaConverter(Area::SQUARE_METER, Area::SQUARE_FEET, wing.planformArea);
            planformWingAreaSquareFeet = areaConverter.getConvertedValues();

            for (const auto &geomNameOfComponent : allGeomData.nameOfComponentGeom)
            {
                for (const auto &geomName : allGeomData.nameGeom)
                {

                    if (geomName == "Custom" && geomNameOfComponent == fuselage.id)
                    {
                        // Trova l'ultima sezione del nose
                        for (size_t i = 0; i < fuseData.xStation.size(); i++)
                        {
                            if (fuseData.xStation[i] <= wing.xloc)
                                idxNoseEnd = i; // Continua ad aggiornare finché siamo prima dell'ala
                            else
                                break; // Appena superiamo wing.xloc, ci fermiamo
                        }

                        // Trova la prima sezione della tail
                        for (size_t i = 0; i < fuseData.xStation.size(); i++)
                        {
                            if (fuseData.xStation[i] >= xTEWing)
                            {
                                idxTailStart = i;
                                break;
                            }
                        }

                        // ========== PARTE ANTERIORE ==========

                        // 1. Aggiungi le sezioni del nose
                        for (int i = 0; i <= idxNoseEnd; i++)
                        {
                            if (i == 0)
                            {
                                xCoordinatePerkinsFront.push_back(fuseData.xStation[i]);
                                diametersFront.push_back(fuseData.allFuselageWidth[i]);

                                ConvLength meterToFtConverterDiamFront(Length::M, Length::FT, diametersFront[i]);

                                diametersFront[i] = meterToFtConverterDiamFront.getConvertedValues();

                                ConvLength meterToFtConverterXFront(Length::M, Length::FT, xCoordinatePerkinsFront[i]);

                                xCoordinatePerkinsFront[i] = meterToFtConverterXFront.getConvertedValues();

                                x1CentroidDistancesFront.push_back(wing.xloc - (fuseData.xStation[i]) / 2.0);

                                ConvLength meterToFtConverterX1Front(Length::M, Length::FT, x1CentroidDistancesFront[i]);

                                x1CentroidDistancesFront[i] = meterToFtConverterX1Front.getConvertedValues();

                                Interpolant interpGradient(x_A_curve, y_A_curve, 2, RegressionMethod::POLYNOMIAL);

                                gradientToLocalFlowAngleDueToUpWash = interpGradient.getYValueFromRegression(x1CentroidDistancesFront[i] / rootChordFeet);

                                summationFuselageContribution += std::pow(diametersFront[i], 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsFront[i]);
                            }
                            else
                            {
                                xCoordinatePerkinsFront.push_back(fuseData.xStation[i] - fuseData.xStation[i - 1]);
                                diametersFront.push_back(fuseData.allFuselageWidth[i]);
                                x1CentroidDistancesFront.push_back(wing.xloc - (fuseData.xStation[i] + fuseData.xStation[i - 1]) / 2.0);

                                ConvLength meterToFtConverterDiamFront(Length::M, Length::FT, diametersFront[i]);
                                diametersFront[i] = meterToFtConverterDiamFront.getConvertedValues();

                                ConvLength meterToFtConverterXFront(Length::M, Length::FT, xCoordinatePerkinsFront[i]);
                                xCoordinatePerkinsFront[i] = meterToFtConverterXFront.getConvertedValues();

                                ConvLength meterToFtConverterX1Front(Length::M, Length::FT, x1CentroidDistancesFront[i]);
                                x1CentroidDistancesFront[i] = meterToFtConverterX1Front.getConvertedValues();

                                Interpolant interpGradient(x_A_curve, y_A_curve, 2, RegressionMethod::POLYNOMIAL);

                                gradientToLocalFlowAngleDueToUpWash = interpGradient.getYValueFromRegression(x1CentroidDistancesFront[i] / rootChordFeet);

                                summationFuselageContribution += std::pow(diametersFront[i], 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsFront[i]);
                            }
                        }

                        // 2. Suddividi lo spazio tra fine nose e LE ala (cabina anteriore)

                        distanceNoseToWing = wing.xloc - fuseData.xStation[idxNoseEnd];
                        stepFront = distanceNoseToWing / numSubdivisionsFront;

                        ConvLength meterToFtConverterDistanceNoseToWing(Length::M, Length::FT, distanceNoseToWing);
                        distanceNoseToWing = meterToFtConverterDistanceNoseToWing.getConvertedValues();

                        ConvLength meterToFtConverterStepFront(Length::M, Length::FT, stepFront);
                        stepFrontFeet = meterToFtConverterStepFront.getConvertedValues();

                        for (int i = 0; i < numSubdivisionsFront; i++)
                        {
                            xCoordinatePerkinsFront.push_back(stepFrontFeet);
                            diametersFront.push_back(fuselageDiameterFeet); // Diametro costante = max diametro

                            if (i == 0)
                            {
                                x1CentroidDistancesFront.push_back(wing.xloc - (fuseData.xStation[idxNoseEnd] + stepFront / 2.0));
                            }
                            else
                            {
                                x1CentroidDistancesFront.push_back(wing.xloc - (fuseData.xStation[idxNoseEnd] + (i + 1) * stepFront));
                            }

                            ConvLength meterToFtConverterX1Front(Length::M, Length::FT, x1CentroidDistancesFront.back());
                            x1CentroidDistancesFront.back() = meterToFtConverterX1Front.getConvertedValues();

                            // Check if the current section  is the last one, to applay Perkins curves - Example at pag. 229 - Performance, stability and control
                            if (x1CentroidDistancesFront.back() == 0.0)
                            {

                                Interpolant interpGradient(x_B_curve, y_B_curve, 2, RegressionMethod::POLYNOMIAL);

                                gradientToLocalFlowAngleDueToUpWash = interpGradient.getYValueFromRegression(xCoordinatePerkinsFront.back() / rootChordFeet);

                                summationFuselageContribution += std::pow(diametersFront.back(), 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsFront.back());
                            }

                            else
                            {

                                Interpolant interpGradient(x_A_curve, y_A_curve, 2, RegressionMethod::POLYNOMIAL);

                                gradientToLocalFlowAngleDueToUpWash = interpGradient.getYValueFromRegression(x1CentroidDistancesFront.back() / rootChordFeet);

                                summationFuselageContribution += std::pow(diametersFront.back(), 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsFront.back());
                            }
                        }

                        // ========== PARTE POSTERIORE ==========

                        // 1. Suddividi lo spazio tra TE ala e inizio tail (cabina posteriore)

                        distanceWingToTail = fuseData.xStation[idxTailStart] - xTEWing;

                        // ConvLength meterToFtConverterDistanceWingToTail(Length::M, Length::FT, distanceWingToTail);
                        // distanceWingToTail = meterToFtConverterDistanceWingToTail.getConvertedValues();

                        stepRear = distanceWingToTail / numSubdivisionsRear;

                        ConvLength meterToFtConverterStepRear(Length::M, Length::FT, stepRear);
                        stepRearFeet = meterToFtConverterStepRear.getConvertedValues();

                        for (int i = 0; i < numSubdivisionsRear; i++)
                        {
                            xCoordinatePerkinsRear.push_back(stepRearFeet);
                            diametersRear.push_back(fuselageDiameterFeet); // Diametro costante = max diametro

                            if (i == 0)
                            {
                                x1CentroidDistancesRear.push_back(stepRear / 2.0);
                            }
                            else
                            {
                                x1CentroidDistancesRear.push_back((i + 1) * stepRear);
                            }

                            ConvLength meterToFtConverterX1Rear(Length::M, Length::FT, x1CentroidDistancesRear.back());
                            x1CentroidDistancesRear.back() = meterToFtConverterX1Rear.getConvertedValues();

                            gradientToLocalFlowAngleDueToUpWash = (x1CentroidDistancesRear[i] / distanceFromeTEWingToQuarterChordMacHorizontal) * (1 - downWashGradient);

                            summationFuselageContribution += std::pow(diametersRear[i], 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsRear[i]);
                        }

                        // 2. Aggiungi le sezioni della tail
                        for (size_t i = idxTailStart; i < (fuseData.xStation.size() - 1); i++)
                        {
                            if (i == idxTailStart)
                            {
                                xCoordinatePerkinsRear.push_back(fuseData.xStation[i + 1] - fuseData.xStation[i]);
                                diametersRear.push_back(fuseData.allFuselageWidth[i]);
                                x1CentroidDistancesRear.push_back(distanceWingToTail + (fuseData.xStation[i + 1] - fuseData.xStation[i]) / 2.0);
                            }
                            else
                            {
                                xCoordinatePerkinsRear.push_back(fuseData.xStation[i + 1] - fuseData.xStation[i]);
                                diametersRear.push_back(fuseData.allFuselageWidth[i]);
                                x1CentroidDistancesRear.push_back(distanceWingToTail + std::accumulate(xCoordinatePerkinsRear.begin() + numSubdivisionsRear, xCoordinatePerkinsRear.end(), 0.0) + (fuseData.xStation[i + 1] - fuseData.xStation[i]) / 2.0);
                            }

                            ConvLength meterToFtConverterDiamRear(Length::M, Length::FT, diametersRear.back());
                            diametersRear.back() = meterToFtConverterDiamRear.getConvertedValues();

                            ConvLength meterToFtConverterXRear(Length::M, Length::FT, xCoordinatePerkinsRear.back());
                            xCoordinatePerkinsRear.back() = meterToFtConverterXRear.getConvertedValues();

                            ConvLength meterToFtConverterX1Rear(Length::M, Length::FT, x1CentroidDistancesRear.back());
                            x1CentroidDistancesRear.back() = meterToFtConverterX1Rear.getConvertedValues();

                            gradientToLocalFlowAngleDueToUpWash = (x1CentroidDistancesRear.back() / distanceFromeTEWingToQuarterChordMacHorizontal) * (1 - downWashGradient);

                            summationFuselageContribution += std::pow(diametersRear.back(), 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsRear.back());
                        }

                        break;

                    } // Fine del costrutto caso TransportFuse

                    else if ((geomName == "Fuselage" || geomName == "Stack") && geomNameOfComponent == fuselage.id)
                    {

                        // Trova l'ultima sezione del nose
                        for (size_t i = 0; i < fuseData.xStation.size(); i++)
                        {
                            if (fuseData.xStation[i] <= wing.xloc)
                                idxNoseEnd = i; // Continua ad aggiornare finché siamo prima dell'ala
                            else
                                break; // Appena superiamo wing.xloc, ci fermiamo
                        }

                        // Trova la prima sezione della tail
                        for (size_t i = 0; i < fuseData.xStation.size(); i++)
                        {
                            if (fuseData.xStation[i] >= xTEWing)
                            {
                                idxTailStart = i;
                                break;
                            }
                        }

                        // ========== PARTE ANTERIORE ==========

                        // 1. Aggiungi le sezioni del nose
                        for (int i = 0; i <= idxNoseEnd; i++)
                        {
                            if (i == 0)
                            {
                                xCoordinatePerkinsFront.push_back(fuseData.xStation[i]);
                                diametersFront.push_back(fuseData.allFuselageWidth[i]);

                                ConvLength meterToFtConverterDiamFront(Length::M, Length::FT, diametersFront[i]);

                                diametersFront[i] = meterToFtConverterDiamFront.getConvertedValues();

                                ConvLength meterToFtConverterXFront(Length::M, Length::FT, xCoordinatePerkinsFront[i]);

                                xCoordinatePerkinsFront[i] = meterToFtConverterXFront.getConvertedValues();

                                x1CentroidDistancesFront.push_back(wing.xloc - (fuseData.xStation[i]) / 2.0);

                                ConvLength meterToFtConverterX1Front(Length::M, Length::FT, x1CentroidDistancesFront[i]);

                                x1CentroidDistancesFront[i] = meterToFtConverterX1Front.getConvertedValues();

                                Interpolant interpGradient(x_A_curve, y_A_curve, 2, RegressionMethod::POLYNOMIAL);

                                gradientToLocalFlowAngleDueToUpWash = interpGradient.getYValueFromRegression(x1CentroidDistancesFront[i] / rootChordFeet);

                                summationFuselageContribution += std::pow(diametersFront[i], 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsFront[i]);
                            }
                            else if (i < idxNoseEnd)
                            {
                                xCoordinatePerkinsFront.push_back(fuseData.xStation[i] - fuseData.xStation[i - 1]);
                                diametersFront.push_back(fuseData.allFuselageWidth[i]);
                                x1CentroidDistancesFront.push_back(wing.xloc - (fuseData.xStation[i] + fuseData.xStation[i - 1]) / 2.0);

                                ConvLength meterToFtConverterDiamFront(Length::M, Length::FT, diametersFront[i]);
                                diametersFront[i] = meterToFtConverterDiamFront.getConvertedValues();

                                ConvLength meterToFtConverterXFront(Length::M, Length::FT, xCoordinatePerkinsFront[i]);
                                xCoordinatePerkinsFront[i] = meterToFtConverterXFront.getConvertedValues();

                                ConvLength meterToFtConverterX1Front(Length::M, Length::FT, x1CentroidDistancesFront[i]);
                                x1CentroidDistancesFront[i] = meterToFtConverterX1Front.getConvertedValues();

                                Interpolant interpGradient(x_A_curve, y_A_curve, 2, RegressionMethod::POLYNOMIAL);

                                gradientToLocalFlowAngleDueToUpWash = interpGradient.getYValueFromRegression(x1CentroidDistancesFront[i] / rootChordFeet);

                                summationFuselageContribution += std::pow(diametersFront[i], 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsFront[i]);
                            }

                            else
                            {

                                xCoordinatePerkinsFront.push_back(fuseData.xStation[i] - fuseData.xStation[i - 1]);
                                diametersFront.push_back(fuseData.allFuselageWidth[i]);
                                x1CentroidDistancesFront.push_back(wing.xloc - (fuseData.xStation[i] + fuseData.xStation[i - 1]) / 2.0);

                                ConvLength meterToFtConverterDiamFront(Length::M, Length::FT, diametersFront[i]);
                                diametersFront[i] = meterToFtConverterDiamFront.getConvertedValues();

                                ConvLength meterToFtConverterXFront(Length::M, Length::FT, xCoordinatePerkinsFront[i]);
                                xCoordinatePerkinsFront[i] = meterToFtConverterXFront.getConvertedValues();

                                ConvLength meterToFtConverterX1Front(Length::M, Length::FT, x1CentroidDistancesFront[i]);
                                x1CentroidDistancesFront[i] = meterToFtConverterX1Front.getConvertedValues();

                                Interpolant interpGradient(x_B_curve, y_B_curve, 2, RegressionMethod::POLYNOMIAL);

                                gradientToLocalFlowAngleDueToUpWash = interpGradient.getYValueFromRegression(xCoordinatePerkinsFront.back() / rootChordFeet);

                                summationFuselageContribution += std::pow(diametersFront.back(), 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsFront.back());
                            }
                        }

                        // // ========== PARTE POSTERIORE ==========

                        // Distnace from wing TE to the first tail section

                        distanceWingToTail = fuseData.xStation[idxTailStart] - xTEWing;

                        for (size_t i = idxTailStart; i < (fuseData.xStation.size() - 1); i++)
                        {
                            if (i == idxTailStart)
                            {
                                xCoordinatePerkinsRear.push_back(fuseData.xStation[i + 1] - fuseData.xStation[i]);
                                diametersRear.push_back(fuseData.allFuselageWidth[i]);
                                x1CentroidDistancesRear.push_back(distanceWingToTail + (fuseData.xStation[i + 1] - fuseData.xStation[i]) / 2.0);
                            }
                            else
                            {
                                xCoordinatePerkinsRear.push_back(fuseData.xStation[i + 1] - fuseData.xStation[i]);
                                diametersRear.push_back(fuseData.allFuselageWidth[i]);
                                x1CentroidDistancesRear.push_back(distanceWingToTail + std::accumulate(xCoordinatePerkinsRear.begin() + idxTailStart, xCoordinatePerkinsRear.end(), 0.0) +
                                                                  (fuseData.xStation[i + 1] - fuseData.xStation[i]) / 2.0);
                            }

                            ConvLength meterToFtConverterDiamRear(Length::M, Length::FT, diametersRear.back());
                            diametersRear.back() = meterToFtConverterDiamRear.getConvertedValues();

                            ConvLength meterToFtConverterXRear(Length::M, Length::FT, xCoordinatePerkinsRear.back());
                            xCoordinatePerkinsRear.back() = meterToFtConverterXRear.getConvertedValues();

                            ConvLength meterToFtConverterX1Rear(Length::M, Length::FT, x1CentroidDistancesRear.back());
                            x1CentroidDistancesRear.back() = meterToFtConverterX1Rear.getConvertedValues();

                            gradientToLocalFlowAngleDueToUpWash = (x1CentroidDistancesRear.back() / distanceFromeTEWingToQuarterChordMacHorizontal) * (1 - downWashGradient);

                            summationFuselageContribution += std::pow(diametersRear.back(), 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsRear.back());
                        }

                        break;
                    }
                } // Fine for per nomi geometrie OpenVsp
            }

            deltaCmDeltaAlphaFuselage = ((psfDynamicPressure / 36.5) * summationFuselageContribution) / (psfDynamicPressure * planformWingAreaSquareFeet * wingMACFeet);
            deltaCmClFuselageContribution = ((psfDynamicPressure / 36.5) * summationFuselageContribution) / (psfDynamicPressure * planformWingAreaSquareFeet * wingMACFeet * finiteWingLiftSlope);

            // ========================================================================
            // STEP 10: Calculate Nacelle Stability
            // ========================================================================

            VSPGEOMTRYEXTRACTOR::DiametersExtractor nacelleExtractor;

            VSPGEOMTRYEXTRACTOR::NacelleDiametersAndXStation nacelleData = nacelleExtractor.extractNacelleDiameters(
                builder.getCommonData().getNameOfAircraft(),
                allGeomData,
                nacelle.length);

            if (builder.getCommonData().getEnginePosition() == EnginePosition::WING_MOUNTED)
            {

                if (!builder.getCommonData().getIsSweptWing())
                {
                    for (size_t j = 0; j < nacelleData.xStation.size(); j++)
                    {

                        // FRONT NACELLE SECTIONS
                        if ((nacelle.xloc.front() + nacelleData.xStation[j]) <= wing.xloc)
                        {
                            indexFoundLastSectionNacelleFront += 1;

                            if (j == 0)
                            {

                                deltaXNacellePerkinsFront.push_back(nacelleData.xStation[j]);
                                diametersNacelleFront.push_back(nacelleData.allNacelleWidth[j]);
                                x1CentroidDistancesNacelleFront.push_back(wing.xloc - (nacelle.xloc.front() + nacelleData.xStation[j]) / 2.0);
                            }

                            else
                            {

                                deltaXNacellePerkinsFront.push_back(nacelleData.xStation[j] - nacelleData.xStation[j - 1]);
                                diametersNacelleFront.push_back(nacelleData.allNacelleWidth[j]);
                                x1CentroidDistancesNacelleFront.push_back(wing.xloc - (nacelle.xloc.front() + nacelleData.xStation[j - 1] + deltaXNacellePerkinsFront.back() / 2.0));
                            }
                        }

                        // REAR NACELLE SECTIONS
                        else if ((nacelle.xloc.front() + nacelleData.xStation[j]) > xTEWing)
                        {

                            indexFoundLastSectionNacelleRear = j;
                            deltaXNacellePerkinsRear.push_back(nacelleData.xStation[j] - nacelleData.xStation[j - 1]);
                            diametersNacelleRear.push_back(nacelleData.allNacelleWidth[j]);
                            x1CentroidDistancesNacelleRear.push_back((nacelle.xloc.front() + nacelleData.xStation[j - 1] + deltaXNacellePerkinsRear.back() / 2.0) - xTEWing);
                        }
                    }

                    // FRONT NACELLE CONTRIBUTION

                    ConvLength meterToFeetFrontNacelleDiametr(Length::M, Length::FT, diametersNacelleFront);

                    frontNacelleDiamterFeet = meterToFeetFrontNacelleDiametr.getConvertedValues();

                    ConvLength meterToFeetFrontNacelleX(Length::M, Length::FT, deltaXNacellePerkinsFront);

                    frontNacelleDeltaXFeet = meterToFeetFrontNacelleX.getConvertedValues();

                    ConvLength meterToFeetFrontNacelleX1(Length::M, Length::FT, x1CentroidDistancesNacelleFront);

                    x1CentroidDistancesNacelleFront = meterToFeetFrontNacelleX1.getConvertedValues();

                    ConvLength meterToFeetRearNacelleDiametr(Length::M, Length::FT, diametersNacelleRear);

                    rearNacelleDiamterFeet = meterToFeetRearNacelleDiametr.getConvertedValues();

                    ConvLength meterToFeetRearNacelleX(Length::M, Length::FT, deltaXNacellePerkinsRear);

                    rearNacelleDeltaXFeet = meterToFeetRearNacelleX.getConvertedValues();

                    ConvLength meterToFeetRearNacelleX1(Length::M, Length::FT, x1CentroidDistancesNacelleRear);

                    x1CentroidDistancesNacelleRear = meterToFeetRearNacelleX1.getConvertedValues();

                    for (size_t i = 0; i < diametersNacelleFront.size(); i++)
                    {

                        if (i != indexFoundLastSectionNacelleFront)
                        {
                            Interpolant interpGradient(x_A_curve, y_A_curve, 2, RegressionMethod::POLYNOMIAL);

                            gradientToLocalFlowAngleDueToUpWashNacelle = interpGradient.getYValueFromRegression(x1CentroidDistancesNacelleFront[i] / rootChordFeet);

                            summationNacelleContribution += std::pow(frontNacelleDiamterFeet[i], 2.0) * gradientToLocalFlowAngleDueToUpWashNacelle * (frontNacelleDeltaXFeet[i]);
                        }
                        else
                        {

                            Interpolant interpGradient(x_B_curve, y_B_curve, 2, RegressionMethod::POLYNOMIAL);

                            gradientToLocalFlowAngleDueToUpWashNacelle = interpGradient.getYValueFromRegression(deltaXNacellePerkinsFront[indexFoundLastSectionNacelleFront] / rootChordFeet);

                            summationNacelleContribution += std::pow(frontNacelleDiamterFeet[i], 2.0) * gradientToLocalFlowAngleDueToUpWashNacelle * (frontNacelleDeltaXFeet[i]);
                        }
                    }

                    for (size_t i = 0; i < diametersNacelleRear.size(); i++)
                    {

                        gradientToLocalFlowAngleDueToUpWashNacelle = (x1CentroidDistancesNacelleRear[i] / distanceFromeTEWingToQuarterChordMacHorizontal) * (1 - downWashGradient);

                        summationNacelleContribution += std::pow(rearNacelleDiamterFeet[i], 2.0) * gradientToLocalFlowAngleDueToUpWashNacelle * (rearNacelleDeltaXFeet[i]);
                    }

                    deltaCmDeltaAlphaNacelle = builder.getEngineData().getNumberOfEngines() * (((psfDynamicPressure / 36.5) * summationNacelleContribution) / (psfDynamicPressure * planformWingAreaSquareFeet * wingMACFeet));
                    deltaCmClNacelleContribution = builder.getEngineData().getNumberOfEngines() * ((psfDynamicPressure / 36.5) * summationNacelleContribution) / (psfDynamicPressure * planformWingAreaSquareFeet * wingMACFeet * finiteWingLiftSlope);
                }

                else
                {

                    // Deferenzio per ottenere il valore mx del vettore, dato che std::max_element restituisce un iteratore
                    ConvLength meterToFeetNacelleWidth(Length::M, Length::FT, nacelleData.allNacelleWidth);

                    maxNacelleWidthFeet = *(std::max_element(meterToFeetNacelleWidth.getConvertedValues().begin(), meterToFeetNacelleWidth.getConvertedValues().end()));

                    ConvLength meterToFeetNacelleLength(Length::M, Length::FT, nacelle.length);
                    nacelleLengthFeet = meterToFeetNacelleLength.getConvertedValues();

                    for (size_t n = 0; n < nacelle.yloc.size(); n++)
                    {

                        Interpolant interpKfFactor(Kf_x, Kf_y, 1, RegressionMethod::EXPONENTIAL);

                        nacelleMomentFactor = interpKfFactor.getYValueFromRegression((wing.xloc + 0.25 * wing.croot.front() - nacelle.xloc.front()) / nacelle.length);

                        deltaCmDeltaAlphaNacelle += (nacelleMomentFactor * std::pow(maxNacelleWidthFeet, 2.0) * nacelleLengthFeet) / (planformWingAreaSquareFeet * wingMACFeet);

                        deltaCmClNacelleContribution += (nacelleMomentFactor * std::pow(maxNacelleWidthFeet, 2.0) * nacelleLengthFeet) / (planformWingAreaSquareFeet * wingMACFeet * finiteWingLiftSlope);
                    }
                }
            }

            else
            {

                for (size_t j = 0; j < nacelleData.xStation.size(); j++)
                {

                    if (j == 0)
                    {

                        deltaXNacellePerkinsRear.push_back(nacelleData.xStation[j]);
                        diametersNacelleRear.push_back(nacelleData.allNacelleWidth[j]);
                        x1CentroidDistancesNacelleRear.push_back((nacelle.xloc.front() + deltaXNacellePerkinsRear.back() / 2.0) - xTEWing);
                    }

                    else
                    {
                        deltaXNacellePerkinsRear.push_back(nacelleData.xStation[j] - nacelleData.xStation[j - 1]);
                        diametersNacelleRear.push_back(nacelleData.allNacelleWidth[j]);
                        x1CentroidDistancesNacelleRear.push_back((nacelle.xloc.front() + nacelleData.xStation[j - 1] + deltaXNacellePerkinsRear.back() / 2.0) - xTEWing);
                    }
                }

                ConvLength meterToFeetRearNacelleDiametr(Length::M, Length::FT, diametersNacelleRear);

                rearNacelleDiamterFeet = meterToFeetRearNacelleDiametr.getConvertedValues();

                ConvLength meterToFeetRearNacelleX(Length::M, Length::FT, deltaXNacellePerkinsRear);

                rearNacelleDeltaXFeet = meterToFeetRearNacelleX.getConvertedValues();

                ConvLength meterToFeetRearNacelleX1(Length::M, Length::FT, x1CentroidDistancesNacelleRear);

                x1CentroidDistancesNacelleRear = meterToFeetRearNacelleX1.getConvertedValues();

                for (size_t i = 0; i < diametersNacelleRear.size(); i++)
                {

                    gradientToLocalFlowAngleDueToUpWashNacelle = (x1CentroidDistancesNacelleRear[i] / distanceFromeTEWingToQuarterChordMacHorizontal) * (1 - downWashGradient);

                    summationNacelleContribution += std::pow(rearNacelleDiamterFeet[i], 2.0) * gradientToLocalFlowAngleDueToUpWashNacelle * (rearNacelleDeltaXFeet[i]);
                }

                deltaCmDeltaAlphaNacelle = builder.getEngineData().getNumberOfEngines() * (((psfDynamicPressure / 36.5) * summationNacelleContribution) / (psfDynamicPressure * planformWingAreaSquareFeet * wingMACFeet));
                deltaCmClNacelleContribution = builder.getEngineData().getNumberOfEngines() * ((psfDynamicPressure / 36.5) * summationNacelleContribution) / (psfDynamicPressure * planformWingAreaSquareFeet * wingMACFeet * finiteWingLiftSlope);
            }

            // ========================================================================
            // STEP 11: Calculate Aircraft Total Longitudinal Stability Derivative
            // ========================================================================

            deltaCmDeltaAlphaAircraft = deltaCmDeltaAlphaFuselage + deltaCmDeltaAlphaNacelle + deltaCmClWingContribution * finiteWingLiftSlope + deltaCmClHorizontalTailContribution * finiteWingLiftSlope + deltaCmClCanardContribution * finiteWingLiftSlope;
            deltaCmDeltaClAircraft = deltaCmClWingContribution + deltaCmClHorizontalTailContribution + deltaCmClCanardContribution + deltaCmClFuselageContribution + deltaCmClNacelleContribution;

            // ========================================================================
            // STEP 12: Calculate Flap Effectivness
            // ========================================================================

            for (size_t n = 0; n < horizontalTail.mov.type.size(); n++)
            {

                if (horizontalTail.mov.type[n] == 'e')
                {

                    // Larghezza del segmento n-esimo
                    double segmentSpan = (horizontalTail.mov.eta_outer[n] - horizontalTail.mov.eta_inner[n]) * horizontalTail.totalProjectedSpan;

                    // cf/c medio del segmento = media tra inner e outer
                    double cfcMean = 0.5 * (horizontalTail.mov.cf_c_inner[n] + horizontalTail.mov.cf_c_outer[n]);

                    numeratorAverageChordRatio += cfcMean * segmentSpan;
                    weightsOfWeightedAverageChordRatio += segmentSpan;
                }
            }

            averageChordRatio = numeratorAverageChordRatio / weightsOfWeightedAverageChordRatio;

            for (size_t i = 0; i < horizontalTail.mov.type.size(); i++)
            {

                if (horizontalTail.mov.type[i] == 'e')
                {

                    etaInner = horizontalTail.mov.eta_inner[i];
                    etaOuter = horizontalTail.mov.eta_outer[i];

                    Interpolant2D kbFactorInterpolator(3, RegressionMethod::POLYNOMIAL);

                    kbFactorInterpolator.addCurve(0.0, x_Kb_taper0, y_Kb_taper0);
                    kbFactorInterpolator.addCurve(0.5, x_Kb_taper05, y_Kb_taper05);
                    kbFactorInterpolator.addCurve(1.0, x_Kb_taper1, y_Kb_taper1);

                    factorKbInner = kbFactorInterpolator.interpolate(etaInner, horizontalTail.taperRatio);
                    factorKbOuter = kbFactorInterpolator.interpolate(etaOuter, horizontalTail.taperRatio);

                    thetaFactor = std::acos(2 * averageChordRatio - 1);

                    twoDimensionalFlapEffectivness = 1 - ((thetaFactor - std::sin(thetaFactor)) / M_PI);

                    flapSpanFactorKb.push_back(factorKbOuter - factorKbInner);

                    Interpolant2D ratioFlapEffectivnessInterpolator(1, RegressionMethod::POWER);

                    ratioFlapEffectivnessInterpolator.addCurve(0.1, x_RatioFlapEff_cf01, y_RatioFlapEff_cf01);
                    ratioFlapEffectivnessInterpolator.addCurve(0.2, x_RatioFlapEff_cf02, y_RatioFlapEff_cf02);
                    ratioFlapEffectivnessInterpolator.addCurve(0.3, x_RatioFlapEff_cf03, y_RatioFlapEff_cf03);
                    ratioFlapEffectivnessInterpolator.addCurve(0.4, x_RatioFlapEff_cf04, y_RatioFlapEff_cf04);
                    ratioFlapEffectivnessInterpolator.addCurve(0.5, x_RatioFlapEff_cf05, y_RatioFlapEff_cf05);
                    ratioFlapEffectivnessInterpolator.addCurve(0.6, x_RatioFlapEff_cf06, y_RatioFlapEff_cf06);
                    ratioFlapEffectivnessInterpolator.addCurve(0.7, x_RatioFlapEff_cf07, y_RatioFlapEff_cf07);
                    ratioFlapEffectivnessInterpolator.addCurve(0.8, x_RatioFlapEff_cf08, y_RatioFlapEff_cf08);
                    ratioFlapEffectivnessInterpolator.addCurve(0.9, x_RatioFlapEff_cf09, y_RatioFlapEff_cf09);

                    ratioEffectivnessFactorKc.push_back(ratioFlapEffectivnessInterpolator.interpolate(horizontalTail.aspectRatio, twoDimensionalFlapEffectivness));

                    tauFlap += flapSpanFactorKb.back() * ratioEffectivnessFactorKc.back() * twoDimensionalFlapEffectivness * finiteHorizontalTailLiftSlope / builder.getCommonData().getMeanAirfoilSlopeHorizontalTail();
                }
            }

            factorToAccountHorizontalTailContributionAndDownwash = std::pow(1 + (finiteHorizontalTailLiftSlope / finiteWingLiftSlope) *
                                                                                    (horizontalTail.planformArea / wing.planformArea) * downWashGradient,
                                                                            -1);

            // ========================================================================
            // STEP 13: Calculate the elevator control power dCm/dElevator and dCm/dih, dCL/dAlpha, dCL/dElevator, dCL/dih, 
            // ========================================================================

            switch (builder.getCommonData().getTypeOfTail())
            {

            case TypeOfTail::CONVENTIONAL_TAIL:
            case TypeOfTail::U_TAIL:
            case TypeOfTail::H_TAIL:

                deltaCmDeltaElevatorDeflection = -finiteHorizontalTailLiftSlope * 0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) *
                                                 ((horizontalTail.planformArea / wing.planformArea) * (fuselage.tailArm / wing.MAC) *
                                                  factorToAccountHorizontalTailContributionAndDownwash) *
                                                 tauFlap;

                deltaCLDeltaElevatorDeflection = finiteHorizontalTailLiftSlope * 0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) *
                                                 (horizontalTail.planformArea / wing.planformArea) * tauFlap;

                deltaCLDeltaIncidenceHorizontalTail = finiteHorizontalTailLiftSlope * 0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) *
                                                      (horizontalTail.planformArea / wing.planformArea);

                deltaCmDeltaIncidenceHorizontalTail = - finiteHorizontalTailLiftSlope * 0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) *
                                                      (horizontalTail.planformArea / wing.planformArea) * (fuselage.tailArm / wing.MAC);

                liftSlopeAircraft = finiteWingLiftSlope * (1 + (finiteHorizontalTailLiftSlope / finiteWingLiftSlope) * (horizontalTail.planformArea / wing.planformArea) * 
                                                           0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) * downWashGradient);

                break;

            case TypeOfTail::V_TAIL:
            case TypeOfTail::V_REV_TAIL:
                deltaCmDeltaElevatorDeflection = -finiteHorizontalTailLiftSlope * 0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) *
                                                 ((horizontalTail.planformArea / wing.planformArea) * (fuselage.tailArm / wing.MAC) *
                                                  factorToAccountHorizontalTailContributionAndDownwash) *
                                                 tauFlap;

                deltaCLDeltaElevatorDeflection = finiteHorizontalTailLiftSlope * 0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) *
                                                 (horizontalTail.planformArea / wing.planformArea) * tauFlap;

                deltaCLDeltaIncidenceHorizontalTail = finiteHorizontalTailLiftSlope * 0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) *
                                                      (horizontalTail.planformArea / wing.planformArea);

                deltaCmDeltaIncidenceHorizontalTail = - finiteHorizontalTailLiftSlope * 0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) *
                                                      (horizontalTail.planformArea / wing.planformArea) * (fuselage.tailArm / wing.MAC);

                liftSlopeAircraft = finiteWingLiftSlope * (1 + (finiteHorizontalTailLiftSlope / finiteWingLiftSlope) * (horizontalTail.planformArea / wing.planformArea) * 
                                                          0.5 * (etaMinimumConventionalTail + etaMaximumConventionalTail) * downWashGradient);

                break;

            case TypeOfTail::T_TAIL:
                deltaCmDeltaElevatorDeflection = -finiteHorizontalTailLiftSlope *
                                                 ((horizontalTail.planformArea / wing.planformArea) * (fuselage.tailArm / wing.MAC) *
                                                  factorToAccountHorizontalTailContributionAndDownwash) *
                                                 tauFlap;

                deltaCLDeltaElevatorDeflection = finiteHorizontalTailLiftSlope * (horizontalTail.planformArea / wing.planformArea) * tauFlap;

                deltaCLDeltaIncidenceHorizontalTail = finiteHorizontalTailLiftSlope * (horizontalTail.planformArea / wing.planformArea);

                deltaCmDeltaIncidenceHorizontalTail = - finiteHorizontalTailLiftSlope * (horizontalTail.planformArea / wing.planformArea) * (fuselage.tailArm / wing.MAC);

                liftSlopeAircraft = finiteWingLiftSlope * (1 + (finiteHorizontalTailLiftSlope / finiteWingLiftSlope) * (horizontalTail.planformArea / wing.planformArea) * downWashGradient);

                break;
            }

            // ========================================================================
            // STEP 14: Calculate CM0 and CL0 in the WFH configuration (Wing-Fuselage-Horizontal)
            // ========================================================================

            RestoreSettings settingsRestore;

            settingsRestore.getSavePrevoiusSettings(settings);

            settingsRestore.getSavePrevoiusAircraftInfo(aircraftInfo);

            SILENTORCOMPONENT::SilentorComponent silentor(builder.getCommonData().getNameOfAircraft(),
                                                         "Silent_components.vspscript");

            for (size_t ii = 0; settings.AoA.size(); ii++)
            {

                if (settings.AoA[ii] == 0.0)
                {
                    foundIndexAtZeroAoA = ii;

                    break;
                }

                else if (ii == settings.AoA.size() - 1 && settings.AoA[ii] != 0.0)
                {
                    settings.AoA.push_back(0.0);

                    foundIndexAtZeroAoA = ii + 1;
                }
            }

            // Disable every deflections of the control surfaces.

            for (size_t n = 0; n < wing.mov.defl.size(); n++)
            {

                if (wing.mov.defl[n] != 0.0)
                {
                    std::cout << "Warning: Wing control surface deflection detected. Lateral stability derivatives will be calculated for the undeflected configuration only." << std::endl;

                    wing.mov.defl[n] = 0.0;

                    aircraftInfo.wing.mov.defl[n] = 0.0;
                }
            }

            if (canard != nullptr)
            {
                for (size_t n = 0; n < canard->mov.defl.size(); n++)
                {

                    if (canard->mov.defl[n] != 0.0)
                    {
                        std::cout << "Warning: Canard control surface deflection detected. Lateral stability derivatives will be calculated for the undeflected configuration only." << std::endl;

                        canard->mov.defl[n] = 0.0;

                        aircraftInfo.canard.mov.defl[n] = 0.0;
                    }
                }
            }

            for (size_t n = 0; n < horizontalTail.mov.defl.size(); n++)
            {

                if (horizontalTail.mov.defl[n] != 0.0)
                {
                    std::cout << "Warning: Horizontal Tail control surface deflection detected. Lateral stability derivatives will be calculated for the undeflected configuration only." << std::endl;

                    horizontalTail.mov.defl[n] = 0.0;

                    aircraftInfo.hor.mov.defl[n] = 0.0;
                }
            }

            if (verticalTail != nullptr)
            {
                for (size_t n = 0; n < verticalTail->mov.defl.size(); n++)
                {

                    if (verticalTail->mov.defl[n] != 0.0)
                    {
                        std::cout << "Warning: Vertical Tail control surface deflection detected. Lateral stability derivatives will be calculated for the undeflected configuration only." << std::endl;

                        verticalTail->mov.defl[n] = 0.0;

                        aircraftInfo.ver.mov.defl[n] = 0.0;
                    }
                }
            }

            // Calculate CL0wfh and Cm0wfh

            if (!builder.getCommonData().getHasCanard())
            {
                silentor.GetGeometryWithThisComponent(aircraftInfo, allGeomData, {wing.id, fuselage.id, horizontalTail.id});
            }

            else
            {
                silentor.GetGeometryWithThisComponent(aircraftInfo, allGeomData, {wing.id, canard->id, fuselage.id, horizontalTail.id});
            }

            silentor.executeAnalysis(settings);

            pitchingMomentAtZeroAoA = silentor.getAerodynamicCoefficients().pitchingMomentCoefficient.at(foundIndexAtZeroAoA);
            liftCoefficientAtZeroAoA = silentor.getAerodynamicCoefficients().liftCoefficient.at(foundIndexAtZeroAoA);

            settings = settingsRestore.getSettingsToRestore();
            aircraftInfo = settingsRestore.getAircrfatInfoToRestore();

            // ========================================================================
            // STEP 14.1: Neutral point stick-fixed, calculation
            // ========================================================================

            xCGAsFractionOfMAC = (settings.X_cg - wing.xloc - wing.deltaXtoLEMAC) / wing.MAC;

            neutralPointAsFractionOfMAC = xCGAsFractionOfMAC - deltaCmDeltaClAircraft;


            // ========================================================================
            // STEP 14.2: Saving results in the structs
            // ========================================================================

            aircraftLongitudinalDerivatives = {.deltaCmDeltaAlphaAircraft = deltaCmDeltaAlphaAircraft,
                                               .deltaCmDeltaClAircraft = deltaCmDeltaClAircraft,
                                               .deltaCmDeltaElevatorDeflection = deltaCmDeltaElevatorDeflection,
                                               .deltaCmDeltaIncidenceHorizontalTail = deltaCmDeltaIncidenceHorizontalTail,
                                               .deltaCLDeltaElevatorDeflection = deltaCLDeltaElevatorDeflection,
                                               .deltaCLDeltaIncidenceHorizontalTail = deltaCLDeltaIncidenceHorizontalTail,
                                               .liftSlopeAircraft = liftSlopeAircraft,
                                               .neutralPointAsFractionOfMAC = neutralPointAsFractionOfMAC};

            singleComponentsDerivatives = {.deltaCmDeltaAlphaWing = deltaCmClWingContribution * finiteWingLiftSlope,
                                           .deltaCmDeltaAlphaCanard = deltaCmClCanardContribution * finiteWingLiftSlope,
                                           .deltaCmDeltaAlphaHorizontalTail = deltaCmClHorizontalTailContribution * finiteWingLiftSlope,
                                           .deltaCmDeltaAlphaFuselage = deltaCmDeltaAlphaFuselage,
                                           .deltaCmDeltaAlphaNacelle = deltaCmDeltaAlphaNacelle};

            longitudinalAerodynamicCoefficients = {.pitchingMomentAtZeroAoA = pitchingMomentAtZeroAoA,
                                                   .liftCoefficientAtZeroAoA = liftCoefficientAtZeroAoA};

            // ========================================================================
            // Optional: Plot downwash angle vs angle of attack (currently commented)
            // ========================================================================
            // Plot plot(odeSolver45.getT(), odeSolver45.getY(),
            // "Alpha (deg)",
            // "Epsilon (deg)",
            // "ODE45 - Solver",
            // "Eps Angle",
            // "lines","1","1","","","blue");
        }

        // Getters for the results
        LONGITUDINAL_STABILITY::LongitudinalStabilityDerivatives getTotalAircrfatLongitudinalDerivatives() const { return aircraftLongitudinalDerivatives; }
        LONGITUDINAL_STABILITY::LongitudinalStabilityDerivativesToSingleComponent getLongitudinalStabilityDerivativesToSingleComponent() const { return singleComponentsDerivatives; }
        LONGITUDINAL_STABILITY::LongitudinalAerodynamicCoefficients getLongitudinalAerodynamicCoefficients() const { return longitudinalAerodynamicCoefficients;}
    };
};
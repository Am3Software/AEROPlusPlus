/**
 * @file LATERALSTABILITY.h
 * @brief Header file for lateral stability calculations
 *
 * This file contains classes and structures for calculating lateral stability
 * derivatives of an aircraft based on geometric and aerodynamic parameters.
 */

#pragma once
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#ifndef M_PI
constexpr double M_PI = 3.14159265358979323846;
#endif
#include <string>
#include <algorithm>
#include "VSPScriptGenerator.h"
#include "VSPGEOMETRYEXTRACTOR.h"
#include "DELTAXANDDIAMETERS.h"
#include "DIRECTIONALSTABILITY.h"
#include "Interpolant.h"
#include "Interpolant2D.h"
#include "Interpolant3D.h"
#include "BASEAIRCRAFTDATA.h"
#include "WINGBASEDATA.h"
#include "FUSELAGEBASEDATA.h"
#include "ENGINEBASEDATA.h"
#include "BUILDAIRCRAFT.h"
#include "COGCALCULATOR.h"
#include "EnumAircraftCategory.h"
#include "EnumEnginePosition.h"
#include "EnumTypeOfWing.h"
#include "EnumWingPosition.h"
#include "RegressionMethod.h"
#include "SILENTORCOMPONENT.h"
#include "VSPAeroGenerator.h"
#include "VSPScriptGenerator.h"
#include "RestoreSettings.h"

namespace LATERAL_STABILITY
{
    /**
     * @struct LateralStabilityDerivatives
     * @brief Contains the overall aircraft lateral stability derivatives
     */
    struct LateralStabilityDerivatives
    {
        double deltaClDeltaBetaAircraft = 0.0; ///< Roll moment coefficient derivative with respect to sideslip angle
        double deltaClBetaDeltaAileronsDeflection = 0.0; ///< Roll moment coefficient derivative with respect to aileron deflection
    };

    /**
     * @struct LateralStabilityDerivativesRollToSingleComponent
     * @brief Contains lateral stability contributions related to roll moment from individual aircraft components
     */
    struct LateralStabilityDerivativesRollToSingleComponent
    {
        double deltaClDeltaBetaWingFuselageContribution = 0.0;   ///< Wing-Fuselage contribution to roll moment derivative
        double deltaClDeltaBetaCanardContribution = 0.0;         ///< Canard contribution to roll moment derivative
        double deltaClDeltaBetaHorizontalTailContribution = 0.0; ///< Horizontal tail contribution to roll moment derivative
        double deltaClDeltaBetaVerticalTailContribution = 0.0;   ///< Vertical tail contribution to roll moment derivative
        double deltaClDeltaBetaNacelleContribution = 0.0;        ///< Nacelle contribution to roll moment derivative
    };

    /**
     * @class LateralStabilityCalculator
     * @brief Calculates lateral stability derivatives for an aircraft (dCl/dBeta)
     *
     * This class implements an hybridization of Perkins, Roskam and DATCOM methods for computing lateral
     * stability contributions from various aircraft components including wing,
     * fuselage, tail surfaces, nacelles, and propellers.
     */
    class LateralStabilityCalculator
    {

    protected:
        BuildAircraft &builder; ///< Reference to the aircraft builder
        VSP::Aircraft aircraftInfo;
        COG::COGDATA cogData;       ///< Center of gravity data
        COG::COGDATA cogComponents; ///< Components COG data

    private:
        // Aircraft component geometry data
        VSP::Wing wing;                    ///< Main wing geometry
        VSP::Wing canard;                  ///< Canard geometry
        VSP::Wing horizontalTail;          ///< Horizontal tail geometry
        VSP::Wing verticalTail;            ///< Vertical tail geometry
        VSP::Fuselage fuselage;            ///< Fuselage geometry
        VSP::Nacelle nacelle;              ///< Nacelle geometry
        VSP::Disk disk;                    ///< Disk geometry for propeller contributions
        VSP::AeroSettings settings;        ///< Aerodynamic analysis settings
        RegressionMethod regressionMethod; ///< Regression method for interpolations

        // Stability derivatives
        LATERAL_STABILITY::LateralStabilityDerivativesRollToSingleComponent singleComponentsDerivativesRoll; ///< Component-level roll moment derivatives
        LATERAL_STABILITY::LateralStabilityDerivatives aircraftDerivativesRoll;                              ///< Total aircraft lateral stability derivative

        // Directional Stability usefule variables
        DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesSideForceToSingleComponent singleComponentsDerivativesSideForce; ///< Component-level side force derivatives

        std::string nameOfAircraft; ///< Aircraft name identifier

        // Component contributions to lateral stability
        double deltaClDeltaBetaWingFuselageContribution = 0.0;   ///< Wing roll moment coefficient derivative
        double deltaClDeltaBetaHorizontalTailContribution = 0.0; ///< Horizontal tail roll moment coefficient derivative
        double deltaClDeltaBetaCanardContribution = 0.0;         ///< Canard roll moment coefficient derivative
        double deltaClDeltaBetaVerticalTailContribution = 0.0;   ///< Vertical tail roll moment coefficient derivative
        double deltaClDeltaBetaFuselageContribution = 0.0;       ///< Fuselage roll moment coefficient derivative
        double deltaClDeltaBetaNacelleContribution = 0.0;        ///< Nacelle roll moment coefficient derivative

        // Total aircraft derivatives
        double deltaClDeltaBetaAircraft = 0.0; ///< Total aircraft roll moment derivative wrt sideslip
        double deltaClBetaDeltaAileronsDeflection = 0.0;  ///< Roll moment coefficient derivative with respect to aileron deflection

        // Fuselage variables related
        std::vector<double> diametersFromXPointWhereFlowCasesToBePotential; ///< Diameters from the point where flow becomes potential  [m]
        std::vector<double> relativeCrossSectionAreaAtXStationFuselage;     ///< Relative cross-sectional area at fuselage x-stations
        std::vector<double> realtiveWidthAtXstationFuselage;                ///< Relative width at fuselage x-stations
        double numeratorweightsAndCrossSectionArea = 0.0;
        double weightsOfWeightedAverageCrossSectionArea = 0.0;
        double averageCrossSectionArea = 0.0;
        double ratioWingFuselageRollBetaLiftCoefficientDueToSweepC2 = 0.0;
        double factorWingFuselageCompressibilityCorrectionToWingSweep = 0.0;
        double factorWingFuselageCorrection = 0.0;
        double distanceFromeNoseToQuarterChordTipWing = 0.0;
        double ratioWingFuselageClBetaToCLAspectRatio = 0.0;
        double ratioWingFuselageClBetaDihedralDueToSweepC2 = 0.0;
        double factorWingFuselageToWingDihedral = 0.0;
        double factorWingFuselageTwistCorrection = 0.0;
        double diamterRelativeToAverageCrossSectionAreaFuselage = 0.0;
        double deltaWingFuselageCRollBetaToDihedralRatio = 0.0;
        double deltaWingFuselageCRollBetaDueToWingPosition = 0.0;

        // Horizontal-Fuselage variables related
        double ratioHorizontalFuselageRollBetaLiftCoefficientDueToSweepC2 = 0.0;
        double factorHorizontalFuselageCompressibilityCorrectionToWingSweep = 0.0;
        double factorHorizontalFuselageCorrection = 0.0;
        double distanceFromeNoseToQuarterChordTipHorizontal = 0.0;
        double ratioHorizontalFuselageClBetaToCLAspectRatio = 0.0;
        double ratioHorizontalFuselageClBetaDihedralDueToSweepC2 = 0.0;
        double factorHorizontalFuselageToWingDihedral = 0.0;
        double factorHorizontalFuselageTwistCorrection = 0.0;
        double deltaHorizontalFuselageCRollBetaToDihedralRatio = 0.0;
        double deltaHorizontalFuselageCRollBetaDueToWingPosition = 0.0;

        // Vertical tail variables related
        double zCoordinatesOfVerticalTailAerodynamicCenter = 0.0;
        double tailArmVerticalTail = 0.0;

        // deltaCldeltaBeta due to ailerons deflection variables related
        double etaInner = 0.0;
        double etaOuter = 0.0;

        double numeratorAverageChordRatio = 0.0;
        double weightsOfWeightedAverageChordRatio = 0.0;
        double averageChordRatio = 0.0;
        double factorKbInner = 0.0;
        double factorKbOuter = 0.0;
        double thetaFactor = 0.0;
        double twoDimensionalFlapEffectivness = 0.0; // alphaCl - DATCOM -Roskam method
        double tauFlap = 0.0;
        std::vector<double> flapSpanFactorKb;
        std::vector<double> ratioEffectivnessFactorKc;

        // Chart - Taper Ratio lambda = 1.0
        // X axis: Sweep at half-chord Lambda_c/2 [deg]
        // Y axis: (Cl_beta / CL)_Lambda_c/2 [per deg]
        // Parameter: Aspect Ratio A = 1, 2, 4, 6, 8
        // Source: Plot Digitizer 2.6.8

        // A = 1
        std::vector<double> x_lambda1_A1 = {
            -19.8331, -15.0708, -8.47677, -0.931558, 5.66082,
            9.90917, 14.8179, 20.3132, 30.0581, 35.4805,
            39.8774, 49.9913, 59.8875};

        std::vector<double> y_lambda1_A1 = {
            0.000993957, 0.000731142, 0.000366107, 1.66e-05, -0.000259685,
            -0.000434101, -0.000696807, -0.00101827, -0.00158812, -0.00192443,
            -0.00221711, -0.00294945, -0.00378553};

        // A = 2
        std::vector<double> x_lambda1_A2 = {
            10.0582, 15.4811, 20.1716, 27.7214, 30.0667,
            39.9653, 47.8859, 53.0975, 55.3005, 57.6510,
            59.8556};

        std::vector<double> y_lambda1_A2 = {
            -0.000581956, -0.000947857, -0.00128471, -0.00188578, -0.0020616,
            -0.00303085, -0.00389799, -0.00472274, -0.00512062, -0.00557757,
            -0.00606423};

        // A = 4
        std::vector<double> x_lambda1_A4 = {
            20.1032, 24.7945, 29.7809, 33.3003, 37.0403,
            40.5613, 44.7440, 47.3864, 50.2500, 53.3348,
            55.7603, 57.5265, 59.8829};

        std::vector<double> y_lambda1_A4 = {
            -0.0015511, -0.00193234, -0.00244653, -0.00278424, -0.00318098,
            -0.00360748, -0.00419624, -0.00460859, -0.00510956, -0.00569914,
            -0.00625961, -0.00679098, -0.00757345};

        // A = 6
        std::vector<double> x_lambda1_A6 = {
            30.1522, 33.5995, 37.8545, 41.3766, 45.1209,
            48.9403, 51.0717, 53.0575, 55.3380, 57.3262,
            59.9791};

        std::vector<double> y_lambda1_A6 = {
            -0.00274218, -0.00313914, -0.00368346, -0.00416914, -0.00480262,
            -0.00553963, -0.00602633, -0.00655754, -0.00719211, -0.00785648,
            -0.00884589};

        // A = 8
        std::vector<double> x_lambda1_A8 = {
            -19.9942, -16.4019, -12.1492, -6.06465, 0.31351,
            7.86408, 15.4889, 20.3282, 25.6082, 30.1557,
            35.5848, 39.9141, 42.5579, 45.8617, 48.8741,
            50.9339, 53.2154, 55.2790, 56.9731, 58.3037,
            59.7067};

        std::vector<double> y_lambda1_A8 = {
            0.00180765, 0.00148478, 0.00107362, 0.00054545, -2.69e-05,
            -0.000672358, -0.00137695, -0.00184686, -0.00240522, -0.00293453,
            -0.00364075, -0.00424421, -0.00473054, -0.00529036, -0.00592438,
            -0.00649993, -0.00719368, -0.00797637, -0.00856697, -0.00929102,
            -0.00997062};

        // Chart - Taper Ratio lambda = 0.5
        // X axis: Sweep at half-chord Lambda_c/2 [deg]
        // Y axis: (Cl_beta / CL)_Lambda_c/2 [per deg]
        // Parameter: Aspect Ratio A = 1, 2, 4, 6, 8
        // Source: Plot Digitizer 2.6.8

        // A = 1
        std::vector<double> x_lambda05_A1 = {
            -19.2529, -9.56041, 0.348910, 10.6227, 13.8293,
            20.2425, 25.1251, 30.0811, 36.2044, 40.2133,
            46.9931, 50.1276, 56.1075, 59.8999};

        std::vector<double> y_lambda05_A1 = {
            0.00114099, 0.000525965, -1.34797e-08, -0.000555351, -0.000760378,
            -0.00117043, -0.00147792, -0.00181498, -0.00229932, -0.00259264,
            -0.00315056, -0.00340009, -0.00400305, -0.00440023};

        // A = 2
        std::vector<double> x_lambda05_A2 = {
            20.3171, 28.3367, 30.3050, 34.9712, 40.5863,
            45.4004, 50.1435, 53.6479, 56.7896, 60.2980};

        std::vector<double> y_lambda05_A2 = {
            -0.00127408, -0.00196441, -0.00212591, -0.00253726, -0.00309605,
            -0.00364062, -0.00428894, -0.00486410, -0.00551361, -0.00631099};

        // A = 4
        std::vector<double> x_lambda05_A4 = {
            30.3831, 35.6348, 40.0852, 43.7330, 47.1633,
            50.2301, 53.3713, 55.7094, 57.4652, 60.2459};

        std::vector<double> y_lambda05_A4 = {
            -0.00242214, -0.00301083, -0.00357048, -0.00402703, -0.00452818,
            -0.00505922, -0.00567910, -0.00616625, -0.00665382, -0.00746655};

        // A = 6
        std::vector<double> x_lambda05_A6 = {
            40.3809, 43.6645, 46.7318, 50.0181, 52.7207,
            55.4995, 57.4763, 59.8194};

        std::vector<double> y_lambda05_A6 = {
            -0.00382211, -0.00426411, -0.00482478, -0.00541493, -0.00593143,
            -0.00664046, -0.00727602, -0.00804463};

        // A = 8
        std::vector<double> x_lambda05_A8 = {
            -19.4811, -13.1381, 0.0572107, 10.3368, 20.3986,
            24.4811, 27.9812, 30.5335, 34.2546, 36.8087,
            40.0196, 44.0353, 47.3936, 48.9286, 50.8283,
            52.9483, 54.7772, 56.7529, 58.0727, 59.5370,
            60.1249};

        std::vector<double> y_lambda05_A8 = {
            0.00168896, 0.00113071, 2.93993e-05, -0.000851851, -0.00176289,
            -0.00210060, -0.00243874, -0.00268869, -0.00317481, -0.00352847,
            -0.00397052, -0.00464901, -0.00519465, -0.00554906, -0.00594764,
            -0.00644976, -0.00695210, -0.00752840, -0.00806074, -0.00853372,
            -0.00884438};

        // Chart - Taper Ratio lambda = 0.0
        // X axis: Sweep at half-chord Lambda_c/2 [deg]
        // Y axis: (Cl_beta / CL)_Lambda_c/2 [per deg]
        // Parameter: Aspect Ratio A = 1, 1.5, 2, 3, 6
        // Source: Plot Digitizer 2.6.8

        // A = 1
        std::vector<double> x_lambda0_A1 = {
            -19.3443, -13.5883, -6.08379, 0.546448, 10.7468,
            20.3643, 26.3388, 30.4918, 36.3206, 40.4007,
            47.1767, 50.1639, 53.2240, 56.6485, 60.1457};

        std::vector<double> y_lambda0_A1 = {
            0.00126108, 0.000862069, 0.000374384, 4.92611e-06, -0.000660099,
            -0.00128079, -0.00167980, -0.00193103, -0.00240394, -0.00269951,
            -0.00323153, -0.00351232, -0.00374877, -0.00405911, -0.00436946};

        // A = 1.5
        std::vector<double> x_lambda0_A1_5 = {
            45.5738, 50.2368, 53.5155, 56.7213, 58.8342, 60.1457};

        std::vector<double> y_lambda0_A1_5 = {
            -0.00329064, -0.00382266, -0.00426601, -0.00475369, -0.00509360, -0.00534483};

        // A = 2
        std::vector<double> x_lambda0_A2 = {
            30.4918, 35.6648, 40.4007, 44.8452, 48.4153,
            50.8197, 53.3698, 55.9199, 57.8871, 60.0729};

        std::vector<double> y_lambda0_A2 = {
            -0.00219704, -0.00262562, -0.00303941, -0.00346798, -0.00383744,
            -0.00419212, -0.00457635, -0.00504926, -0.00546305, -0.00593596};

        // A = 3
        std::vector<double> x_lambda0_A3 = {
            40.6193, 43.6794, 46.9581, 49.5082, 51.6211,
            54.0255, 56.1384, 57.8871, 60.0729};

        std::vector<double> y_lambda0_A3 = {
            -0.00326108, -0.00367488, -0.00413300, -0.00447291, -0.00485714,
            -0.00531527, -0.00577340, -0.00621675, -0.00674877};

        // A = 6
        std::vector<double> x_lambda0_A6 = {
            -19.3443, -12.3497, 0.546448, 7.46812, 10.4554,
            20.2914, 25.0273, 28.3789, 31.3661, 36.1749,
            40.1093, 44.1894, 47.3953, 49.9454, 52.2040,
            53.8798, 55.3370, 57.0856, 58.6157, 60.0000};

        std::vector<double> y_lambda0_A6 = {
            0.00158621, 0.000980296, -5.41872e-05, -0.000571429, -0.000793103,
            -0.00156158, -0.00194581, -0.00225616, -0.00253695, -0.00300985,
            -0.00343842, -0.00394089, -0.00442857, -0.00482759, -0.00521182,
            -0.00555172, -0.00595074, -0.00637931, -0.00683744, -0.00731034};

        // K factor vs Mach number
        // X axis: Mach number [-]
        // Y axis: K factor [-]
        // Parameter: AspectRatio/ cos(sweepC2) = 2, 3, 4, 5, 6, 8, 10
        // Source: Plot Digitizer 2.6.8

        // lambda ratio = 2
        std::vector<double> x_K_Mach_lambda2 = {
            0.803256, 0.848372, 0.877194, 0.903097, 0.924824, 0.951568};

        std::vector<double> y_K_Mach_lambda2 = {
            1.00075, 0.999623, 0.999694, 0.997684, 0.995248, 0.991166};

        // lambda ratio = 3
        std::vector<double> x_K_Mach_lambda3 = {
            0.803584, 0.836153, 0.868732, 0.900486, 0.928057, 0.951457};

        std::vector<double> y_K_Mach_lambda3 = {
            1.03644, 1.04150, 1.04241, 1.03917, 1.03841, 1.03515};

        // lambda ratio = 4
        std::vector<double> x_K_Mach_lambda4 = {
            0.400951, 0.454399, 0.505336, 0.561278, 0.633925, 0.704069,
            0.801762, 0.866066, 0.903654, 0.947931};

        std::vector<double> y_K_Mach_lambda4 = {
            1.02050, 1.02810, 1.03736, 1.04995, 1.06340, 1.07603,
            1.09619, 1.10548, 1.10806, 1.10817};

        // lambda ratio = 5
        std::vector<double> x_K_Mach_lambda5 = {
            0.799918, 0.829120, 0.869178, 0.902578, 0.930967, 0.950182};

        std::vector<double> y_K_Mach_lambda5 = {
            1.16506, 1.18008, 1.19677, 1.20349, 1.20937, 1.20942};

        // lambda ratio = 6
        std::vector<double> x_K_Mach_lambda6 = {
            0.399219, 0.437630, 0.478529, 0.521927, 0.567821, 0.613701,
            0.669595, 0.711303, 0.758840, 0.799698, 0.846393, 0.892247,
            0.925601, 0.953113};

        std::vector<double> y_K_Mach_lambda6 = {
            1.04456, 1.05213, 1.06634, 1.08387, 1.10556, 1.13224,
            1.16391, 1.18891, 1.22139, 1.25220, 1.28717, 1.32463,
            1.34961, 1.37209};

        // lambda ratio = 8
        std::vector<double> x_K_Mach_lambda8 = {
            0.400821, 0.440871, 0.495939, 0.543492, 0.588532, 0.626063,
            0.666063, 0.703565, 0.737309, 0.773524, 0.799729, 0.827198,
            0.859628, 0.885802, 0.914489, 0.930660, 0.946838};

        std::vector<double> y_K_Mach_lambda8 = {
            1.07195, 1.09197, 1.11949, 1.14575, 1.17450, 1.19949,
            1.23942, 1.27561, 1.31180, 1.36168, 1.40532, 1.44522,
            1.50505, 1.56113, 1.61473, 1.66208, 1.70693};

        // lambda ratio = 10
        std::vector<double> x_K_Mach_lambda10 = {
            0.0977469, 0.164119, 0.221721, 0.294352, 0.350690, 0.398256,
            0.457487, 0.500862, 0.555069, 0.598409, 0.645060, 0.673370,
            0.704988, 0.735767, 0.764883, 0.791465, 0.813892, 0.836308,
            0.855373, 0.873606, 0.889328, 0.905890, 0.918262, 0.928974,
            0.935555, 0.941287};

        std::vector<double> y_K_Mach_lambda10 = {
            0.998999, 1.01659, 1.03292, 1.05302, 1.07432, 1.09518,
            1.12853, 1.15519, 1.19267, 1.23344, 1.28584, 1.32326,
            1.37396, 1.42632, 1.47535, 1.53517, 1.58668, 1.64234,
            1.70131, 1.75945, 1.81924, 1.87737, 1.94047, 1.99942,
            2.04010, 2.08576};

        // Kf factor
        // X axis: ratio [-]
        // Y axis: Kf [-]
        // Parameter: AspectRatio/cos(sweepC2) = 4, 4.5, 5, 5.5, 6, 7, 8
        // Source: Plot Digitizer 2.6.8

        // ratio = 4
        std::vector<double> x_Kf_ratio4 = {
            1.29769, 1.36656, 1.42117, 1.48290, 1.54345, 1.60280};

        std::vector<double> y_Kf_ratio4 = {
            0.996912, 0.992673, 0.987273, 0.981272, 0.975272, 0.968683};

        // ratio = 4.5
        std::vector<double> x_Kf_ratio4_5 = {
            1.03637, 1.08978, 1.15745, 1.20612, 1.27020, 1.33785,
            1.40786, 1.48261, 1.53837, 1.60124};

        std::vector<double> y_Kf_ratio4_5 = {
            0.996116, 0.990127, 0.982934, 0.977543, 0.967401, 0.958436,
            0.946512, 0.931625, 0.921495, 0.907218};

        // ratio = 5
        std::vector<double> x_Kf_ratio5 = {
            0.807117, 0.875968, 0.940053, 1.00295, 1.08245, 1.14651,
            1.22600, 1.28294, 1.34699, 1.40392, 1.46557, 1.51776,
            1.55807, 1.60194};

        std::vector<double> y_Kf_ratio5 = {
            0.995864, 0.988670, 0.979118, 0.969569, 0.955858, 0.942169,
            0.924912, 0.912416, 0.898137, 0.883277, 0.864273, 0.850602,
            0.838722, 0.825655};

        // ratio = 5.5
        std::vector<double> x_Kf_ratio5_5 = {
            0.554123, 0.594484, 0.644326, 0.701303, 0.757080, 0.805731,
            0.857935, 0.916064, 0.970629, 1.01453, 1.05840, 1.11414,
            1.16513, 1.20664, 1.26356, 1.32048, 1.37620, 1.43666,
            1.50069, 1.56115, 1.59789};

        std::vector<double> y_Kf_ratio5_5 = {
            0.998011, 0.993814, 0.986057, 0.979471, 0.971114, 0.962768,
            0.952643, 0.940146, 0.927654, 0.918723, 0.906247, 0.891980,
            0.877129, 0.865838, 0.850978, 0.833754, 0.815941, 0.796938,
            0.777340, 0.757747, 0.744099};

        // ratio = 6
        std::vector<double> x_Kf_ratio6 = {
            0.387829, 0.412752, 0.455488, 0.504150, 0.558747, 0.607409,
            0.651308, 0.709448, 0.773519, 0.823333, 0.902818, 0.953824,
            1.00719, 1.05343, 1.10560, 1.15659, 1.21469, 1.28109,
            1.34985, 1.42453, 1.47432, 1.53834, 1.59405};

        std::vector<double> y_Kf_ratio6 = {
            0.998256, 0.994673, 0.990473, 0.983900, 0.976727, 0.970153,
            0.961814, 0.951090, 0.939174, 0.926689, 0.910023, 0.898127,
            0.883272, 0.869610, 0.854167, 0.838724, 0.821499, 0.802488,
            0.779336, 0.753811, 0.737780, 0.716999, 0.696822};

        // ratio = 7
        std::vector<double> x_Kf_ratio7 = {
            0.232229, 0.269019, 0.311756, 0.355677, 0.406714, 0.451812,
            0.517068, 0.563347, 0.614363, 0.678427, 0.734184, 0.788739,
            0.848038, 0.921566, 1.00220, 1.05199, 1.12788, 1.19426,
            1.25115, 1.32822, 1.40288, 1.47756, 1.54039, 1.59609};

        std::vector<double> y_Kf_ratio7 = {
            0.999077, 0.993704, 0.989503, 0.984710, 0.978133, 0.971565,
            0.959057, 0.951306, 0.941183, 0.928085, 0.916182, 0.901917,
            0.886463, 0.866850, 0.843090, 0.827649, 0.804487, 0.781930,
            0.761751, 0.737405, 0.709516, 0.683991, 0.662030, 0.641262};

        // ratio = 8
        std::vector<double> x_Kf_ratio8 = {
            0.00891963, 0.0492983, 0.108668, 0.163283, 0.211951, 0.289106,
            0.362698, 0.419649, 0.482537, 0.543045, 0.607105, 0.659306,
            0.726912, 0.773152, 0.834820, 0.890549, 0.947468, 1.00556,
            1.04824, 1.10987, 1.16913, 1.25923, 1.32086, 1.37775,
            1.44412, 1.50457, 1.56145, 1.59818};

        std::vector<double> y_Kf_ratio8 = {
            0.999407, 0.998165, 0.994532, 0.990314, 0.984923, 0.975943,
            0.966969, 0.956246, 0.944924, 0.933014, 0.919325, 0.908610,
            0.891961, 0.877708, 0.861068, 0.844437, 0.828395, 0.809396,
            0.796330, 0.773780, 0.751233, 0.721549, 0.698999, 0.677046,
            0.652716, 0.630759, 0.609398, 0.593385};

        // (Cl_beta / CL) ratio vs Aspect Ratio
        // X axis: Aspect Ratio A [-]
        // Y axis: (Cl_beta / CL) [per deg]
        // Parameter: Taper Ratio lambda = 0, 0.5, 1.0
        // Source: Plot Digitizer 2.6.8

        // lambda = 0
        std::vector<double> x_ClbetaCL_lambda0 = {
            0.991005, 1.18139, 1.32407, 1.52342, 1.71463,
            2.01885, 2.30712, 2.65202, 3.04511, 3.44673,
            3.84859, 4.26670, 4.70900, 5.22364, 5.74637,
            6.30934, 6.84031, 7.34709, 7.99874};

        std::vector<double> y_ClbetaCL_lambda0 = {
            -0.00558507, -0.00468163, -0.00396848, -0.00336532, -0.00273065,
            -0.00220530, -0.00172758, -0.00134404, -0.000928316, -0.000670590,
            -0.000491912, -0.000360473, -0.000244556, -9.61621e-05, 3.65181e-05,
            0.000169675, 0.000239212, 0.000324273, 0.000411051};

        // lambda = 0.5
        std::vector<double> x_ClbetaCL_lambda05 = {
            0.974050, 1.13278, 1.27497, 1.50583, 1.74460,
            2.05604, 2.29569, 2.62415, 2.96061, 3.45023,
            3.96413, 4.58295, 5.25816, 5.98975, 6.68939,
            7.36499, 7.97639};

        std::vector<double> y_ClbetaCL_lambda05 = {
            -0.00792518, -0.00719603, -0.00632478, -0.00549991, -0.00462752,
            -0.00383331, -0.00324549, -0.00275148, -0.00224157, -0.00180888,
            -0.00142335, -0.00113142, -0.000854643, -0.000593005, -0.000410796,
            -0.000260493, -0.000174192};

        // lambda = 1.0
        std::vector<double> x_ClbetaCL_lambda1 = {
            1.00054, 1.19756, 1.37902, 1.51312, 1.71945,
            1.92618, 2.18133, 2.46092, 2.71676, 3.00488,
            3.42987, 3.87110, 4.24039, 4.73044, 5.14826,
            5.63041, 6.23338, 6.80425, 7.18216, 7.51999,
            7.95448};

        std::vector<double> y_ClbetaCL_lambda1 = {
            -0.0113083, -0.00994624, -0.00875832, -0.00787136, -0.00692030,
            -0.00609572, -0.00531799, -0.00463484, -0.00407844, -0.00355329,
            -0.00304232, -0.00257859, -0.00227382, -0.00198343, -0.00175713,
            -0.00151426, -0.00130158, -0.00112089, -0.00100574, -0.000938495,
            -0.000901723};

        // (Cl_beta / Gamma) ratio vs Aspect Ratio
        // X axis: Aspect Ratio A [-]
        // Y axis: (Cl_beta / Gamma) [per deg^2]
        // Parameter: Taper Ratio lambda = 1.0, Sweep c/2 = 0, 40, 60 [deg]
        // Source: Plot Digitizer 2.6.8

        // lambda = 1.0, sweepC2 = 0 deg
        std::vector<double> x_ClbetaGamma_lambda1_sweep0 = {
            0.00794907, 0.278637, 0.597095, 0.979273, 1.37737,
            1.71178, 2.02232, 2.50011, 2.96199, 3.37610,
            3.90972, 4.40351, 4.86549, 5.35135, 5.81335,
            6.33907, 6.80906, 7.31091, 7.74106, 7.98003};

        std::vector<double> y_ClbetaGamma_lambda1_sweep0 = {
            -1.20e-06, -1.56e-05, -3.24e-05, -5.08e-05, -7.04e-05,
            -8.60e-05, -9.99e-05, -0.000118342, -0.000135538, -0.000149935,
            -0.000165536, -0.000179936, -0.000191140, -0.000203543, -0.000213148,
            -0.000223955, -0.000232363, -0.000241171, -0.000249178, -0.000253581};

        // lambda = 1.0, sweepC2 = 40 deg
        std::vector<double> x_ClbetaGamma_lambda1_sweep40 = {
            2.98598, 3.24880, 3.50366, 3.88599, 4.24440,
            4.73028, 5.16042, 5.67820, 6.18006, 6.65804,
            7.07229, 7.47857, 7.98842};

        std::vector<double> y_ClbetaGamma_lambda1_sweep40 = {
            -0.000130346, -0.000137946, -0.000145546, -0.000154749, -0.000164351,
            -0.000175156, -0.000183961, -0.000193170, -0.000201978, -0.000208389,
            -0.000214397, -0.000220405, -0.000227217};

        // lambda = 1.0, sweepC2 = 60 deg
        std::vector<double> x_ClbetaGamma_lambda1_sweep60 = {
            0.0318471, 0.238834, 0.509534, 0.875763, 1.09872,
            1.48892, 1.99064, 2.40478, 2.86675, 3.33670,
            3.83058, 4.36432, 4.80247, 5.24062, 5.71065,
            6.10899, 6.57109, 7.00132, 7.40765, 7.71837,
            7.96536};

        std::vector<double> y_ClbetaGamma_lambda1_sweep60 = {
            -1.59914e-06, -1.31920e-05, -2.67847e-05, -4.59738e-05, -5.51704e-05,
            -7.07653e-05, -8.75629e-05, -0.000100362, -0.000111965, -0.000122370,
            -0.000131178, -0.000139189, -0.000145199, -0.000151208, -0.000156819,
            -0.000161229, -0.000164444, -0.000167657, -0.000170469, -0.000172878,
            -0.000174087};

        // (Cl_beta / Gamma) ratio vs Aspect Ratio
        // X axis: Aspect Ratio A [-]
        // Y axis: (Cl_beta / Gamma) [per deg^2]
        // Parameter: Taper Ratio lambda = 0.5, Sweep c/2 = 0, 40, 60 [deg]
        // Source: Plot Digitizer 2.6.8

        // lambda = 0.5, sweepC2 = 0 deg
        std::vector<double> x_ClbetaGamma_lambda05_sweep0 = {
            0.0396564, 0.285582, 0.571265, 0.920489, 1.20625,
            1.61115, 1.98424, 2.24629, 2.54021, 2.90559,
            3.31876, 3.77975, 4.21691, 4.63029, 4.99604,
            5.49695, 5.99788, 6.49094, 6.98406, 7.47718,
            7.97821};

        std::vector<double> y_ClbetaGamma_lambda05_sweep0 = {
            -2.38093e-06, -1.62720e-05, -3.09609e-05, -4.80346e-05, -6.15362e-05,
            -7.94102e-05, -9.68834e-05, -0.000107611, -0.000117948, -0.000131462,
            -0.000144588, -0.000156930, -0.000168476, -0.000178436, -0.000186014,
            -0.000196779, -0.000207149, -0.000215934, -0.000223927, -0.000231921,
            -0.000240707};

        // lambda = 0.5, sweepC2 = 40 deg
        std::vector<double> x_ClbetaGamma_lambda05_sweep40 = {
            2.00838, 2.30230, 2.65987, 2.97770, 3.35124,
            3.74874, 4.14622, 4.60742, 4.99701, 5.54578,
            6.01499, 6.54001, 6.99342, 7.49450, 7.99570};

        std::vector<double> y_ClbetaGamma_lambda05_sweep40 = {
            -9.29295e-05, -0.000103266, -0.000114404, -0.000124349, -0.000134699,
            -0.000143864, -0.000153426, -0.000162602, -0.000170975, -0.000180164,
            -0.000188550, -0.000195757, -0.000202161, -0.000210156, -0.000216172};

        // lambda = 0.5, sweepC2 = 60 deg
        std::vector<double> x_ClbetaGamma_lambda05_sweep60 = {
            0.0476434, 0.333301, 0.634933, 0.944501, 1.25420,
            1.65139, 1.97714, 2.44604, 2.96272, 3.40794,
            3.92489, 4.36235, 4.78397, 5.30110, 5.91377,
            6.39119, 6.90853, 7.37806, 7.70437, 7.98298};

        std::vector<double> y_ClbetaGamma_lambda05_sweep60 = {
            -1.98642e-06, -1.70711e-05, -3.13668e-05, -4.60595e-05, -5.87734e-05,
            -7.26884e-05, -8.34259e-05, -9.65607e-05, -0.000109703, -0.000119668,
            -0.000128457, -0.000135254, -0.000140862, -0.000146880, -0.000152914,
            -0.000157343, -0.000160196, -0.000163436, -0.000165467, -0.000166302};

        // (Cl_beta / Gamma) ratio vs Aspect Ratio
        // X axis: Aspect Ratio A [-]
        // Y axis: (Cl_beta / Gamma) [per deg^2]
        // Parameter: Taper Ratio lambda = 0.0, Sweep c/2 = 0, 40, 60 [deg]
        // Source: Plot Digitizer 2.6.8

        // lambda = 0.0, sweepC2 = 0 deg
        std::vector<double> x_ClbetaGamma_lambda0_sweep0 = {
            0.0316943, 0.189823, 0.467072, 0.728550, 1.02187,
            1.41055, 1.85499, 2.28366, 2.78395, 3.36381,
            3.89633, 4.30950, 4.69901, 5.02498, 5.42245,
            5.84392, 6.27331, 6.61528, 7.00498, 7.40260,
            7.74461, 7.96734};

        std::vector<double> y_ClbetaGamma_lambda0_sweep0 = {
            -1.57815e-06, -1.30029e-05, -2.75971e-05, -4.06168e-05, -5.36441e-05,
            -6.86572e-05, -8.32908e-05, -9.63499e-05, -0.000109819, -0.000123699,
            -0.000133248, -0.000141984, -0.000148359, -0.000153148, -0.000159524,
            -0.000164728, -0.000170327, -0.000174334, -0.000178745, -0.000183551,
            -0.000187166, -0.000189182};

        // lambda = 0.0, sweepC2 = 40 deg
        std::vector<double> x_ClbetaGamma_lambda0_sweep40 = {
            2.97510, 3.43604, 3.91290, 4.48534, 4.98623,
            5.55874, 5.99618, 6.45757, 6.99843, 7.49959,
            7.98487};

        std::vector<double> y_ClbetaGamma_lambda0_sweep40 = {
            -0.000109078, -0.000117826, -0.000126577, -0.000134958, -0.000142144,
            -0.000149740, -0.000154555, -0.000158590, -0.000164215, -0.000168652,
            -0.000172693};

        // lambda = 0.0, sweepC2 = 60 deg
        std::vector<double> x_ClbetaGamma_lambda0_sweep60 = {
            0.0236946, 0.309095, 0.491375, 0.673808, 0.951475,
            1.21329, 1.62608, 2.12633, 2.64285, 3.15944,
            3.66826, 4.20908, 4.93293, 5.56141, 6.27746,
            7.00954, 7.51090, 7.96456};

        std::vector<double> y_ClbetaGamma_lambda0_sweep60 = {
            -1.96894e-06, -1.46017e-05, -2.32834e-05, -3.03945e-05, -4.06694e-05,
            -5.01552e-05, -6.28179e-05, -7.66793e-05, -8.70104e-05, -9.65563e-05,
            -0.000104137, -0.000110154, -0.000117000, -0.000122253, -0.000127527,
            -0.000131626, -0.000134100, -0.000135778};

        // Kmd factor vs Mach number
        // X axis: Mach * cos(sweepC2) [-]
        // Y axis: Kmd factor [-]
        // Parameter: A / tan(sweepC2) ratio = 2, 4, 6, 8, 10
        // Source: Plot Digitizer 2.6.8

        // A / tan(sweepC2) = 2
        std::vector<double> x_Kmd_ratio2 = {
            0.402359, 0.442914, 0.485606, 0.540037, 0.600873,
            0.697733, 0.799398, 0.898664, 0.951501};

        std::vector<double> y_Kmd_ratio2 = {
            1.01018, 1.01014, 1.01248, 1.01481, 1.01793,
            1.02578, 1.03602, 1.04785, 1.05655};

        // A / tan(sweepC2) = 4
        std::vector<double> x_Kmd_ratio4 = {
            0.399975, 0.440802, 0.496842, 0.552882, 0.599318,
            0.671373, 0.700197, 0.745836, 0.799484, 0.857138,
            0.901183, 0.950837};

        std::vector<double> y_Kmd_ratio4 = {
            1.02610, 1.03163, 1.04032, 1.04981, 1.06090,
            1.07595, 1.08387, 1.09735, 1.11560, 1.13702,
            1.15766, 1.18308};

        // A / tan(sweepC2) = 6
        std::vector<double> x_Kmd_ratio6 = {
            0.600959, 0.639397, 0.675431, 0.706662, 0.752312,
            0.790755, 0.833213, 0.876472, 0.906117, 0.927754,
            0.947789};

        std::vector<double> y_Kmd_ratio6 = {
            1.09830, 1.11418, 1.12766, 1.14116, 1.16498,
            1.18643, 1.21901, 1.25318, 1.28020, 1.30405,
            1.32631};

        // A / tan(sweepC2) = 8
        std::vector<double> x_Kmd_ratio8 = {
            0.798837, 0.818871, 0.838909, 0.857343, 0.876583,
            0.895020, 0.913465, 0.930307, 0.943944, 0.947959};

        std::vector<double> y_Kmd_ratio8 = {
            1.25883, 1.28030, 1.30574, 1.32880, 1.35663,
            1.38128, 1.41388, 1.44490, 1.47274, 1.48467};

        // A / tan(sweepC2) = 10
        std::vector<double> x_Kmd_ratio10 = {
            0.00934030, 0.0445573, 0.0973889, 0.155026, 0.199854,
            0.246287, 0.296725, 0.349565, 0.397604, 0.441643,
            0.482483, 0.501702, 0.542543, 0.572976, 0.602611,
            0.633046, 0.666692, 0.698736, 0.731584, 0.760435,
            0.790090, 0.819746, 0.842999, 0.863849, 0.892725,
            0.919207, 0.941681, 0.952121};

        std::vector<double> y_Kmd_ratio10 = {
            1.00185, 1.00022, 1.00334, 1.00885, 1.01358,
            1.02069, 1.03098, 1.04207, 1.05475, 1.06903,
            1.08569, 1.09443, 1.11189, 1.12777, 1.14525,
            1.16352, 1.18895, 1.21438, 1.24299, 1.27638,
            1.31216, 1.35032, 1.38770, 1.42428, 1.48075,
            1.54358, 1.60165, 1.63347};

        // Twist correction factor vs Aspect Ratio
        // X axis: Aspect Ratio A [-]
        // Y axis: Twist correction factor [per deg^2]
        // Parameter: Taper Ratio lambda = 0, 0.4, 1.0
        // Source: Plot Digitizer 2.6.8

        // lambda = 0
        std::vector<double> x_TwistCorr_lambda0 = {
            3.00369, 3.38835, 3.84773, 4.27503, 4.74502,
            5.00135, 5.34307, 5.71678, 6.04778, 6.57102,
            7.15839, 7.86329, 8.45079, 8.94215, 9.41219,
            9.78609, 10.0853, 10.4592, 10.7691, 11.0898,
            11.3036, 11.4748};

        std::vector<double> y_TwistCorr_lambda0 = {
            -1.90e-05, -2.05e-05, -2.17e-05, -2.26e-05, -2.34e-05,
            -2.36e-05, -2.36e-05, -2.33e-05, -2.30e-05, -2.28e-05,
            -2.30e-05, -2.36e-05, -2.47e-05, -2.56e-05, -2.67e-05,
            -2.76e-05, -2.86e-05, -2.99e-05, -3.13e-05, -3.31e-05,
            -3.49e-05, -3.72e-05};

        // lambda = 0.4
        std::vector<double> x_TwistCorr_lambda04 = {
            2.99348, 3.22871, 3.48528, 3.76320, 4.00903,
            4.35097, 4.63945, 4.98131, 5.30175, 5.62219,
            5.99601, 6.39117, 6.81833, 7.26684, 7.72607,
            8.01441, 8.34549, 8.66589, 8.97562, 9.39220,
            9.76607, 10.0438, 10.2682, 10.5781, 10.7491,
            10.8774, 10.9843};

        std::vector<double> y_TwistCorr_lambda04 = {
            -2.21e-05, -2.40e-05, -2.58e-05, -2.76e-05, -2.89e-05,
            -3.03e-05, -3.13e-05, -3.22e-05, -3.27e-05, -3.32e-05,
            -3.35e-05, -3.38e-05, -3.38e-05, -3.38e-05, -3.40e-05,
            -3.41e-05, -3.44e-05, -3.46e-05, -3.49e-05, -3.55e-05,
            -3.63e-05, -3.70e-05, -3.76e-05, -3.88e-05, -3.99e-05,
            -4.08e-05, -4.15e-05};

        // lambda = 1.0
        std::vector<double> x_TwistCorr_lambda1 = {
            2.98299, 3.16474, 3.37858, 3.56031, 3.77409,
            3.99850, 4.29773, 4.59691, 4.88537, 5.16311,
            5.49422, 5.87874, 6.10304, 6.32736, 6.51965,
            6.69057, 7.03232, 7.36337, 7.91868, 8.43131,
            8.73034, 8.97599, 9.22167, 9.58486, 9.83059,
            10.0550, 10.2794, 10.4505};

        std::vector<double> y_TwistCorr_lambda1 = {
            -2.33e-05, -2.46e-05, -2.63e-05, -2.76e-05, -2.89e-05,
            -2.99e-05, -3.14e-05, -3.24e-05, -3.33e-05, -3.39e-05,
            -3.43e-05, -3.48e-05, -3.51e-05, -3.55e-05, -3.60e-05,
            -3.63e-05, -3.65e-05, -3.66e-05, -3.66e-05, -3.69e-05,
            -3.71e-05, -3.73e-05, -3.77e-05, -3.84e-05, -3.92e-05,
            -4.02e-05, -4.12e-05, -4.23e-05};

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

    public:
        /**
         * @brief Constructor for DirectionalStabilityCalculator
         * @param builder Reference to aircraft builder containing all aircraft data
         * @param cogData Center of gravity data
         * @param settings Aerodynamic analysis settings
         * @param wing Main wing geometry
         * @param horizontalTail Horizontal tail geometry
         * @param fuselage Fuselage geometry
         * @param nacelle Nacelle geometry
         * @param verticalTail Vertical tail geometry (optional)
         * @param canard Canard geometry (optional)
         */
        LateralStabilityCalculator(
            BuildAircraft &builder,
            VSP::Aircraft aircraftInfo,
            COG::COGDATA cogData,
            VSP::AeroSettings settings,
            VSP::Wing wing,
            VSP::Wing horizontalTail,
            VSP::Wing verticalTail,
            VSP::Fuselage fuselage,
            VSP::Nacelle nacelle,
            DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesSideForceToSingleComponent directionalDerivatives,
            VSP::Wing canard = VSP::Wing(),
            VSP::Disk disk = VSP::Disk()) : builder(builder),
                                            aircraftInfo(aircraftInfo),
                                            nameOfAircraft(nameOfAircraft),
                                            cogData(cogData),
                                            settings(settings),
                                            wing(wing),
                                            horizontalTail(horizontalTail),
                                            verticalTail(verticalTail),
                                            fuselage(fuselage),
                                            nacelle(nacelle),
                                            singleComponentsDerivativesSideForce(directionalDerivatives),
                                            canard(canard),
                                            disk(disk)
        {
        }

        /**
         * @brief Destructor
         */
        ~LateralStabilityCalculator() = default;

        /**
         * @brief Main method to calculate all lateral stability derivatives
         *
         * This method computes contributions from:
         * - Wing (swept wing effects and position)
         * - Canard (if present)
         * - Horizontal tail
         * - Fuselage (using Perkins empirical method)
         * - Nacelles (if present)
         * - Propellers (if activated in settings)
         *
         * Results are stored in class member variables
         */
        void calculateLateralStabilityDerivatives()
        {


            RestoreSettings restoreSettings;

            restoreSettings.getSavePrevoiusSettings(settings);
            restoreSettings.getSavePrevoiusAircraftInfo(aircraftInfo);

            if (settings.AoA.size() > 1)
            {
                std::cout << "Warning: Multiple angles of attack provided. Lateral stability derivatives will be calculated for the first AoA only." << std::endl;

                settings.AoA = {0.0};
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

            for (size_t n = 0; n < canard.mov.defl.size(); n++)
            {

                if (canard.mov.defl[n] != 0.0)
                {
                    std::cout << "Warning: Canard control surface deflection detected. Lateral stability derivatives will be calculated for the undeflected configuration only." << std::endl;

                    canard.mov.defl[n] = 0.0;

                    aircraftInfo.canard.mov.defl[n] = 0.0;
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

            for (size_t n = 0; n < verticalTail.mov.defl.size(); n++)
            {

                if (verticalTail.mov.defl[n] != 0.0)
                {
                    std::cout << "Warning: Vertical Tail control surface deflection detected. Lateral stability derivatives will be calculated for the undeflected configuration only." << std::endl;

                    verticalTail.mov.defl[n] = 0.0;

                    aircraftInfo.ver.mov.defl[n] = 0.0;
                }
            }

            // ========================================================================
            // STEP 1: Calculate Wing-Fuselage and Horizontal_Fuselage Contribution to Lateral Stability Derivative
            // ========================================================================
            // Swept wings contribute to lateral stability due to asymmetric lift
            // distribution in sideslip. Contribution is proportional to sweep angle.

            // Extract geometric data from VSP model
            VSPGEOMTRYEXTRACTOR::GeometryExtractor geomExtractor;
            geomExtractor.extractAllGeoms(builder.getCommonData().getNameOfAircraft(),
                                          builder.getCommonData().getNameOfAircraft() + "_AllGeoms.vspscript");

            VSPGEOMTRYEXTRACTOR::AircraftGeometryData allGeomData = geomExtractor.getGeometryData();

            // Extract fuselage diameter distribution along length
            VSPGEOMTRYEXTRACTOR::DiametersExtractor fuseExtractor;

            VSPGEOMTRYEXTRACTOR::FuselageDiametersAndXStation fuseData = fuseExtractor.extractFuselageDiameters(
                builder.getCommonData().getNameOfAircraft(),
                allGeomData,
                fuselage.length);

            SILENTORCOMPONENT::SilentorComponent silentor(builder.getCommonData().getNameOfAircraft(),
                                                          "Silent_components.vspscript");

            // Calculate CLwf
            silentor.GetGeometryWithThisComponent(aircraftInfo, allGeomData, {wing.id, fuselage.id});

            silentor.executeAnalysis(settings);

            std::vector<double> liftCoefficientWingFuselage = silentor.getAerodynamicCoefficients().liftCoefficient;

            // Calculate CLhf
            silentor.GetGeometryWithThisComponent(aircraftInfo, allGeomData, {horizontalTail.id, fuselage.id});

            silentor.executeAnalysis(settings);

            std::vector<double> liftCoefficientHorizontalFuselage = silentor.getAerodynamicCoefficients().liftCoefficient;

            // Calculate Cl_beta/CL|sweepC2
            Interpolant3D ratioRollBettaLiftCoefficientInterpolator(2, RegressionMethod::POLYNOMIAL);

            ratioRollBettaLiftCoefficientInterpolator.addCurve(1.0, 1.0, x_lambda1_A1, y_lambda1_A1);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(1.0, 2.0, x_lambda1_A2, y_lambda1_A2);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(1.0, 4.0, x_lambda1_A4, y_lambda1_A4);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(1.0, 6.0, x_lambda1_A6, y_lambda1_A6);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(1.0, 8.0, x_lambda1_A8, y_lambda1_A8);

            ratioRollBettaLiftCoefficientInterpolator.addCurve(0.5, 1.0, x_lambda05_A1, y_lambda05_A1);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(0.5, 2.0, x_lambda05_A2, y_lambda05_A2);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(0.5, 4.0, x_lambda05_A4, y_lambda05_A4);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(0.5, 6.0, x_lambda05_A6, y_lambda05_A6);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(0.5, 8.0, x_lambda05_A8, y_lambda05_A8);

            ratioRollBettaLiftCoefficientInterpolator.addCurve(0.0, 1.0, x_lambda0_A1, y_lambda0_A1);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(0.0, 1.5, x_lambda0_A1_5, y_lambda0_A1_5);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(0.0, 2.0, x_lambda0_A2, y_lambda0_A2);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(0.0, 3.0, x_lambda0_A3, y_lambda0_A3);
            ratioRollBettaLiftCoefficientInterpolator.addCurve(0.0, 6.0, x_lambda0_A6, y_lambda0_A6);

            ratioWingFuselageRollBetaLiftCoefficientDueToSweepC2 = ratioRollBettaLiftCoefficientInterpolator.interpolate(wing.sweepC2, wing.aspectRatio, wing.taperRatio);
            ratioHorizontalFuselageRollBetaLiftCoefficientDueToSweepC2 = ratioRollBettaLiftCoefficientInterpolator.interpolate(horizontalTail.sweepC2, horizontalTail.aspectRatio, horizontalTail.taperRatio);

            // Calculate K_{M_lambda}

            Interpolant2D KMLambdaInterpolator(3, RegressionMethod::POLYNOMIAL);

            KMLambdaInterpolator.addCurve(2.0, x_K_Mach_lambda2, y_K_Mach_lambda2);
            KMLambdaInterpolator.addCurve(3.0, x_K_Mach_lambda3, y_K_Mach_lambda3);
            KMLambdaInterpolator.addCurve(4.0, x_K_Mach_lambda4, y_K_Mach_lambda4);
            KMLambdaInterpolator.addCurve(5.0, x_K_Mach_lambda5, y_K_Mach_lambda5);
            KMLambdaInterpolator.addCurve(6.0, x_K_Mach_lambda6, y_K_Mach_lambda6);
            KMLambdaInterpolator.addCurve(8.0, x_K_Mach_lambda8, y_K_Mach_lambda8);
            KMLambdaInterpolator.addCurve(10.0, x_K_Mach_lambda10, y_K_Mach_lambda10);

            factorWingFuselageCompressibilityCorrectionToWingSweep = KMLambdaInterpolator.interpolate(settings.Mach * std::cos(wing.sweepC2 / 57.3), wing.aspectRatio / (std::cos(wing.sweepC2 / 57.3)));
            factorHorizontalFuselageCompressibilityCorrectionToWingSweep = KMLambdaInterpolator.interpolate(settings.Mach * std::cos(horizontalTail.sweepC2 / 57.3), horizontalTail.aspectRatio / (std::cos(horizontalTail.sweepC2 / 57.3)));

            // Calculate Kf

            distanceFromeNoseToQuarterChordTipWing = wing.xloc + 0.5 * wing.totalProjectedSpan * std::tan(wing.averageLeadingEdgeSweep / 57.3) + 0.5 * wing.ctip.back();
            distanceFromeNoseToQuarterChordTipHorizontal = horizontalTail.xloc + 0.5 * horizontalTail.totalProjectedSpan * std::tan(horizontalTail.averageLeadingEdgeSweep / 57.3) + 0.5 * horizontalTail.ctip.back();

            Interpolant2D KfInterpolator(3, RegressionMethod::POLYNOMIAL);

            KfInterpolator.addCurve(4.0, x_Kf_ratio4, y_Kf_ratio4);
            KfInterpolator.addCurve(4.5, x_Kf_ratio4_5, y_Kf_ratio4_5);
            KfInterpolator.addCurve(5.0, x_Kf_ratio5, y_Kf_ratio5);
            KfInterpolator.addCurve(5.5, x_Kf_ratio5_5, y_Kf_ratio5_5);
            KfInterpolator.addCurve(6.0, x_Kf_ratio6, y_Kf_ratio6);
            KfInterpolator.addCurve(7.0, x_Kf_ratio7, y_Kf_ratio7);
            KfInterpolator.addCurve(8.0, x_Kf_ratio8, y_Kf_ratio8);

            factorWingFuselageCorrection = KfInterpolator.interpolate(distanceFromeNoseToQuarterChordTipWing / wing.totalProjectedSpan, wing.aspectRatio / (std::cos(wing.sweepC2 / 57.3)));
            factorHorizontalFuselageCorrection = KfInterpolator.interpolate(distanceFromeNoseToQuarterChordTipHorizontal / horizontalTail.totalProjectedSpan, horizontalTail.aspectRatio / (std::cos(horizontalTail.sweepC2 / 57.3)));

            // Calculate ratio Cl_beta/CL due to aspect ratio

            Interpolant2D ClBetaCLAspectRatioInterpolator(5, RegressionMethod::POLYNOMIAL);

            ClBetaCLAspectRatioInterpolator.addCurve(0.0, x_ClbetaCL_lambda0, y_ClbetaCL_lambda0);
            ClBetaCLAspectRatioInterpolator.addCurve(0.5, x_ClbetaCL_lambda05, y_ClbetaCL_lambda05);
            ClBetaCLAspectRatioInterpolator.addCurve(1.0, x_ClbetaCL_lambda1, y_ClbetaCL_lambda1);

            ratioWingFuselageClBetaToCLAspectRatio = ClBetaCLAspectRatioInterpolator.interpolate(wing.aspectRatio, wing.taperRatio);
            ratioHorizontalFuselageClBetaToCLAspectRatio = ClBetaCLAspectRatioInterpolator.interpolate(horizontalTail.aspectRatio, horizontalTail.taperRatio);

            // Calculate Cl_beta/Gamma ratio due to  dihedral effect

            Interpolant3D ClBetaGammaInterpolator(3, RegressionMethod::POLYNOMIAL);

            ClBetaGammaInterpolator.addCurve(1.0, 0.0, x_ClbetaGamma_lambda1_sweep0, y_ClbetaGamma_lambda1_sweep0);
            ClBetaGammaInterpolator.addCurve(1.0, 40.0, x_ClbetaGamma_lambda1_sweep40, y_ClbetaGamma_lambda1_sweep40);
            ClBetaGammaInterpolator.addCurve(1.0, 60.0, x_ClbetaGamma_lambda1_sweep60, y_ClbetaGamma_lambda1_sweep60);

            ClBetaGammaInterpolator.addCurve(0.5, 0.0, x_ClbetaGamma_lambda05_sweep0, y_ClbetaGamma_lambda05_sweep0);
            ClBetaGammaInterpolator.addCurve(0.5, 40.0, x_ClbetaGamma_lambda05_sweep40, y_ClbetaGamma_lambda05_sweep40);
            ClBetaGammaInterpolator.addCurve(0.5, 60.0, x_ClbetaGamma_lambda05_sweep60, y_ClbetaGamma_lambda05_sweep60);

            ClBetaGammaInterpolator.addCurve(0.0, 0.0, x_ClbetaGamma_lambda0_sweep0, y_ClbetaGamma_lambda0_sweep0);
            ClBetaGammaInterpolator.addCurve(0.0, 40.0, x_ClbetaGamma_lambda0_sweep40, y_ClbetaGamma_lambda0_sweep40);
            ClBetaGammaInterpolator.addCurve(0.0, 60.0, x_ClbetaGamma_lambda0_sweep60, y_ClbetaGamma_lambda0_sweep60);

            ratioWingFuselageClBetaDihedralDueToSweepC2 = ClBetaGammaInterpolator.interpolate(wing.aspectRatio, std::abs(wing.sweepC2), wing.taperRatio);
            ratioHorizontalFuselageClBetaDihedralDueToSweepC2 = ClBetaGammaInterpolator.interpolate(horizontalTail.aspectRatio, std::abs(horizontalTail.sweepC2), horizontalTail.taperRatio);

            // Calculate factor KM due to dihedral effect

            Interpolant2D KMDihedralInterpolator(2, RegressionMethod::POLYNOMIAL);

            KMDihedralInterpolator.addCurve(2.0, x_Kmd_ratio2, y_Kmd_ratio2);
            KMDihedralInterpolator.addCurve(4.0, x_Kmd_ratio4, y_Kmd_ratio4);
            KMDihedralInterpolator.addCurve(6.0, x_Kmd_ratio6, y_Kmd_ratio6);
            KMDihedralInterpolator.addCurve(8.0, x_Kmd_ratio8, y_Kmd_ratio8);
            KMDihedralInterpolator.addCurve(10.0, x_Kmd_ratio10, y_Kmd_ratio10);

            factorWingFuselageToWingDihedral = KMDihedralInterpolator.interpolate(settings.Mach * std::cos(wing.sweepC2 / 57.3), wing.aspectRatio / (std::cos(wing.sweepC2 / 57.3)));
            factorHorizontalFuselageToWingDihedral = KMDihedralInterpolator.interpolate(settings.Mach * std::cos(horizontalTail.sweepC2 / 57.3), horizontalTail.aspectRatio / (std::cos(horizontalTail.sweepC2 / 57.3)));

            // Calculate twist correction factor

            Interpolant2D TwistCorrectionInterpolator(6, RegressionMethod::POLYNOMIAL);

            TwistCorrectionInterpolator.addCurve(0.0, x_TwistCorr_lambda0, y_TwistCorr_lambda0);
            TwistCorrectionInterpolator.addCurve(0.4, x_TwistCorr_lambda04, y_TwistCorr_lambda04);
            TwistCorrectionInterpolator.addCurve(1.0, x_TwistCorr_lambda1, y_TwistCorr_lambda1);

            factorWingFuselageTwistCorrection = TwistCorrectionInterpolator.interpolate(wing.taperRatio, wing.aspectRatio);
            factorHorizontalFuselageTwistCorrection = TwistCorrectionInterpolator.interpolate(horizontalTail.taperRatio, horizontalTail.aspectRatio);

            // Calculate the averege corss-section area
            std::map<int, double> deltaXFromXPointWhereFlowCasesToBePotentialMap;

            for (size_t j = 0; j < fuseData.xStation.size(); j++)
            {

                for (const auto &nameOfTheComponentFuselage : allGeomData.nameOfComponentGeom)
                {

                    if (nameOfTheComponentFuselage == fuselage.id)
                    {

                        for (auto &nameFuselageGeom : allGeomData.nameGeom)
                        {

                            if (nameFuselageGeom == "Custom")
                            {

                                relativeCrossSectionAreaAtXStationFuselage.push_back(M_PI * std::pow(fuseData.allFuselageWidth[j] / 2.0, 2.0));
                                realtiveWidthAtXstationFuselage.push_back(fuseData.allFuselageWidth[j]);
                                numeratorweightsAndCrossSectionArea += relativeCrossSectionAreaAtXStationFuselage.back() * realtiveWidthAtXstationFuselage.back();
                                weightsOfWeightedAverageCrossSectionArea += realtiveWidthAtXstationFuselage.back();

                                break;
                            }
                            else if ((nameFuselageGeom == "Fuselage" || nameFuselageGeom == "Stack"))
                            {

                                relativeCrossSectionAreaAtXStationFuselage.push_back(M_PI * fuseData.allFuselageHeight[j] * fuseData.allFuselageWidth[j] / 4.0); // Approximating cross section as ellipse with height and width from VSP data
                                realtiveWidthAtXstationFuselage.push_back(fuseData.allFuselageWidth[j]);
                                numeratorweightsAndCrossSectionArea += relativeCrossSectionAreaAtXStationFuselage.back() * realtiveWidthAtXstationFuselage.back();
                                weightsOfWeightedAverageCrossSectionArea += realtiveWidthAtXstationFuselage.back();

                                break;
                            }
                        }
                    }
                }
            }

            averageCrossSectionArea = numeratorweightsAndCrossSectionArea / weightsOfWeightedAverageCrossSectionArea;

            // Calculate df_ave - Roskam Part VI pag. 397

            diamterRelativeToAverageCrossSectionAreaFuselage = std::sqrt(averageCrossSectionArea / 0.7854);

            // Claculate DeltaCRolBeta/Dihedral
            deltaWingFuselageCRollBetaToDihedralRatio = -0.0005 * wing.aspectRatio * std::pow(diamterRelativeToAverageCrossSectionAreaFuselage / wing.totalProjectedSpan, 2.0);
            deltaHorizontalFuselageCRollBetaToDihedralRatio = -0.0005 * horizontalTail.aspectRatio * std::pow(diamterRelativeToAverageCrossSectionAreaFuselage / horizontalTail.totalProjectedSpan, 2.0);

            // Calculate DeltaCRollBeta | wing  position along z-axis

            if (builder.getWingData().getWingPosition() == WingPosition::HIGH_WING)
            {

                deltaWingFuselageCRollBetaDueToWingPosition = 0.042 * std::pow(wing.aspectRatio, 0.5) *
                                                              (-std::abs(wing.zloc) / wing.totalProjectedSpan) * (diamterRelativeToAverageCrossSectionAreaFuselage / wing.totalProjectedSpan);
            }

            else if (builder.getWingData().getWingPosition() == WingPosition::LOW_WING)
            {

                deltaWingFuselageCRollBetaDueToWingPosition = -0.042 * std::pow(wing.aspectRatio, 0.5) *
                                                              (std::abs(wing.zloc) / wing.totalProjectedSpan) * (diamterRelativeToAverageCrossSectionAreaFuselage / wing.totalProjectedSpan);
            }
            else
            {
                deltaWingFuselageCRollBetaDueToWingPosition = 0.0;
            }

            // Calculate DeltaCRollBeta | horizontal tail position along z-axis
            if (horizontalTail.zloc > 0.0)
            {

                deltaHorizontalFuselageCRollBetaDueToWingPosition = 0.042 * std::pow(horizontalTail.aspectRatio, 0.5) *
                                                                    (-std::abs(horizontalTail.zloc) / horizontalTail.totalProjectedSpan) * (diamterRelativeToAverageCrossSectionAreaFuselage / horizontalTail.totalProjectedSpan);
            }

            else if (horizontalTail.zloc < 0.0)
            {

                deltaHorizontalFuselageCRollBetaDueToWingPosition = -0.042 * std::pow(horizontalTail.aspectRatio, 0.5) *
                                                                    (std::abs(horizontalTail.zloc) / horizontalTail.totalProjectedSpan) * (diamterRelativeToAverageCrossSectionAreaFuselage / horizontalTail.totalProjectedSpan);
            }

            // Calculate final CRollBeta wing-fuselage contribution

            deltaClDeltaBetaWingFuselageContribution = liftCoefficientWingFuselage.front() *
                                                           (ratioWingFuselageRollBetaLiftCoefficientDueToSweepC2 * factorWingFuselageCompressibilityCorrectionToWingSweep * factorWingFuselageCorrection +
                                                            ratioWingFuselageClBetaToCLAspectRatio) +
                                                       wing.averageDihedral * (ratioWingFuselageClBetaDihedralDueToSweepC2 * factorWingFuselageToWingDihedral + deltaWingFuselageCRollBetaToDihedralRatio) +
                                                       deltaWingFuselageCRollBetaDueToWingPosition + (wing.twist.back() - wing.twist.front()) * factorWingFuselageTwistCorrection;

            // Calculate final CRollBeta horizontal tail-fuselage contribution
            deltaClDeltaBetaHorizontalTailContribution = (liftCoefficientHorizontalFuselage.front() *
                                                              (ratioHorizontalFuselageRollBetaLiftCoefficientDueToSweepC2 * factorHorizontalFuselageCompressibilityCorrectionToWingSweep * factorHorizontalFuselageCorrection +
                                                               ratioHorizontalFuselageClBetaToCLAspectRatio) +
                                                          horizontalTail.averageDihedral * (ratioHorizontalFuselageClBetaDihedralDueToSweepC2 * factorHorizontalFuselageToWingDihedral + deltaHorizontalFuselageCRollBetaToDihedralRatio) +
                                                          deltaHorizontalFuselageCRollBetaDueToWingPosition + (horizontalTail.twist.back() - horizontalTail.twist.front()) * factorHorizontalFuselageTwistCorrection) *
                                                         (horizontalTail.planformArea / wing.planformArea) * (horizontalTail.totalProjectedSpan / wing.totalProjectedSpan);

            // ========================================================================
            // STEP 2: Calculate Flap Effectivness
            // ========================================================================

            for (size_t n = 0; n < wing.mov.type.size(); n++)
            {

                if (wing.mov.type[n] == 'a')
                {

                    // Larghezza del segmento n-esimo
                    double segmentSpan = (wing.mov.eta_outer[n] - wing.mov.eta_inner[n]) * wing.totalProjectedSpan;

                    // cf/c medio del segmento = media tra inner e outer
                    double cfcMean = 0.5 * (wing.mov.cf_c_inner[n] + wing.mov.cf_c_outer[n]);

                    numeratorAverageChordRatio += cfcMean * segmentSpan;
                    weightsOfWeightedAverageChordRatio += segmentSpan;
                }
            }

            averageChordRatio = numeratorAverageChordRatio / weightsOfWeightedAverageChordRatio;

            for (size_t i = 0; i < wing.mov.type.size(); i++)
            {

                if (wing.mov.type[i] == 'a')
                {

                    etaInner = wing.mov.eta_inner[i];
                    etaOuter = wing.mov.eta_outer[i];

                    Interpolant2D kbFactorInterpolator(3, RegressionMethod::POLYNOMIAL);

                    kbFactorInterpolator.addCurve(0.0, x_Kb_taper0, y_Kb_taper0);
                    kbFactorInterpolator.addCurve(0.5, x_Kb_taper05, y_Kb_taper05);
                    kbFactorInterpolator.addCurve(1.0, x_Kb_taper1, y_Kb_taper1);

                    factorKbInner = kbFactorInterpolator.interpolate(etaInner, wing.taperRatio);
                    factorKbOuter = kbFactorInterpolator.interpolate(etaOuter, wing.taperRatio);

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

                    ratioEffectivnessFactorKc.push_back(ratioFlapEffectivnessInterpolator.interpolate(wing.aspectRatio, twoDimensionalFlapEffectivness));

                    tauFlap += flapSpanFactorKb.back() * ratioEffectivnessFactorKc.back() * twoDimensionalFlapEffectivness * wing.finiteSlope / builder.getCommonData().getMeanAirfoilSlopeWing();
                }
            }

            deltaClBetaDeltaAileronsDeflection = wing.finiteSlope * tauFlap * wing.yMAC / wing.totalProjectedSpan;

         
            // ========================================================================
            // STEP 3: Calculate Vertical Tail Contribution to Lateral Stability Derivative
            // ========================================================================


            zCoordinatesOfVerticalTailAerodynamicCenter = verticalTail.zloc + verticalTail.yMAC;
            tailArmVerticalTail = (verticalTail.xloc + verticalTail.deltaXtoLEMAC + 0.25 * verticalTail.MAC) - cogData.xCG;

            deltaClDeltaBetaVerticalTailContribution = singleComponentsDerivativesSideForce.deltaCyDeltaBetaVerticalTailContribution *
                                                       (zCoordinatesOfVerticalTailAerodynamicCenter * std::cos(settings.AoA.front() / 57.3) - tailArmVerticalTail * std::sin(settings.AoA.front() / 57.3)) / wing.totalProjectedSpan;

            // ========================================================================
            // STEP 4: Calculate Total Aircraft CRoll Beta Derivative
            // ========================================================================

            deltaClDeltaBetaAircraft = deltaClDeltaBetaWingFuselageContribution + deltaClDeltaBetaHorizontalTailContribution + deltaClDeltaBetaVerticalTailContribution;

            // ========================================================================
            // STEP 5: Saving results
            // ========================================================================

            singleComponentsDerivativesRoll = {
                .deltaClDeltaBetaWingFuselageContribution = deltaClDeltaBetaWingFuselageContribution,
                .deltaClDeltaBetaHorizontalTailContribution = deltaClDeltaBetaHorizontalTailContribution,
                .deltaClDeltaBetaVerticalTailContribution = deltaClDeltaBetaVerticalTailContribution,
            };

            aircraftDerivativesRoll = {.deltaClDeltaBetaAircraft = deltaClDeltaBetaAircraft,
                                       .deltaClBetaDeltaAileronsDeflection = deltaClBetaDeltaAileronsDeflection};

            settings = restoreSettings.getSettingsToRestore();
            aircraftInfo = restoreSettings.getAircrfatInfoToRestore();
        }

        // Getters for results

        LATERAL_STABILITY::LateralStabilityDerivativesRollToSingleComponent getComponentsLateralStabilityDerivativesRoll() const
        {
            return singleComponentsDerivativesRoll;
        }

        LATERAL_STABILITY::LateralStabilityDerivatives getAircraftLateralStabilityDerivativesRoll() const
        {
            return aircraftDerivativesRoll;
        }
    };
};
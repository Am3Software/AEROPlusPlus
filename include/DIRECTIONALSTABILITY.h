/**
 * @file DIRECTIONALSTABILITY.h
 * @brief Header file for directional stability calculations
 *
 * This file contains classes and structures for calculating directional stability
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
#include "Interpolant.h"
#include "Interpolant2D.h"
#include "Interpolant3D.h"
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
#include "EnumTypeOfStabilizer.h"
#include "EnumTypeOfTail.h"
#include "EnumTypeOfWing.h"
#include "EnumWingPosition.h"
#include "RegressionMethod.h"
#include "ConvDensity.h"
#include "ConvLength.h"
#include "ConvArea.h"
#include "ConvPressure.h"

namespace DIRECTIONAL_STABILITY
{
    /**
     * @struct DirectionalStabilityDerivatives
     * @brief Contains the overall aircraft directional stability derivatives
     */
    struct DirectionalStabilityDerivatives
    {
        double deltaCnDeltaBetaAircraft = 0.0;     ///< Yaw moment coefficient derivative with respect to sideslip angle
        double deltaCyDeltaBetaAircraft = 0.0;     ///< Side force coefficient derivative with respect to sideslip angle
        double deltaCnDeltaRudderDeflection = 0.0; ///< Power rudder control
    };

    /**
     * @struct DirectionalStabilityDerivativesYawToSingleComponent
     * @brief Contains directional stability contributions related to yaw moment from individual aircraft components
     */
    struct DirectionalStabilityDerivativesYawToSingleComponent
    {
        double deltaCnDeltaBetaWingContribution = 0.0;           ///< Wing contribution to yaw moment derivative
        double deltaCnDeltaBetaCanardContribution = 0.0;         ///< Canard contribution to yaw moment derivative
        double deltaCnDeltaBetaHorizontalTailContribution = 0.0; ///< Horizontal tail contribution to yaw moment derivative
        double deltaCnDeltaBetaVerticalTailContribution = 0.0;   ///< Vertical tail contribution to yaw moment derivative
        double deltaCnDeltaBetaFuselageContribution = 0.0;       ///< Fuselage contribution to yaw moment derivative
        double deltaCnDeltaBetaNacelleContribution = 0.0;        ///< Nacelle contribution to yaw moment derivative
    };

    /**
     * @struct DirectionalStabilityDerivativesSideForceToSingleComponent
     * @brief Contains directional stability contributions related to side force from individual aircraft components
     */
    struct DirectionalStabilityDerivativesSideForceToSingleComponent
    {
        double deltaCyDeltaBetaWingContribution = 0.0;           ///< Wing contribution to side force derivative
        double deltaCyDeltaBetaCanardContribution = 0.0;         ///< Canard contribution to side force derivative
        double deltaCyDeltaBetaHorizontalTailContribution = 0.0; ///< Horizontal tail contribution to side force derivative
        double deltaCyDeltaBetaVerticalTailContribution = 0.0;   ///< Vertical tail contribution to side force derivative
        double deltaCyDeltaBetaFuselageContribution = 0.0;       ///< Fuselage contribution to side force derivative
        double deltaCyDeltaBetaNacelleContribution = 0.0;        ///< Nacelle contribution to side force derivative
        double deltaCyDeltaBetaWindMillingPropeller = 0.0;       ///< Windmilling propeller contribution to side force derivative
    };

    /**
     * @class DirectionalStabilityCalculator
     * @brief Calculates directional stability derivatives for an aircraft (dCn/dBeta and dCy/dBeta)
     *
     * This class implements an hybridization of Perkins, Roskam and DATCOM methods for computing directional
     * stability contributions from various aircraft components including wing,
     * fuselage, tail surfaces, nacelles, and propellers.
     */
    class DirectionalStabilityCalculator
    {

    protected:
        BuildAircraft &builder;     ///< Reference to the aircraft builder
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
        DIRECTIONAL_STABILITY::DirectionalStabilityDerivatives aircraftDirectionalDerivatives;                                 ///< Aircraft-level derivatives
        DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesYawToSingleComponent singleComponentsDerivativesYaw;             ///< Component-level derivatives
        DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesSideForceToSingleComponent singleComponentsDerivativesSideForce; ///< Component-level side force derivatives

        std::string nameOfAircraft; ///< Aircraft name identifier

        // Component contributions to directional stability
        double deltaCnDeltaBetaWingContribution = 0.0;           ///< Wing yaw moment coefficient derivative
        double deltaCnDeltaBetaHorizontalTailContribution = 0.0; ///< Horizontal tail yaw moment coefficient derivative
        double deltaCnDeltaBetaCanardContribution = 0.0;         ///< Canard yaw moment coefficient derivative
        double deltaCnDeltaBetaVerticalTailContribution = 0.0;   ///< Vertical tail yaw moment coefficient derivative
        double deltaCnDeltaBetaFuselageContribution = 0.0;       ///< Fuselage yaw moment coefficient derivative
        double deltaCnDeltaBetaNacelleContribution = 0.0;        ///< Nacelle yaw moment coefficient derivative

        // Component contributions to side force derivative
        double deltaCyDeltaBetaWingContribution = 0.0;           ///< Wing contribution to side force derivative
        double deltaCyDeltaBetaCanardContribution = 0.0;         ///< Canard contribution to side force derivative
        double deltaCyDeltaBetaHorizontalTailContribution = 0.0; ///< Horizontal tail contribution to side force derivative
        double deltaCyDeltaBetaVerticalTailContribution = 0.0;   ///< Vertical tail contribution to side force derivative
        double deltaCyDeltaBetaFuselageContribution = 0.0;       ///< Fuselage contribution to side force derivative
        double deltaCyDeltaBetaNacelleContribution = 0.0;        ///< Nacelle contribution to side force derivative

        // Total aircraft derivatives
        double deltaCnDeltaBetaAircraft = 0.0;          ///< Total aircraft yaw moment derivative wrt sideslip
        double deltaCyDeltaBetaAircraft = 0.0;          ///< Side force coefficient derivative with respect to sideslip angle
        double deltaCnDeltaClAircraft = 0.0;            ///< Yaw moment derivative wrt roll rate
        double deltaCnDeltaBetaDueToWingPosition = 0.0; ///< Wing position correction factor
        double deltaCnDeltaRudderDeflection = 0.0;      ///< Power rudder control

        // Propeller contributions
        double deltaCnDeltaBetaPropeller = 0.0;            ///< Propeller yaw moment derivative
        double deltaCyDeltaBetaWindMillingPropeller = 0.0; ///< Side force derivative for windmilling propeller

        // Wing geometric parameters
        double xTEWing = 0.0;                                        ///< X-coordinate of wing trailing edge [m]
        double wingMACFeet = 0.0;                                    ///< Wing mean aerodynamic chord [ft]
        double wingSpanFeet = 0.0;                                   ///< Wing span [ft]
        double volumetricRatio = 0.0;                                ///< Horizontal tail volume ratio
        double volumetricRatioCanard = 0.0;                          ///< Canard volume ratio
        double tailArmCanard = 0.0;                                  ///< Canard moment arm [m]
        double distanceFromeTEWingToQuarterChordMacHorizontal = 0.0; ///< Distance from wing TE to horizontal tail quarter chord [ft]
        double zWingQuarerRootChordPosition = 0.0;                   ///< Z-coordinate of wing quarter root chord [m]

        // Fuselage section indices
        int idxNoseEnd = -1;                          ///< Last nose section index (before wing LE)
        int idxTailStart = -1;                        ///< First tail section index (after wing TE)
        double maxFuselageDepth;                      ///< Maximum fuselage depth (height) [m]
        std::vector<double> diamterTailConeFuselage;  ///< X-stations along the fuselage [m]
        std::vector<double> xStationTailConeFuselage; ///< Diameters along the fuselage [m]

        // Fuselage discretization arrays (Perkins and Roskam methods)
        std::vector<double> xCoordinatePerkinsFront;                                 ///< X-coordinates for fuselage front sections [m]
        std::vector<double> xCoordinatePerkinsRear;                                  ///< X-coordinates for fuselage rear sections [m]
        std::vector<double> diametersFront;                                          ///< Front diameters of the fuselage, ordered from nose to LE wing [m]
        std::vector<double> diametersRear;                                           ///< Rear diameters of the fuselage, ordered from TE wing to tail end [m]
        std::vector<double> x1CentroidDistancesFront;                                ///< Centroid distances for front sections [m]
        std::vector<double> x1CentroidDistancesRear;                                 ///< Centroid distances for rear sections [m]
        std::vector<double> deltaXFromXPointWhereFlowCasesToBePotential;             ///< X-stations from the point where flow becomes potential  [m]
        std::vector<double> diametersFromXPointWhereFlowCasesToBePotential;          ///< Diameters from the point where flow becomes potential  [m]
        std::vector<double> widthsFromXPointWhereFlowCasesToBePotential;             ///< Widths from the point where flow becomes potential  [m]
        std::vector<double> xStationToInterpolateDiamterWhereFlowCasesToBePotential; ///< X-stations for interpolation of diameter at the point where flow becomes potential [m]
        int indexFoundWhereFlowCasesToBePotential = 0;                               ///< Index of the fuselage section where flow becomes potential
        int numSubdivisionsFront = 6;                                                ///< Number of subdivisions for front fuselage section
        int numSubdivisionsRear = 6;                                                 ///< Number of subdivisions for rear fuselage section
        double distanceNoseToWing = 0.0;                                             ///< Distance from nose end to wing LE [m]
        double stepFront = 0.0;                                                      ///< Step size for front subdivisions [m]
        double distanceWingToTail = 0.0;                                             ///< Distance from wing TE to tail start [m]
        double stepRear = 0.0;                                                       ///< Step size for rear subdivisions [m]
        double xPointWhereFlowCasesToBePotentialToFuselageLengthRatio = 0.0;
        double xPointWhereFlowCasesToBePotential = 0.0;
        double averageXPointWhereFuselageReachesMaximumNegativeSlope = 0.65;
        double crossSectionAreaFromXPointWhereFlowCasesToBePotential = 0.0; ///< Cross-sectional area from the point where flow becomes potential [m²]
        double diameterAtXPointWhereFlowCasesToBePotential = 0.0;           ///< Fuselage diameter at the point where flow becomes potential [m]
        double factorKiToSideForceFuselageContirbution = 0.0;

        // Aerodynamic and dimensional parameters
        double dynamicPressureFreeStream = 0.0;  ///< Freestream dynamic pressure [Pa]
        double psfDynamicPressure = 0.0;         ///< Dynamic pressure [psf]
        double rootChordFeet = 0.0;              ///< Wing root chord [ft]
        double fuselageDiameterFeet = 0.0;       ///< Maximum fuselage diameter [ft]
        double planformWingAreaSquareFeet = 0.0; ///< Wing planform area [ft²]
        double stepFrontFeet = 0.0;              ///< Front step size [ft]
        double stepRearFeet = 0.0;               ///< Rear step size [ft]
        double sideFuselageArea = 0.0;           ///< Fuselage side projected area [m²]
        double sideFuselageAreaFeetSquare = 0.0; ///< Fuselage side projected area [ft²]

        // Nacelle discretization arrays
        std::vector<double> deltaXNacellePerkinsFront;       ///< Delta X for nacelle front sections [m]
        std::vector<double> x1CentroidDistancesNacelleFront; ///< Centroid distances for nacelle front [m]
        std::vector<double> diametersNacelleFront;           ///< Nacelle front diameters (heights) [m]
        std::vector<double> deltaXNacellePerkinsRear;        ///< Delta X for nacelle rear sections [m]
        std::vector<double> x1CentroidDistancesNacelleRear;  ///< Centroid distances for nacelle rear [m]
        std::vector<double> diametersNacelleRear;            ///< Nacelle rear diameters [m]
        std::vector<double> nacelleWidth;                    ///< Nacelle widths at stations [m]
        std::vector<double> frontNacelleDiamterFeet;         ///< Front nacelle diameters [ft]
        std::vector<double> frontNacelleDeltaXFeet;          ///< Front nacelle delta X [ft]
        std::vector<double> rearNacelleDiamterFeet;          ///< Rear nacelle diameters [ft]
        std::vector<double> rearNacelleDeltaXFeet;           ///< Rear nacelle delta X [ft]
        double summationNacelleContribution = 0.0;           ///< Sum of nacelle contributions
        double nacelleMomentFactor = 0.0;                    ///< Nacelle moment arm factor
        double maxNacelleWidthFeet = 0.0;                    ///< Maximum nacelle width [ft]
        double nacelleLengthFeet = 0.0;                      ///< Nacelle length [ft]

        // Kbeta empirical factors and fuselage dimensions
        double kBetaFactorFuselage = 0.0;                      ///< Fuselage empirical factor from Perkins charts
        double kBetaFactorNacelle = 0.0;                       ///< Nacelle empirical factor from Perkins charts
        double fuselageDiamterAtQuarterLength = 0.0;           ///< Fuselage diameter at 1/4 length [m]
        double fuselageDiamterAtQuarterLengthFeet = 0.0;       ///< Fuselage diameter at 1/4 length [ft]
        double fuselageWidthAtQuarterLength = 0.0;             ///< Fuselage width at 1/4 length [ft]
        double fuselageDiameterAtThreeQuarterLength = 0.0;     ///< Fuselage diameter at 3/4 length [m]
        double fuselageDiameterAtThreeQuarterLengthFeet = 0.0; ///< Fuselage diameter at 3/4 length [ft]
        double fuselageWidthAtThreeQuarterLength = 0.0;        ///< Fuselage width at 3/4 length [ft]
        double fuselageLengthFeet = 0.0;                       ///< Total fuselage length [ft]
        double distanceFromPropellerDiskToCOG = 0.0;           ///< Distance from propeller disk to COG [m]
        std::vector<double> xStationNose;                      ///< X-stations for nose section [m]
        std::vector<double> xStationTail;                      ///< X-stations for tail section [m]
        std::vector<double> dimatersNose;                      ///< Diameters at nose stations [m]
        std::vector<double> diametersTail;                     ///< Diameters at tail stations [m]

        // Nacelle dimensional parameters
        double sideAreaNacelle = 0.0;                                         ///< Nacelle side projected area [m²]
        double sideAreaNacelleFeetSquare = 0.0;                               ///< Nacelle side projected area [ft²]
        double nacelleDiamterAtQuarterLength = 0.0;                           ///< Nacelle diameter at 1/4 length [m]
        double nacelleDiamterAtQuarterLengthFeet = 0.0;                       ///< Nacelle diameter at 1/4 length [ft]
        double nacelleWidthAtQuarterLength = 0.0;                             ///< Nacelle width at 1/4 length [ft]
        double nacelleDiameterAtThreeQuarterLength = 0.0;                     ///< Nacelle diameter at 3/4 length [m]
        double nacelleDiameterAtThreeQuarterLengthFeet = 0.0;                 ///< Nacelle diameter at 3/4 length [ft]
        double nacelleWidthAtThreeQuarterLength = 0.0;                        ///< Nacelle width at 3/4 length [ft]
        int halfNacelleIndex = -1;                                            ///< Index for nacelle mid-point
        std::vector<double> diamtersToQuarterLengthNacelleDiameter;           ///< Diameters up to 1/4 nacelle length [m]
        std::vector<double> diamtersToThreeQuarterLengthNacelleDiameter;      ///< Diameters up to 3/4 nacelle length [m]
        std::vector<double> nacelleStationToQuarterLengthNacelleDiamter;      ///< Stations up to 1/4 nacelle length [m]
        std::vector<double> nacelleStationToThreeQuarterLengthNacelleDiamter; ///< Stations up to 3/4 nacelle length [m]
        std::vector<double> nacelleWidthToQuarterLengthNacelle;               ///< Widths up to 1/4 nacelle length [m]
        std::vector<double> nacelleWidthToThreeQuarterLengthNacelle;          ///< Widths up to 3/4 nacelle length [m]

        // vertical tail variables related
        double equivalentVerticalTailAspectRatio = 0.0;
        double verticalTailLiftSlope = 0.0;
        double firstPortionToDeltaCnDeltaBetaVerticalTailPerkins = 0.0;
        double deltaContributionToVerticalTail = 0.0;
        double tailArmVerticalTail = 0.0; ///< Vertical tail moment arm [m]
        double fuselageDepthInTheVerticalLocation = 0.0;
        double spanVerticalToDepthFuselageRatio = 0.0;
        double ratioToAspectRatioVerticalInThePresenceOfTheFuselage = 0.0;
        double ratioToAspectRatioVerticalInThePresenceOfTheHorizontal = 0.0;
        double xLocationOfTheAerodynamicCenterHorizontalTail = 0.25;
        double relativePositionBetweenHorizontalAndVertical = 0.0;
        double zLocationHorizontalToSpanVerticalRatio = 0.0;
        double factorKvh = 0.0;
        double finiteLiftSlopeVerticalTail = 0.0;
        double sideWashGradientTimesDynamicPressureRatio = 0.0;
        double factorKvForSingleVerticalTail = 0.0;
        double deltaCnDeltaBetaDueToVerticalTailSurface = 0.0;
        double dynamicPressureRatioAtVerticalTail = 0.0;

        // Vee tail variables related
        double projectedPlanformAreaToVTail = 0.0; ///< Projected planform area of wing and horizontal tail to vertical tail [m²]
        double projectedSpanToVtail = 0.0;         ///< Projected span of wing and horizontal tail to vertical tail [m]
        double equivalentHorizontalTailArea = 0.0;
        double kFactorToLiftSlopeVeeTail = 0.0;

        // Vertical twin variables related
        double ratioSideForceWithResepectToBeta = 0.0;
        double effectiveSideForceWRTBetaVerticalTwin = 0.0;
        double zCoordinatesOfVerticalTailAerodynamicCenter = 0.0;

        // Rudder power control variables related
        double etaInner = 0.0;
        double etaOuter = 0.0;

        double numeratorAverageChordRatio = 0.0;
        double weightsOfWeightedAverageChordRatio = 0.0;
        double averageChordRatio = 0.0;
        double factorKbInner = 0.0;
        double factorKbOuter = 0.0;
        double thetaFactor = 0.0;
        double twoDimensionalFlapEffectivness = 0.0; // alphaCl - DATCOM -Roskam method
        double tauRudder = 0.0;
        std::vector<double> flapSpanFactorKb;
        std::vector<double> ratioEffectivnessFactorKc;

        /**
         * Kbeta empirical factor data from Perkins charts
         * Used for fuselage and nacelle directional stability calculations
         *
         * X-axis: b_f / d_f (body width to diameter ratio at specific station)
         * Y-axis: Kbeta (empirical factor)
         *
         * Multiple curves are provided for different fineness ratios (l_f/d_f)
         * where l_f is body length and d_f is maximum body diameter
         */

        // Fineness ratio l_f/d_f = 2.5
        std::vector<double> Kbeta_x_2_5 = {
            0.104036, 0.14054, 0.186464, 0.309212, 0.406106,
            0.504954, 0.608771, 0.709419, 0.805717};
        std::vector<double> Kbeta_y_2_5 = {
            0.172295, 0.183396, 0.196881, 0.234701, 0.263481,
            0.294412, 0.325292, 0.354655, 0.384061};

        // l_f/d_f = 3.0
        std::vector<double> Kbeta_x_3 = {
            0.10265, 0.142953, 0.203788, 0.30698, 0.405159,
            0.50546, 0.609455, 0.708074, 0.80584};
        std::vector<double> Kbeta_y_3 = {
            0.148636, 0.161456, 0.179854, 0.210637, 0.240438,
            0.271458, 0.301613, 0.332029, 0.361834};

        // l_f/d_f = 4.0
        std::vector<double> Kbeta_x_4 = {
            0.101248, 0.203189, 0.305544, 0.40456, 0.504819,
            0.60676, 0.707425, 0.805207};
        std::vector<double> Kbeta_y_4 = {
            0.114537, 0.145126, 0.175711, 0.20571, 0.235697,
            0.266286, 0.296062, 0.32628};

        // l_f/d_f = 5.0
        std::vector<double> Kbeta_x_5 = {
            0.1007, 0.202244, 0.304582, 0.403607, 0.50472,
            0.607489, 0.706953, 0.804288};
        std::vector<double> Kbeta_y_5 = {
            0.0810489, 0.112056, 0.142228, 0.172433, 0.203031,
            0.233612, 0.264434, 0.29383};

        // l_f/d_f = 6.0
        std::vector<double> Kbeta_x_6 = {
            0.099545, 0.201909, 0.304669, 0.402866, 0.503133,
            0.605919, 0.706195, 0.805634};
        std::vector<double> Kbeta_y_6 = {
            0.0529428, 0.0837347, 0.114109, 0.144323, 0.174516,
            0.20551, 0.23591, 0.266112};

        // l_f/d_f = 7.0
        std::vector<double> Kbeta_x_7 = {
            0.0996513, 0.200755, 0.303542, 0.402152, 0.502428,
            0.605646, 0.705896, 0.802833};
        std::vector<double> Kbeta_y_7 = {
            0.0353681, 0.0657593, 0.0967535, 0.126963, 0.157363,
            0.188766, 0.218546, 0.24836};

        // l_f/d_f = 8.0
        std::vector<double> Kbeta_x_8 = {
            0.0990992, 0.200609, 0.303404, 0.40198, 0.503093,
            0.60631, 0.705335, 0.803075};
        std::vector<double> Kbeta_y_8 = {
            0.021935, 0.0521153, 0.0833162, 0.1127, 0.143297,
            0.174701, 0.204906, 0.234092};

        // l_f/d_f = 10.0
        std::vector<double> Kbeta_x_10 = {
            0.0970668, 0.201112, 0.303053, 0.401647, 0.502743,
            0.60596, 0.754502, 0.802319};
        std::vector<double> Kbeta_y_10 = {
            0.0027281, 0.0341229, 0.0647123, 0.094509, 0.124694,
            0.156097, 0.201509, 0.215699};

        // Vertical tail lift slope data for various aspect ratios
        std::vector<double> equivalentAspectRatioVerticalTail = {
            0.00941024, 0.128102, 0.229043, 0.349922, 0.527122,
            0.739681, 0.893405, 1.08975, 1.26472, 1.48846,
            1.76238, 1.99185, 2.25655, 2.49888, 2.80578,
            3.04811, 3.38953, 3.68078, 3.95009, 4.19103,
            4.50953, 4.8483, 5.10196, 5.34891, 5.64435,
            5.9472, 6.25704, 6.47479};

        std::vector<double> liftSlopeVerticalTail = {
            -9.66e-07, 0.00355346, 0.00757104, 0.0122616, 0.0171581,
            0.022265, 0.0258605, 0.0298099, 0.0335462, 0.0371997,
            0.0414958, 0.0444953, 0.0476325, 0.0500489, 0.0533225,
            0.0557389, 0.0587877, 0.061194, 0.0630969, 0.0647881,
            0.066826, 0.0684969, 0.0695326, 0.0707147, 0.0716691,
            0.0728397, 0.0740088, 0.074544};

        std::vector<double> x_lambda1 = {
            0.00492954, 0.0294306, 0.0588647, 0.108073, 0.167129,
            0.265724, 0.384109, 0.487752, 0.601274, 0.714835,
            0.843219, 0.941981, 1.07534, 1.21369, 1.34215,
            1.50029, 1.68315, 1.85614, 2.01433, 2.14781,
            2.3308, 2.49894, 2.67205, 2.81055, 2.97376,
            3.1172, 3.31996, 3.58699, 3.81938, 4.09626,
            4.47695, 4.80325, 5.17897, 5.52997, 5.9996,
            6.51373, 7.00809};

        std::vector<double> y_lambda1 = {
            0.00803152, 0.132505, 0.263003, 0.393506, 0.546095,
            0.704718, 0.853309, 0.951708, 1.05412, 1.13446,
            1.21881, 1.28107, 1.34535, 1.38755, 1.42573,
            1.45589, 1.48806, 1.50417, 1.50823, 1.50024,
            1.45813, 1.42205, 1.37191, 1.32577, 1.28366,
            1.23552, 1.19543, 1.14532, 1.11929, 1.09327,
            1.06929, 1.05734, 1.0454, 1.03948, 1.04162,
            1.03976, 1.03388};

        std::vector<double> x_lambda06 = {
            -0.00495048, -0.0100685, -0.000377047, 0.00437793, 0.00422083,
            0.0386508, 0.0730633, 0.132176, 0.196281, 0.275188,
            0.383764, 0.502244, 0.640463, 0.80835, 1.08993,
            1.35675, 1.53465, 1.70763, 1.90535, 2.04871,
            2.18221, 2.28112, 2.39488, 2.51853, 2.71144,
            2.85985, 3.02803, 3.17148, 3.3149, 3.47316,
            3.63141, 3.84403, 4.05664, 4.30385, 4.556,
            4.86253, 5.20861, 5.51018, 5.85623, 6.17263,
            6.50879, 6.98833};

        std::vector<double> y_lambda06 = {
            0.00401364, 0.104389, 0.216813, 0.325221, 0.415559,
            0.515946, 0.62637, 0.746838, 0.839203, 0.947632,
            1.05205, 1.14644, 1.26091, 1.37137, 1.48388,
            1.56225, 1.59843, 1.62458, 1.63467, 1.63471,
            1.61869, 1.59865, 1.56254, 1.52644, 1.46226,
            1.40408, 1.3419, 1.29376, 1.25967, 1.21957,
            1.18548, 1.16145, 1.13742, 1.1134, 1.09541,
            1.08144, 1.06147, 1.05152, 1.0476, 1.03966,
            1.03574, 1.02785};

        std::vector<double> x_xcv05 = {
            -0.0030876, -0.0324491, -0.0819857, -0.137944, -0.205824,
            -0.275518, -0.34152, -0.424909, -0.486273, -0.53939,
            -0.593406, -0.634554, -0.691217, -0.728667, -0.755105,
            -0.788846, -0.812525, -0.842538, -0.87163, -0.89798,
            -0.922475, -0.943284, -0.962233, -0.98033, -0.99113,
            -1.00196};

        std::vector<double> y_xcv05 = {
            1.04822, 1.03008, 1.00295, 0.972191, 0.936033,
            0.905454, 0.884086, 0.866648, 0.86374, 0.862577,
            0.866981, 0.886033, 0.921954, 0.95207, 0.989451,
            1.03433, 1.07538, 1.14059, 1.20764, 1.27279,
            1.34534, 1.42524, 1.51438, 1.58313, 1.64253,
            1.69267};

        std::vector<double> x_xcv06 = {
            0.00271779, -0.0284928, -0.0697879, -0.105595, -0.149637,
            -0.20195, -0.275342, -0.325777, -0.383531, -0.44128,
            -0.492589, -0.549369, -0.595149, -0.624423, -0.653667,
            -0.688393, -0.713047, -0.736785, -0.765965, -0.796964,
            -0.820649, -0.846143, -0.86613, -0.887919, -0.906985,
            -0.921501, -0.935049, -0.957684, -0.976638, -0.999213};

        std::vector<double> y_xcv06 = {
            1.1463, 1.12263, 1.09539, 1.06622, 1.03901,
            1.00265, 0.961009, 0.93944, 0.919818, 0.902047,
            0.893453, 0.892337, 0.894782, 0.904421, 0.923319,
            0.945993, 0.968535, 0.991066, 1.03033, 1.07333,
            1.11253, 1.15916, 1.20942, 1.26896, 1.32106,
            1.36384, 1.42328, 1.50506, 1.59234, 1.69264};

        std::vector<double> x_xcv07 = {
            0.000199579, -0.0310169, -0.067751, -0.105395, -0.148539,
            -0.199942, -0.243081, -0.292617, -0.349503, -0.399938,
            -0.430197, -0.466861, -0.49526, -0.528243, -0.567619,
            -0.594157, -0.622509, -0.65359, -0.683738, -0.714802,
            -0.750386, -0.777775, -0.798737, -0.821495, -0.845145,
            -0.865144, -0.885107, -0.90962, -0.924119, -0.942234,
            -0.958529, -0.977524, -1.00103};

        std::vector<double> y_xcv07 = {
            1.21856, 1.19303, 1.16018, 1.12918, 1.09641,
            1.05818, 1.02726, 1.00012, 0.965674, 0.944105,
            0.931534, 0.920898, 0.917562, 0.912434, 0.912944,
            0.918844, 0.930323, 0.947393, 0.970007, 0.992633,
            1.03384, 1.06012, 1.09187, 1.13476, 1.18507,
            1.23163, 1.28929, 1.35628, 1.40462, 1.46782,
            1.52729, 1.60161, 1.69637};

        std::vector<double> x_xcv08 = {
            0.000434377, -0.031733, -0.0629612, -0.0896108, -0.130935,
            -0.163091, -0.194301, -0.231945, -0.263156, -0.300794,
            -0.351258, -0.394373, -0.437459, -0.474134, -0.507106,
            -0.534589, -0.559313, -0.588604, -0.620625, -0.659026,
            -0.688247, -0.71654, -0.745755, -0.775856, -0.802312,
            -0.825956, -0.845966, -0.86323, -0.884121, -0.909562,
            -0.925862, -0.943983, -0.962104, -0.982924, -0.998286};

        std::vector<double> y_xcv08 = {
            1.29263, 1.25601, 1.22678, 1.1975, 1.161,
            1.12808, 1.10441, 1.07341, 1.04974, 1.0206,
            0.989772, 0.966256, 0.951999, 0.937659, 0.936235,
            0.932887, 0.933208, 0.937291, 0.946966, 0.965983,
            0.992288, 1.02229, 1.05044, 1.08787, 1.1197,
            1.17186, 1.21471, 1.25753, 1.3115, 1.3748,
            1.43242, 1.49377, 1.55512, 1.63132, 1.69633};

        std::vector<double> x_Kvh = {
            0.00528463, 0.047778, 0.0867195, 0.116767, 0.161036,
            0.207125, 0.254975, 0.304644, 0.361432, 0.418234,
            0.471542, 0.540863, 0.615612, 0.699255, 0.788415,
            0.89002, 1.01494, 1.11134, 1.21313, 1.32743,
            1.42032, 1.54721, 1.65983, 1.75282, 1.83688,
            1.95317};

        std::vector<double> y_Kvh = {
            0.0112236, 0.0730661, 0.131152, 0.181711, 0.245432,
            0.305441, 0.369192, 0.429231, 0.49492, 0.558746,
            0.611363, 0.677157, 0.735543, 0.801457, 0.848785,
            0.907397, 0.960614, 0.998687, 1.03308, 1.0713,
            1.09817, 1.12904, 1.15234, 1.16803, 1.17991,
            1.19207};

        // K factor vs Aspect Ratio (lift slope, Vee tail, directional)
        // Data digitized from plots via Plot Digitizer 2.6.8
        // X = Aspect Ratio, Y = K factor

        // Lambda = 0.25
        std::vector<double> K_factor_x_lambda_025 = {
            3.00067, 3.14328, 3.33173, 3.52008, 3.72369,
            4.00365, 4.50730, 4.98551, 5.49917, 5.99747,
            6.48557, 7.00409, 7.48697, 7.98509, 8.48829,
            8.98634, 9.47928, 9.98240};

        std::vector<double> K_factor_y_lambda_025 = {
            0.607499, 0.612965, 0.620051, 0.626121, 0.632392,
            0.641090, 0.653618, 0.665540, 0.676441, 0.685921,
            0.694794, 0.703255, 0.710502, 0.717950, 0.725398,
            0.732033, 0.738263, 0.744898};

        // Lambda = 0.5
        std::vector<double> K_factor_x_lambda_05 = {
            2.99893, 3.22805, 3.49782, 3.99651, 4.51027,
            4.99340, 5.50191, 5.99001, 6.49837, 7.00669,
            7.50985, 7.98763, 8.48570, 8.98879, 9.48684,
            9.99496};

        std::vector<double> K_factor_y_lambda_05 = {
            0.645094, 0.652987, 0.661281, 0.675029, 0.687149,
            0.697241, 0.707330, 0.716202, 0.724665, 0.732518,
            0.739559, 0.746604, 0.753443, 0.759671, 0.766306,
            0.771924};

        // Lambda = 1.0
        std::vector<double> K_factor_x_lambda_1 = {
            2.80820, 3.05270, 3.34285, 3.63291, 3.99410,
            4.48751, 4.99101, 5.49441, 5.98753, 6.48574,
            6.99913, 7.47695, 7.99019, 8.34091, 8.62551,
            8.97618, 9.47921, 9.95181};

        std::vector<double> K_factor_y_lambda_1 = {
            0.669508,
            0.679024,
            0.688128,
            0.696215,
            0.705106,
            0.716619,
            0.727521,
            0.737204,
            0.745466,
            0.753930,
            0.761783,
            0.769234,
            0.775461,
            0.780694,
            0.784515,
            0.789139,
            0.794758,
            0.800584};

        // LOW WING  Ki factor Roskam Part VI - zw/df positive
        std::vector<double> Ki_x_low_wing = {
            -1.19e-05, 0.0992818, 0.195163, 0.301377, 0.40243,
            0.503448, 0.602778, 0.702131, 0.803172, 0.904166,
            1.00526};
        std::vector<double> Ki_y_low_wing = {
            1.00086, 1.05216, 1.10085, 1.15044, 1.19914,
            1.25044, 1.29915, 1.34612, 1.39569, 1.44872,
            1.49484};

        // HIGH WING Ki factor Roskam Part VI - zw/df negative
        std::vector<double> Ki_x_high_wing = {
            -0.00183138, -0.0964458, -0.22968, -0.305029, -0.408353,
            -0.524064, -0.609925, -0.697616, -0.795729, -0.893843,
            -0.962269, -1.00948};
        std::vector<double> Ki_y_high_wing = {
            1.00602, 1.08398, 1.19795, 1.26050, 1.34273,
            1.44213, 1.51325, 1.59038, 1.67091, 1.75144,
            1.81316, 1.84654};

        // Kv factor - side force vertical tail (CSV 31)
        std::vector<double> Kv = {
            2.01132, 2.51182, 3.01659, 3.49571};
        std::vector<double> verticalTailSpanToFuselageDepthInRegionOfVerticalRatio = {
            0.751998, 0.835855, 0.912854, 0.995003};

        // Vertical twins side force evaluation
        //  bh/lf = 1.0
        std::vector<double> ratio_WHF_x_bh_lf_1 = {
            0.00632926, 0.0386912, 0.0861026, 0.1448, 0.205755,
            0.305086, 0.402155, 0.505995, 0.603816, 0.703893,
            0.80397, 0.899531, 1.00036};

        std::vector<double> ratio_WHF_y_bh_lf_1 = {
            1.0, 0.990821, 0.980883, 0.97172, 0.96256,
            0.951895, 0.946598, 0.942842, 0.93908, 0.936855,
            0.93463, 0.933169, 0.932479};

        //  bh/lf = 0.8
        std::vector<double> ratio_WHF_x_bh_lf_08 = {
            0.00482493, 0.0326728, 0.0672937, 0.102667, 0.20276,
            0.303602, 0.402937, 0.503019, 0.6031, 0.701674,
            0.801751, 0.902581, 1.0019};

        std::vector<double> ratio_WHF_y_bh_lf_08 = {
            0.999234, 0.989281, 0.977799, 0.967085, 0.942609,
            0.922738, 0.906701, 0.896804, 0.889975, 0.88468,
            0.881688, 0.879463, 0.878772};

        //  bh/lf = 0.6
        std::vector<double> ratio_WHF_x_bh_lf_06 = {
            0.000309697, 0.0341816, 0.0733229, 0.101925, 0.205792,
            0.303632, 0.401467, 0.504564, 0.601637, 0.700968,
            0.802553, 0.902635, 1.00271};

        std::vector<double> ratio_WHF_y_bh_lf_06 = {
            0.999998, 0.983912, 0.96476, 0.951739, 0.911921,
            0.88284, 0.85913, 0.842329, 0.83243, 0.821764,
            0.814937, 0.806574, 0.802814};

        //  bh/lf = 0.4
        std::vector<double> ratio_WHF_x_bh_lf_04 = {
            0.00106271, 0.0168754, 0.0409644, 0.0733348, 0.103447,
            0.149367, 0.205069, 0.300664, 0.356361, 0.405285,
            0.505382, 0.601713, 0.702554, 0.801135, 0.900465,
            1.00356};

        std::vector<double> ratio_WHF_y_bh_lf_04 = {
            0.999232, 0.983899, 0.969339, 0.948648, 0.928722,
            0.901136, 0.872023, 0.82606, 0.803853, 0.783941,
            0.754095, 0.729617, 0.712047, 0.697545, 0.686879,
            0.680053};

        // C bh/lf = 0.2
        std::vector<double> ratio_WHF_x_bh_lf_02 = {
            0.00482493, 0.0183848, 0.0432308, 0.0786223, 0.103469,
            0.205864, 0.303735, 0.370732, 0.401598, 0.503215,
            0.580739, 0.60106, 0.70266, 0.801247, 0.900582,
            1.00067};

        std::vector<double> ratio_WHF_y_bh_lf_02 = {
            0.999234, 0.977762, 0.957065, 0.92103, 0.898799,
            0.815246, 0.742431, 0.702585, 0.681893, 0.631332,
            0.602235, 0.594578, 0.567802, 0.545627, 0.529591,
            0.517391};

        // dCy/dBeta effective Vertical twin
        // gammaLE = 0°
        std::vector<double> effectiveAspectRatioVerticalTwin_0 = {
            0.489604, 0.590556, 0.740132, 0.942137, 1.10675,
            1.24145, 1.45847, 1.70175, 1.9675, 2.22958,
            2.51044, 2.80628, 3.01228, 3.24823, 3.51417,
            3.79138, 4.00115, 4.24842, 4.50691, 4.63431};

        std::vector<double> sideForceWRTBetaEffective_0 = {
            0.955684, 1.12458, 1.36259, 1.63135, 1.8387,
            1.99614, 2.25342, 2.48774, 2.73742, 2.93342,
            3.11026, 3.28713, 3.38332, 3.51024, 3.62953,
            3.73734, 3.8182, 3.89528, 3.98005, 4.00709};

        // gammaLE = 20°
        std::vector<double> effectiveAspectRaticoVerticalTwin_20 = {
            0.508384, 0.60189, 0.788937, 0.931073, 1.11815,
            1.30521, 1.51477, 1.7169, 1.96023, 2.18488,
            2.45448, 2.75033, 3.01626, 3.28969, 3.50695,
            3.88157, 4.27119, 4.6084, 4.85944, 5.01309};

        std::vector<double> sideForceWRTBetaEffective_20 = {
            0.928866, 1.06324, 1.30897, 1.50861, 1.73517,
            1.96939, 2.19982, 2.38038, 2.58784, 2.74926,
            2.92609, 3.09912, 3.22608, 3.35306, 3.4416,
            3.5764, 3.69972, 3.78078, 3.83869, 3.85426};

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

        // DeltaCn_beta versus wing position to maximum fuselage diameter ratio

        // Wing position to maximum fuselage diameter ratio
        std::vector<double> wingToMaximumFuselageDiameterRatio = {
            0.146854, 0.172482, 0.206499, 0.243947, 0.289077,
            0.344711, 0.389921, 0.443541, 0.477663, 0.523627,
            0.575922, 0.621210, 0.681228, 0.746795, 0.814521,
            0.875968, 0.927614, 0.972332, 1.00028};

        // DeltaCn_beta
        std::vector<double> deltaCnBetaDueToVerticalTailSurface = {
            -0.000286658, -0.000248600, -0.000206470, -0.000155759, -0.000100533,
            -4.20949e-05, 4.68170e-06, 5.31830e-05, 8.40474e-05, 0.000125218,
            0.000165217, 0.000203544, 0.000243834, 0.000291375, 0.000333363,
            0.000370888, 0.000405228, 0.000429447, 0.000444584};

        // ============================================================
        // Rudder Effectiveness Data
        // Source: Plot Digitizer 2.6.8 - 17/03/2026
        // x = deflection angle [deg]
        // y = rudder effectiveness [-]
        // ============================================================

        // --- AR = 1.5 | cr/c = 0.30 ---
        std::vector<double> x_rudder_AR1_5_cr_c_0_30 = {
            4.77487, 9.54973, 14.2305, 18.9113, 24.0625, 28.4375};
        std::vector<double> y_rudder_AR1_5_cr_c_0_30 = {
            0.548023, 0.559322, 0.566586, 0.527845, 0.462470, 0.404358};

        // --- AR = 1.5 | cr/c = 0.37 ---
        std::vector<double> x_rudder_AR1_5_cr_c_0_37 = {
            4.51613, 9.73790, 14.7715, 20.3461, 25.3091, 30.8367};
        std::vector<double> y_rudder_AR1_5_cr_c_0_37 = {
            0.598870, 0.619855, 0.610977, 0.598870, 0.556901, 0.488297};

        // --- AR = 1.5 | cr/c = 0.45 ---
        std::vector<double> x_rudder_AR1_5_cr_c_0_45 = {
            5.17473, 9.97312, 14.7480, 21.0282, 25.4973, 30.6956};
        std::vector<double> y_rudder_AR1_5_cr_c_0_45 = {
            0.621469, 0.636804, 0.645682, 0.661017, 0.635997, 0.535109};

        // --- AR = 2.0 | cr/c = 0.30 ---
        std::vector<double> x_rudder_AR2_0_cr_c_0_30 = {
            4.79312, 9.54171, 14.3840, 18.9431, 24.0875, 28.4107};
        std::vector<double> y_rudder_AR2_0_cr_c_0_30 = {
            0.547725, 0.558788, 0.561788, 0.535793, 0.450946, 0.402399};

        // --- AR = 2.0 | cr/c = 0.37 ---
        std::vector<double> x_rudder_AR2_0_cr_c_0_37 = {
            4.48941, 9.70801, 14.8555, 20.2604, 25.3363, 30.8098};
        std::vector<double> y_rudder_AR2_0_cr_c_0_37 = {
            0.595282, 0.603905, 0.596416, 0.560712, 0.525829, 0.441773};

        // --- AR = 2.0 | cr/c = 0.45 ---
        std::vector<double> x_rudder_AR2_0_cr_c_0_45 = {
            5.19597, 10.0869, 14.7397, 20.9693, 25.4786, 30.7888};
        std::vector<double> y_rudder_AR2_0_cr_c_0_45 = {
            0.630705, 0.674799, 0.641546, 0.655764, 0.560472, 0.506238};

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
        DirectionalStabilityCalculator(
            BuildAircraft &builder,
            COG::COGDATA cogData,
            VSP::AeroSettings settings,
            VSP::Wing wing,
            VSP::Wing horizontalTail,
            VSP::Fuselage fuselage,
            VSP::Nacelle nacelle,
            VSP::Wing verticalTail = VSP::Wing(),
            VSP::Disk disk = VSP::Disk(),
            VSP::Wing canard = VSP::Wing()) : builder(builder),
                                              nameOfAircraft(nameOfAircraft),
                                              cogData(cogData),
                                              settings(settings),
                                              wing(wing),
                                              horizontalTail(horizontalTail),
                                              fuselage(fuselage),
                                              nacelle(nacelle),
                                              verticalTail(verticalTail),
                                              disk(disk),
                                              canard(canard) {}

        /**
         * @brief Destructor
         */
        ~DirectionalStabilityCalculator() = default;

        /**
         * @brief Main method to calculate all directional stability derivatives
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
        void calculateDirectionalStabilityDerivatives()
        {

            // ========================================================================
            // STEP 1: Calculate Wing Contribution to Directional Stability Derivative
            // ========================================================================
            // Swept wings contribute to directional stability due to asymmetric lift
            // distribution in sideslip. Contribution is proportional to sweep angle.

            if (builder.getCommonData().getIsSweptWing())
            {
                // Linear relationship: deltaCn/deltaBeta = 6e-6 * sweep_angle [deg]
                deltaCnDeltaBetaWingContribution = 6e-6 * std::pow(wing.sweepC4, 0.5);
            }
            else
            {
                deltaCnDeltaBetaWingContribution = 0.0; // Negligible contribution from non-swept wing
            }

            // Wing vertical position affects directional stability through
            // interaction with fuselage side force and flow field asymmetries
            if (builder.getCommonData().getWingPosition() == WingPosition::HIGH_WING)
            {
                // High wing provides positive contribution (stabilizing)
                deltaCnDeltaBetaDueToWingPosition = 0.0002;
            }
            else if (builder.getCommonData().getWingPosition() == WingPosition::MID_WING)
            {
                // Mid wing provides slight negative contribution (destabilizing)
                deltaCnDeltaBetaDueToWingPosition = -0.0001;
            }
            else
            {
                // Low wing - no additional contribution
                deltaCnDeltaBetaDueToWingPosition = 0.0;
            }

            // Roskam part VI - Side force contribution, from pag. 383 Given in 1/rda, so the conversion to 1/deg is obtained by dividing by 57.3
            deltaCyDeltaBetaWingContribution = -5.73e-3 * std::abs(wing.averageDihedral) / 57.3;

            // ========================================================================
            // STEP 2: Calculate Canard Contribution to Directional Stability Derivative
            // ========================================================================
            // Canard contribution follows same physics as wing - proportional to sweep

            if (canard.sweepC4 != 0.0 && canard.sweepC4 > 0.0)
            {
                // Same empirical relationship as wing
                deltaCnDeltaBetaCanardContribution = 6e-6 * std::pow(canard.sweepC4, 0.5);
            }
            else
            {
                deltaCnDeltaBetaCanardContribution = 0.0;
            }

            deltaCyDeltaBetaCanardContribution = -5.73e-3 * std::abs(canard.averageDihedral) / 57.3;

            // ========================================================================
            // STEP 3: Calculate Horizontal Tail Contribution to Directional Stability Derivative
            // ========================================================================
            // Horizontal tail contribution also proportional to sweep angle

            if (horizontalTail.sweepC4 != 0.0 && horizontalTail.sweepC4 > 0.0)
            {
                // Same empirical relationship as other lifting surfaces
                deltaCnDeltaBetaHorizontalTailContribution = 6e-6 * std::pow(horizontalTail.sweepC4, 0.5);
            }
            else
            {
                deltaCnDeltaBetaHorizontalTailContribution = 0.0;
            }

            deltaCyDeltaBetaHorizontalTailContribution = -5.73e-3 * std::abs(horizontalTail.averageDihedral) / 57.3;

            // ========================================================================
            // STEP 4: Calculate Fuselage Contribution to Directional Stability Derivative
            // ========================================================================
            // Uses Perkins empirical method based on fuselage geometry discretization
            // and empirical Kbeta factor from wind tunnel data

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

            // Calculate wing reference coordinates for fuselage discretization
            xTEWing = wing.xloc + wing.croot.front(); // Wing trailing edge X-coordinate

            // Convert wing MAC to feet for empirical formulas
            ConvLength meterToFtConverterXTEWing(Length::M, Length::FT, wing.MAC);
            wingMACFeet = meterToFtConverterXTEWing.getConvertedValues();

            // Calculate tail arm (distance from wing TE to horizontal tail quarter-chord)
            distanceFromeTEWingToQuarterChordMacHorizontal = (horizontalTail.xloc + horizontalTail.deltaXtoLEMAC + 0.25 * horizontalTail.MAC) - xTEWing;

            // Convert tail arm to feet
            ConvLength meterToFtConverterDistanceTEWingToQuarterChordHorizontal(Length::M, Length::FT, distanceFromeTEWingToQuarterChordMacHorizontal);

            distanceFromeTEWingToQuarterChordMacHorizontal = meterToFtConverterDistanceTEWingToQuarterChordHorizontal.getConvertedValues();

            // Calculate dynamic pressure for normalization
            dynamicPressureFreeStream = 0.5 * settings.rho * std::pow(settings.Vinf, 2.0);

            // Convert to pounds per square foot (imperial units for empirical formulas)
            ConvPressure pressureConverter(Pressure::PASCALS, Pressure::PSF, dynamicPressureFreeStream);
            psfDynamicPressure = pressureConverter.getConvertedValues();

            // Convert wing root chord to feet
            ConvLength meterToFtConverterRootChordWing(Length::M, Length::FT, wing.croot.front());
            rootChordFeet = meterToFtConverterRootChordWing.getConvertedValues();

            // Convert wing planform area to square feet
            ConvArea areaConverter(Area::SQUARE_METER, Area::SQUARE_FEET, wing.planformArea);
            planformWingAreaSquareFeet = areaConverter.getConvertedValues();

            // Iterate through geometry components to find and process fuselage

            for (const auto &nameOfTheComponent : allGeomData.nameOfComponentGeom)
            {

                if (nameOfTheComponent == fuselage.id)
                {

                    for (const auto &geomName : allGeomData.nameGeom)
                    {
                        // Handle custom fuselage geometry (transport-style fuselage)
                        if (geomName == "Custom")
                        {
                            // Find the last nose section (before wing leading edge)
                            for (size_t i = 0; i < fuseData.xStation.size(); i++)
                            {
                                if (fuseData.xStation[i] <= wing.xloc)
                                {
                                    idxNoseEnd = i; // Continue updating while before wing LE
                                    xStationNose.push_back(fuseData.xStation[i]);
                                }
                                else
                                {
                                    break; // Stop when we pass wing LE
                                }
                            }

                            maxFuselageDepth = *std::max_element(fuseData.allFuselageWidth.begin(), fuseData.allFuselageWidth.end());

                            // Find the first tail section (after wing trailing edge)
                            for (size_t i = 0; i < fuseData.xStation.size(); i++)
                            {
                                if (fuseData.xStation[i] >= xTEWing)
                                {
                                    idxTailStart = i;
                                    break;
                                }
                            }

                            // ========== FRONT SECTION (NOSE TO WING LE) ==========

                            // 1. Add nose sections up to wing LE
                            for (int i = 0; i <= idxNoseEnd; i++)
                            {
                                if (i == 0)
                                {
                                    // First station: use absolute X-coordinate
                                    xCoordinatePerkinsFront.push_back(fuseData.xStation[i]);
                                    diametersFront.push_back(fuseData.allFuselageWidth[i]);

                                    // ConvLength meterToFtConverterDiamFront(Length::M, Length::FT, diametersFront[i]);

                                    // diametersFront[i] = meterToFtConverterDiamFront.getConvertedValues();

                                    // ConvLength meterToFtConverterXFront(Length::M, Length::FT, xCoordinatePerkinsFront[i]);

                                    // xCoordinatePerkinsFront[i] = meterToFtConverterXFront.getConvertedValues();

                                    // Accumulate side projected area
                                    sideFuselageArea += xCoordinatePerkinsFront[i] * diametersFront[i];
                                }
                                else
                                {
                                    // Subsequent stations: use incremental delta X
                                    xCoordinatePerkinsFront.push_back(fuseData.xStation[i] - fuseData.xStation[i - 1]);
                                    diametersFront.push_back(fuseData.allFuselageWidth[i]);
                                    // x1CentroidDistancesFront.push_back(wing.xloc - (fuseData.xStation[i] + fuseData.xStation[i - 1]) / 2.0);

                                    // ConvLength meterToFtConverterDiamFront(Length::M, Length::FT, diametersFront[i]);
                                    // diametersFront[i] = meterToFtConverterDiamFront.getConvertedValues();

                                    // ConvLength meterToFtConverterXFront(Length::M, Length::FT, xCoordinatePerkinsFront[i]);
                                    // xCoordinatePerkinsFront[i] = meterToFtConverterXFront.getConvertedValues();

                                    // ConvLength meterToFtConverterX1Front(Length::M, Length::FT, x1CentroidDistancesFront[i]);
                                    // x1CentroidDistancesFront[i] = meterToFtConverterX1Front.getConvertedValues();

                                    // Interpolant interpGradient(x_A_curve, y_A_curve, 2, RegressionMethod::POLYNOMIAL);

                                    // gradientToLocalFlowAngleDueToUpWash = interpGradient.getYValueFromRegression(x1CentroidDistancesFront[i] / rootChordFeet);

                                    // summationFuselageContribution += std::pow(diametersFront[i], 2.0) * gradientToLocalFlowAngleDueToUpWash * (xCoordinatePerkinsFront[i]);

                                    sideFuselageArea += xCoordinatePerkinsFront[i] * diametersFront[i];
                                }
                            }

                            // 2. Subdivide space between end of nose and wing LE (front cabin section)

                            distanceNoseToWing = wing.xloc - fuseData.xStation[idxNoseEnd];
                            stepFront = distanceNoseToWing / numSubdivisionsFront;

                            // ConvLength meterToFtConverterDistanceNoseToWing(Length::M, Length::FT, distanceNoseToWing);
                            // distanceNoseToWing = meterToFtConverterDistanceNoseToWing.getConvertedValues();

                            // ConvLength meterToFtConverterStepFront(Length::M, Length::FT, stepFront);
                            // stepFrontFeet = meterToFtConverterStepFront.getConvertedValues();

                            for (int i = 0; i < numSubdivisionsFront; i++)
                            {
                                xCoordinatePerkinsFront.push_back(stepFront);
                                diametersFront.push_back(fuselage.diameter); // Constant diameter = max diameter

                                sideFuselageArea += xCoordinatePerkinsFront.back() * diametersFront.back();
                            }

                            // ========== REAR SECTION (WING TE TO TAIL) ==========

                            // 1. Subdivide space between wing TE and tail start (rear cabin section)

                            distanceWingToTail = fuseData.xStation[idxTailStart] - xTEWing;

                            // ConvLength meterToFtConverterDistanceWingToTail(Length::M, Length::FT, distanceWingToTail);
                            // distanceWingToTail = meterToFtConverterDistanceWingToTail.getConvertedValues();

                            stepRear = distanceWingToTail / numSubdivisionsRear;

                            ConvLength meterToFtConverterStepRear(Length::M, Length::FT, stepRear);
                            stepRearFeet = meterToFtConverterStepRear.getConvertedValues();

                            for (int i = 0; i < numSubdivisionsRear; i++)
                            {
                                xCoordinatePerkinsRear.push_back(stepRear);
                                diametersRear.push_back(fuselage.diameter); // Constant diameter = max diameter

                                sideFuselageArea += xCoordinatePerkinsRear.back() * diametersRear.back();
                            }

                            // 2. Add tail sections from tail start to tail end
                            for (size_t i = idxTailStart; i < (fuseData.xStation.size() - 1); i++)
                            {
                                if (i == idxTailStart)
                                {
                                    xCoordinatePerkinsRear.push_back(fuseData.xStation[i + 1] - fuseData.xStation[i]);
                                    diametersRear.push_back(fuseData.allFuselageWidth[i]);
                                }
                                else
                                {
                                    xCoordinatePerkinsRear.push_back(fuseData.xStation[i + 1] - fuseData.xStation[i]);
                                    diametersRear.push_back(fuseData.allFuselageWidth[i]);
                                }

                                xStationTail.push_back(fuseData.xStation[i]);

                                sideFuselageArea += xCoordinatePerkinsRear.back() * diametersRear.back();
                            }

                            // Initialize 2D interpolator for Kbeta factor using power regression
                            // Interpolates in 2D space: (COG position, fineness ratio) -> Kbeta
                            Interpolant2D kBetafCatorInterpolator(1, RegressionMethod::POWER);

                            // Add all empirical curves from Perkins charts for different fineness ratios
                            kBetafCatorInterpolator.addCurve(2.5, Kbeta_x_2_5, Kbeta_y_2_5);
                            kBetafCatorInterpolator.addCurve(3.0, Kbeta_x_3, Kbeta_y_3);
                            kBetafCatorInterpolator.addCurve(4.0, Kbeta_x_4, Kbeta_y_4);
                            kBetafCatorInterpolator.addCurve(5.0, Kbeta_x_5, Kbeta_y_5);
                            kBetafCatorInterpolator.addCurve(6.0, Kbeta_x_6, Kbeta_y_6);
                            kBetafCatorInterpolator.addCurve(7.0, Kbeta_x_7, Kbeta_y_7);
                            kBetafCatorInterpolator.addCurve(8.0, Kbeta_x_8, Kbeta_y_8);
                            kBetafCatorInterpolator.addCurve(10.0, Kbeta_x_10, Kbeta_y_10);

                            // Interpolate Kbeta for current fuselage (COG position ratio, fineness ratio)
                            kBetaFactorFuselage = kBetafCatorInterpolator.interpolate(cogData.xCG / fuselage.length, fuselage.length / fuselage.diameter);

                            // Extract nose section diameters for interpolation
                            dimatersNose = std::vector<double>(diametersFront.begin(), diametersFront.begin() + idxNoseEnd + 1);

                            // Create linear interpolator for nose diameters
                            Interpolant diamterFuselage(xStationNose, dimatersNose, 1, RegressionMethod::LINEAR);

                            // Get fuselage diameter at 1/4 length (needed for Perkins formula)
                            fuselageDiamterAtQuarterLength = diamterFuselage.getYValueFromRegression(0.25 * fuselage.length);

                            // Extract tail section diameters for interpolation
                            diametersTail = std::vector<double>(fuseData.allFuselageWidth.begin() + idxTailStart, fuseData.allFuselageWidth.end());

                            // Create linear interpolator for tail diameters
                            Interpolant diamterFuselageTail(xStationTail, diametersTail, 1, RegressionMethod::LINEAR);

                            // Get fuselage diameter at 3/4 length (needed for Perkins formula)
                            fuselageDiameterAtThreeQuarterLength = diamterFuselageTail.getYValueFromRegression(0.75 * fuselage.length);

                            Interpolant diamterFuselageQuarterLength(xStationNose, dimatersNose, 1, RegressionMethod::LINEAR);

                            fuselageWidthAtQuarterLength = diamterFuselageQuarterLength.getYValueFromRegression(0.25 * fuselage.length);

                            Interpolant diamterFuselageThreeQuarterLength(xStationTail, diametersTail, 1, RegressionMethod::LINEAR);

                            fuselageWidthAtThreeQuarterLength = diamterFuselageThreeQuarterLength.getYValueFromRegression(0.75 * fuselage.length);

                            ConvArea sideAreaConverter(Area::SQUARE_METER, Area::SQUARE_FEET, sideFuselageArea);
                            sideFuselageAreaFeetSquare = sideAreaConverter.getConvertedValues();

                            ConvLength meterToFtConverterFuselageLength(Length::M, Length::FT, fuselage.length);
                            fuselageLengthFeet = meterToFtConverterFuselageLength.getConvertedValues();

                            ConvLength meterToFtSpanConverter(Length::M, Length::FT, wing.totalProjectedSpan);
                            wingSpanFeet = meterToFtSpanConverter.getConvertedValues();

                            ConvLength meterToFtDiameterQuarterFuselageLengthConverter(Length::M, Length::FT, fuselageWidthAtQuarterLength);
                            fuselageDiamterAtQuarterLengthFeet = meterToFtDiameterQuarterFuselageLengthConverter.getConvertedValues();

                            ConvLength meterToFtDiameterThreeQuarterFuselageLengthConverter(Length::M, Length::FT, fuselageWidthAtThreeQuarterLength);
                            fuselageDiameterAtThreeQuarterLengthFeet = meterToFtDiameterThreeQuarterFuselageLengthConverter.getConvertedValues();

                            ConvLength meterToFtWidthAtQuarterFuselageLengthConverter(Length::M, Length::FT, fuselageWidthAtQuarterLength);
                            fuselageWidthAtQuarterLength = meterToFtWidthAtQuarterFuselageLengthConverter.getConvertedValues();

                            ConvLength meterToFtWidthAtThreeQuarterFuselageLengthConverter(Length::M, Length::FT, fuselageWidthAtThreeQuarterLength);
                            fuselageWidthAtThreeQuarterLength = meterToFtWidthAtThreeQuarterFuselageLengthConverter.getConvertedValues();

                            // Perkins empirical formula for fuselage directional stability contribution
                            // Negative sign indicates destabilizing effect (typical for fuselages)
                            // 57.3 converts degrees to radians, 0.96 is empirical constant
                            deltaCnDeltaBetaFuselageContribution = -(0.96 * kBetaFactorFuselage / 57.3) * (sideFuselageAreaFeetSquare / planformWingAreaSquareFeet) *
                                                                   (fuselageLengthFeet / wingSpanFeet) * std::pow((fuselageDiamterAtQuarterLengthFeet / fuselageDiameterAtThreeQuarterLengthFeet), 0.5) *
                                                                   std::pow((fuselageWidthAtQuarterLength / fuselageWidthAtThreeQuarterLength), 0.33);

                            break;

                        } // End of Custom fuselage case

                        // Handle standard fuselage or stack geometry
                        else if ((geomName == "Fuselage" || geomName == "Stack"))
                        {
                            // Find the last nose section (before wing leading edge)
                            for (size_t i = 0; i < fuseData.xStation.size(); i++)
                            {
                                if (fuseData.xStation[i] <= wing.xloc)
                                    idxNoseEnd = i; // Continue updating while before wing LE
                                else
                                    break; // Stop when we pass wing LE
                            }

                            // Find the first tail section (after wing trailing edge)
                            for (size_t i = 0; i < fuseData.xStation.size(); i++)
                            {
                                if (fuseData.xStation[i] >= xTEWing)
                                {
                                    idxTailStart = i;
                                    break;
                                }
                            }

                            maxFuselageDepth = *std::max_element(fuseData.allFuselageHeight.begin(), fuseData.allFuselageHeight.end());

                            // ========== FRONT SECTION (NOSE TO WING LE) ==========

                            // 1. Add nose sections (use height for standard fuselage)
                            for (int i = 0; i <= idxNoseEnd; i++)
                            {
                                if (i == 0)
                                {
                                    xCoordinatePerkinsFront.push_back(fuseData.xStation[i]);
                                    diametersFront.push_back(fuseData.allFuselageHeight[i]);

                                    sideFuselageArea += xCoordinatePerkinsFront[i] * diametersFront[i];
                                }
                                else if (i < idxNoseEnd)
                                {
                                    xCoordinatePerkinsFront.push_back(fuseData.xStation[i] - fuseData.xStation[i - 1]);
                                    diametersFront.push_back(fuseData.allFuselageHeight[i]);

                                    sideFuselageArea += xCoordinatePerkinsFront.back() * diametersFront.back();
                                }

                                else
                                {

                                    xCoordinatePerkinsFront.push_back(fuseData.xStation[i] - fuseData.xStation[i - 1]);
                                    diametersFront.push_back(fuseData.allFuselageHeight[i]);
                                    sideFuselageArea += xCoordinatePerkinsFront.back() * diametersFront.back();
                                }
                            }

                            // ========== REAR SECTION (WING TE TO TAIL) ==========

                            // Calculate distance from wing TE to first tail section
                            distanceWingToTail = fuseData.xStation[idxTailStart] - xTEWing;

                            for (size_t i = idxTailStart; i < (fuseData.xStation.size() - 1); i++)
                            {
                                if (i == idxTailStart)
                                {
                                    xCoordinatePerkinsRear.push_back(fuseData.xStation[i + 1] - fuseData.xStation[i]);
                                    diametersRear.push_back(fuseData.allFuselageHeight[i]);
                                    sideFuselageArea += xCoordinatePerkinsRear.back() * diametersRear.back();
                                }
                                else
                                {
                                    xCoordinatePerkinsRear.push_back(fuseData.xStation[i + 1] - fuseData.xStation[i]);
                                    diametersRear.push_back(fuseData.allFuselageHeight[i]);
                                    sideFuselageArea += xCoordinatePerkinsRear.back() * diametersRear.back();
                                }
                            }

                            break;
                        }

                    } // Fine del for interno

                    break;
                }
            }

            //========================================================================
            // STEP 4.1: Calculate Fuselage Side Force Contribution to Directional Stability Derivative
            //========================================================================

            xPointWhereFlowCasesToBePotentialToFuselageLengthRatio = 0.378 + 0.527 * averageXPointWhereFuselageReachesMaximumNegativeSlope; // The xO/lf ratio of Roskam
            xPointWhereFlowCasesToBePotential = xPointWhereFlowCasesToBePotentialToFuselageLengthRatio * fuselage.length;

            // Calculate the cross section area from xO point described by Roskam Part VI
            std::map<int, double> deltaXFromXPointWhereFlowCasesToBePotentialMap;

            for (size_t j = 0; j < fuseData.xStation.size() - 1; j++)
            {
                if (fuseData.xStation[j] / fuselage.length > xPointWhereFlowCasesToBePotentialToFuselageLengthRatio)
                {

                    bool alreadyDone = deltaXFromXPointWhereFlowCasesToBePotentialMap.count(indexFoundWhereFlowCasesToBePotential) > 0;

                    if (!alreadyDone)
                    {
                        deltaXFromXPointWhereFlowCasesToBePotential.push_back(fuseData.xStation[j] - xPointWhereFlowCasesToBePotential);
                        deltaXFromXPointWhereFlowCasesToBePotentialMap[j] = fuseData.xStation[j] - xPointWhereFlowCasesToBePotential;

                        xStationToInterpolateDiamterWhereFlowCasesToBePotential.push_back(fuseData.xStation[j]);

                        indexFoundWhereFlowCasesToBePotential = j;
                    }

                    else
                    {

                        deltaXFromXPointWhereFlowCasesToBePotential.push_back(fuseData.xStation[j + 1] - fuseData.xStation[j]);
                        xStationToInterpolateDiamterWhereFlowCasesToBePotential.push_back(fuseData.xStation[j]);
                    }

                    for (const auto &nameOfTheComponentFuselage : allGeomData.nameOfComponentGeom)
                    {
                        if (nameOfTheComponentFuselage == fuselage.id)
                        {
                            for (auto &nameFuselageGeom : allGeomData.nameGeom)
                            {

                                if (nameFuselageGeom == "Custom")
                                {
                                    diametersFromXPointWhereFlowCasesToBePotential.push_back(fuseData.allFuselageWidth[j]);
                                    // crossSectionAreaFromXPointWhereFlowCasesToBePotential += diametersFromXPointWhereFlowCasesToBePotential.back() * deltaXFromXPointWhereFlowCasesToBePotential.back();

                                    break;
                                }
                                else if ((nameFuselageGeom == "Fuselage" || nameFuselageGeom == "Stack"))
                                {
                                    diametersFromXPointWhereFlowCasesToBePotential.push_back(fuseData.allFuselageHeight[j]);
                                    // crossSectionAreaFromXPointWhereFlowCasesToBePotential += diametersFromXPointWhereFlowCasesToBePotential.back() * deltaXFromXPointWhereFlowCasesToBePotential.back();

                                    break;
                                }
                            }
                        }
                    }
                }

                else if (fuseData.xStation[j] / fuselage.length == xPointWhereFlowCasesToBePotentialToFuselageLengthRatio)
                {

                    deltaXFromXPointWhereFlowCasesToBePotential.push_back(fuseData.xStation[j + 1] - fuseData.xStation[j]);

                    for (const auto &nameOfTheComponentFuselage : allGeomData.nameOfComponentGeom)
                    {
                        if (nameOfTheComponentFuselage == fuselage.id)
                        {
                            for (auto &nameFuselageGeom : allGeomData.nameGeom)
                            {

                                if (nameFuselageGeom == "Custom")
                                {
                                    diametersFromXPointWhereFlowCasesToBePotential.push_back(fuseData.allFuselageWidth[j]);
                                    // crossSectionAreaFromXPointWhereFlowCasesToBePotential += diametersFromXPointWhereFlowCasesToBePotential.back() * deltaXFromXPointWhereFlowCasesToBePotential.back();
                                }
                                else if ((nameFuselageGeom == "Fuselage" || nameFuselageGeom == "Stack"))
                                {
                                    diametersFromXPointWhereFlowCasesToBePotential.push_back(fuseData.allFuselageHeight[j]);
                                    widthsFromXPointWhereFlowCasesToBePotential.push_back(fuseData.allFuselageWidth[j]);

                                    break;
                                }
                            }

                            break;
                        }
                    }

                    break;
                }
            }

            for (const auto &nameOfTheComponentFuselage : allGeomData.nameOfComponentGeom)
            {

                if (nameOfTheComponentFuselage == fuselage.id)
                {

                    for (auto &nameFuselageGeom : allGeomData.nameGeom)
                    {

                        if (nameFuselageGeom == "Custom")
                        {

                            Interpolant diamterFuselageFromXPointWhereFlowCasesToBePotential(xStationToInterpolateDiamterWhereFlowCasesToBePotential, diametersFromXPointWhereFlowCasesToBePotential, 1, RegressionMethod::LINEAR);

                            diameterAtXPointWhereFlowCasesToBePotential = diamterFuselageFromXPointWhereFlowCasesToBePotential.getYValueFromRegression(xPointWhereFlowCasesToBePotential);

                            crossSectionAreaFromXPointWhereFlowCasesToBePotential = M_PI * std::pow(diameterAtXPointWhereFlowCasesToBePotential / 2.0, 2.0);

                            break;
                        }
                        else if ((nameFuselageGeom == "Fuselage" || nameFuselageGeom == "Stack"))
                        {

                            crossSectionAreaFromXPointWhereFlowCasesToBePotential = M_PI * diametersFromXPointWhereFlowCasesToBePotential.back() * widthsFromXPointWhereFlowCasesToBePotential.back() / 4.0; // Approximating cross section as ellipse with height and width from VSP data

                            break;
                        }
                    }

                    break;
                }
            }

            // Calculate Ki factor due to wing position for side force derivative
            switch (builder.getWingData().getWingPosition())
            {

            case WingPosition::HIGH_WING:

            {

                Interpolant factorKiInterpolatorFuselage(Ki_x_high_wing, Ki_y_high_wing, 1, RegressionMethod::LINEAR);

                factorKiToSideForceFuselageContirbution = factorKiInterpolatorFuselage.getYValueFromRegression(-2 * wing.zloc / fuselage.diameter);
            }
            break;

            case WingPosition::LOW_WING:

            {

                Interpolant factorKiInterpolatorFuselage(Ki_x_low_wing, Ki_y_low_wing, 1, RegressionMethod::LINEAR);

                factorKiToSideForceFuselageContirbution = factorKiInterpolatorFuselage.getYValueFromRegression(2 * std::abs(wing.zloc) / fuselage.diameter);
            }

            break;
            }

            deltaCyDeltaBetaFuselageContribution = -(2 * factorKiToSideForceFuselageContirbution * (crossSectionAreaFromXPointWhereFlowCasesToBePotential / wing.planformArea)) / 57.3;

            // ========================================================================
            // STEP 5: Calculate Propeller Contribution to Directional Stability, if activated
            // ========================================================================
            // Windmilling propeller creates asymmetric side force in sideslip
            // Contribution depends on number of blades and propeller location relative to COG

            if (settings.IncludePropToAnlysis == "Yes")
            {

                // Two-bladed propeller coefficients
                if (builder.getEngineData().getNumberOfBlades() == 2)
                {
                    // Sign depends on propeller location: forward of COG is negative
                    if (nacelle.xloc.front() < cogData.xCG)
                    {
                        deltaCyDeltaBetaWindMillingPropeller = -0.00165;
                    }
                    else
                    {
                        deltaCyDeltaBetaWindMillingPropeller = 0.00165;
                    }
                }

                // Three-bladed propeller coefficients (higher than 2-blade)
                else if (builder.getEngineData().getNumberOfBlades() == 3)
                {
                    if (nacelle.xloc.front() < cogData.xCG)
                    {
                        deltaCyDeltaBetaWindMillingPropeller = -0.00235;
                    }
                    else
                    {
                        deltaCyDeltaBetaWindMillingPropeller = 0.00235;
                    }
                }

                // Four-bladed propeller coefficients
                else if (builder.getEngineData().getNumberOfBlades() == 4)
                {
                    if (nacelle.xloc.front() < cogData.xCG)
                    {
                        deltaCyDeltaBetaWindMillingPropeller = -0.00296;
                    }
                    else
                    {
                        deltaCyDeltaBetaWindMillingPropeller = 0.00296;
                    }
                }

                // Six-bladed propeller coefficients (highest)
                else if (builder.getEngineData().getNumberOfBlades() == 6)
                {
                    if (nacelle.xloc.front() < cogData.xCG)
                    {
                        deltaCyDeltaBetaWindMillingPropeller = -0.00510;
                    }
                    else
                    {
                        deltaCyDeltaBetaWindMillingPropeller = 0.00510;
                    }
                }

                // Calculate moment arm from propeller disk to COG
                distanceFromPropellerDiskToCOG = nacelle.xloc.front() - cogData.xCG;

                // Calculate propeller yaw moment contribution using empirical formula
                // Includes propeller disk area, moment arm, and number of engines
                deltaCnDeltaBetaPropeller = 1.5 * (M_PI * std::pow(disk.diameter.front(), 2.0) * distanceFromPropellerDiskToCOG * deltaCyDeltaBetaWindMillingPropeller * builder.getEngineData().getNumberOfEngines()) /
                                            (4 * wing.planformArea * wing.totalProjectedSpan);
            }

            // ========================================================================
            // STEP 6: Calculate Nacelle Contribution to Directional Stability Derivative
            // ========================================================================
            // Similar approach to fuselage, using Perkins method with Kbeta factors

            // Extract nacelle diameter distribution
            VSPGEOMTRYEXTRACTOR::DiametersExtractor nacelleExtractor;

            VSPGEOMTRYEXTRACTOR::NacelleDiametersAndXStation nacelleData = nacelleExtractor.extractNacelleDiameters(
                builder.getCommonData().getNameOfAircraft(),
                allGeomData,
                nacelle.length);

            // Discretize nacelle geometry for Perkins method
            for (size_t j = 0; j < nacelleData.xStation.size(); j++)
            {
                if (j == 0)
                {
                    // First station: use absolute X-coordinate
                    deltaXNacellePerkinsFront.push_back(nacelleData.xStation[j]);
                    diametersNacelleFront.push_back(nacelleData.allNacelleHeights[j]);
                    nacelleWidth.push_back(nacelleData.allNacelleWidth[j]);

                    // Accumulate nacelle side projected area
                    sideAreaNacelle += deltaXNacellePerkinsFront.back() * diametersNacelleFront.back();
                }
                else
                {
                    // Subsequent stations: use incremental delta X
                    deltaXNacellePerkinsFront.push_back(nacelleData.xStation[j] - nacelleData.xStation[j - 1]);
                    diametersNacelleFront.push_back(nacelleData.allNacelleHeights[j]);
                    nacelleWidth.push_back(nacelleData.allNacelleWidth[j]);
                    sideAreaNacelle += deltaXNacellePerkinsFront.back() * diametersNacelleFront.back();
                }
            }

            // Initialize 2D interpolator for nacelle Kbeta factor
            Interpolant2D kBetaFactorInterpolator(1, RegressionMethod::POWER);

            // Add all empirical curves from Perkins charts
            kBetaFactorInterpolator.addCurve(2.5, Kbeta_x_2_5, Kbeta_y_2_5);
            kBetaFactorInterpolator.addCurve(3.0, Kbeta_x_3, Kbeta_y_3);
            kBetaFactorInterpolator.addCurve(4.0, Kbeta_x_4, Kbeta_y_4);
            kBetaFactorInterpolator.addCurve(5.0, Kbeta_x_5, Kbeta_y_5);
            kBetaFactorInterpolator.addCurve(6.0, Kbeta_x_6, Kbeta_y_6);
            kBetaFactorInterpolator.addCurve(7.0, Kbeta_x_7, Kbeta_y_7);
            kBetaFactorInterpolator.addCurve(8.0, Kbeta_x_8, Kbeta_y_8);
            kBetaFactorInterpolator.addCurve(10.0, Kbeta_x_10, Kbeta_y_10);

            // Interpolate Kbeta for current nacelle geometry
            kBetaFactorNacelle = kBetaFactorInterpolator.interpolate(cogData.xCG / nacelle.length, nacelle.length / nacelle.aDiameter);

            // Extract nacelle geometry data for quarter and three-quarter length positions
            // These are needed for the Perkins formula diameter ratio term
            diamtersToQuarterLengthNacelleDiameter = std::vector<double>(diametersNacelleFront.begin() + 1, diametersNacelleFront.end() - 3);

            diamtersToThreeQuarterLengthNacelleDiameter = std::vector<double>(diametersNacelleFront.begin() + 2, diametersNacelleFront.end());

            nacelleStationToQuarterLengthNacelleDiamter = std::vector<double>(nacelleData.xStation.begin() + 1, nacelleData.xStation.end() - 3);

            nacelleStationToThreeQuarterLengthNacelleDiamter = std::vector<double>(nacelleData.xStation.begin() + 2, nacelleData.xStation.end());

            // Create interpolators for nacelle diameters
            Interpolant diamterNacelleQuarterLength(nacelleStationToQuarterLengthNacelleDiamter, diamtersToQuarterLengthNacelleDiameter, 1, RegressionMethod::LINEAR);
            Interpolant diamterNacelleThreeQuarterLength(nacelleStationToThreeQuarterLengthNacelleDiamter, diamtersToThreeQuarterLengthNacelleDiameter, 1, RegressionMethod::LINEAR);

            // Interpolate nacelle diameters at 1/4 and 3/4 length positions
            nacelleDiamterAtQuarterLength = diamterNacelleQuarterLength.getYValueFromRegression(0.25 * nacelle.length);
            nacelleDiameterAtThreeQuarterLength = diamterNacelleThreeQuarterLength.getYValueFromRegression(0.75 * nacelle.length);

            // Extract and interpolate nacelle widths at quarter and three-quarter positions
            nacelleWidthToQuarterLengthNacelle = std::vector<double>(nacelleWidth.begin() + 1, nacelleWidth.end() - 3);
            nacelleWidthToThreeQuarterLengthNacelle = std::vector<double>(nacelleWidth.begin() + 2, nacelleWidth.end());

            // Create interpolators for nacelle widths
            Interpolant widthNacelleQuarterLength(nacelleStationToQuarterLengthNacelleDiamter, nacelleWidthToQuarterLengthNacelle, 1, RegressionMethod::LINEAR);
            Interpolant widthNacelleThreeQuarterLength(nacelleStationToThreeQuarterLengthNacelleDiamter, nacelleWidthToThreeQuarterLengthNacelle, 1, RegressionMethod::LINEAR);

            // Interpolate nacelle widths at 1/4 and 3/4 length positions
            nacelleWidthAtQuarterLength = widthNacelleQuarterLength.getYValueFromRegression(0.25 * nacelle.length);
            nacelleWidthAtThreeQuarterLength = widthNacelleThreeQuarterLength.getYValueFromRegression(0.75 * nacelle.length);

            // Perkins empirical formula for nacelle directional stability contribution
            // Same form as fuselage equation, scaled by nacelle dimensions
            // Negative sign indicates typically destabilizing contribution
            deltaCnDeltaBetaNacelleContribution = -(0.96 * kBetaFactorNacelle / 57.3) * (sideAreaNacelle / wing.planformArea) *
                                                  (nacelle.length / wing.totalProjectedSpan) * std::pow((nacelleDiamterAtQuarterLength / nacelleDiameterAtThreeQuarterLength), 0.5) *
                                                  std::pow((nacelleWidthAtQuarterLength / nacelleWidthAtThreeQuarterLength), 0.33);

            // ========================================================================
            // STEP 7: Calculate Vertical Contribution to Directional Stability Derivative
            // ========================================================================

            for (size_t i = 0; i < fuseData.xStation.size(); i++)
            {

                if (fuseData.xStation[i] / fuselage.length > 0.65)
                {

                    xStationTailConeFuselage.push_back(fuseData.xStation[i]);

                    for (const auto &nameOfTheComponentFuselage : allGeomData.nameOfComponentGeom)
                    {

                        if (nameOfTheComponentFuselage == fuselage.id)
                        {
                            for (auto &nameFuselageGeom : allGeomData.nameGeom)
                            {

                                if ((nameFuselageGeom == "Custom"))
                                {
                                    diamterTailConeFuselage.push_back(fuseData.allFuselageWidth[i]);

                                    break;
                                }

                                else if ((nameFuselageGeom == "Fuselage" || nameFuselageGeom == "Stack"))
                                {

                                    diamterTailConeFuselage.push_back(fuseData.allFuselageHeight[i]);

                                    break;
                                }

                            } // Fine del for interno
                        }
                    }
                }
            }

            switch (builder.getCommonData().getTypeOfTail())
            {

            case TypeOfTail::CONVENTIONAL_TAIL:

            {

                Interpolant depthFuselageInterpolator(xStationTailConeFuselage, diamterTailConeFuselage, 1, RegressionMethod::LINEAR);

                fuselageDepthInTheVerticalLocation = depthFuselageInterpolator.getYValueFromRegression(verticalTail.xloc);

                spanVerticalToDepthFuselageRatio = verticalTail.totalSpan / fuselageDepthInTheVerticalLocation;

                // Detrmaine which curves to interpolate for the ratio of the aspect ratio related to the vertical tail in the presence of the fuselage

                if (verticalTail.taperRatio == 1)
                {

                    Interpolant ratioInterpolator(x_lambda1, y_lambda1, 5, RegressionMethod::POLYNOMIAL);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                else if (verticalTail.taperRatio < 1 && verticalTail.taperRatio > 0.6)
                {

                    Interpolant2D ratioInterpolator(5, RegressionMethod::POLYNOMIAL);

                    ratioInterpolator.addCurve(1.0, x_lambda1, y_lambda1);
                    ratioInterpolator.addCurve(0.6, x_lambda06, y_lambda06);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.interpolate(verticalTail.taperRatio, spanVerticalToDepthFuselageRatio);
                }

                else
                {

                    Interpolant ratioInterpolator(x_lambda06, y_lambda06, 5, RegressionMethod::POLYNOMIAL);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                // Detrmine the ratio of the aspect ratio of vertical tail in the presence of the horizontal

                xLocationOfTheAerodynamicCenterHorizontalTail = 0.25 * horizontalTail.MAC;
                relativePositionBetweenHorizontalAndVertical = xLocationOfTheAerodynamicCenterHorizontalTail / verticalTail.croot.front();

                if (horizontalTail.zloc > 0.0)
                {
                    zLocationHorizontalToSpanVerticalRatio = -std::abs(horizontalTail.zloc) / verticalTail.totalSpan;
                }

                else
                {

                    zLocationHorizontalToSpanVerticalRatio = std::abs(horizontalTail.zloc) / verticalTail.totalSpan;
                }

                Interpolant2D aspectRatioInThePresenceOfTheHorizontalTailInterpolator(3, RegressionMethod::POLYNOMIAL);

                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.5, x_xcv05, y_xcv05);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.6, x_xcv06, y_xcv06);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.7, x_xcv07, y_xcv07);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.8, x_xcv08, y_xcv08);

                ratioToAspectRatioVerticalInThePresenceOfTheHorizontal = aspectRatioInThePresenceOfTheHorizontalTailInterpolator.interpolate(zLocationHorizontalToSpanVerticalRatio, relativePositionBetweenHorizontalAndVertical);

                // Calculate Kvh factor Roskam Part VI page.390
                Interpolant kvhInterpolator(x_Kvh, y_Kvh, 3, RegressionMethod::POLYNOMIAL);

                factorKvh = kvhInterpolator.getYValueFromRegression(horizontalTail.planformArea / verticalTail.planformArea);

                // Calculate tail arm from vertical tail aerodynamic center to COG
                // Assume aerodynamic center is at quarter chord of vertical tail

                tailArmVerticalTail = (verticalTail.xloc + verticalTail.deltaXtoLEMAC + 0.25 * verticalTail.MAC) - cogData.xCG;

                equivalentVerticalTailAspectRatio = ratioToAspectRatioVerticalInThePresenceOfTheFuselage * verticalTail.aspectRatio *
                                                    (1 + factorKvh * (ratioToAspectRatioVerticalInThePresenceOfTheHorizontal - 1));

                Interpolant slopeVerticalTailInterpolator(equivalentAspectRatioVerticalTail, liftSlopeVerticalTail, 3, RegressionMethod::POLYNOMIAL);

                verticalTailLiftSlope = slopeVerticalTailInterpolator.getYValueFromRegression(equivalentVerticalTailAspectRatio);

                // DATCOM Method to estimate (1 + deltaSigma/deltaBeta) * etaV - Roskam part VI pag 389

                switch (builder.getWingData().getWingPosition())
                {

                case WingPosition::HIGH_WING:

                {

                    zWingQuarerRootChordPosition = -std::abs(wing.zloc);

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((verticalTail.planformArea / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) +
                                                                0.4 * (zWingQuarerRootChordPosition / maxFuselageDepth) + 0.009 * wing.aspectRatio;
                }

                break;

                case WingPosition::MID_WING:

                {

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((verticalTail.planformArea / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) +
                                                                +0.009 * equivalentVerticalTailAspectRatio;
                }

                break;

                case WingPosition::LOW_WING:

                {

                    zWingQuarerRootChordPosition = std::abs(wing.zloc);

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((verticalTail.planformArea / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) -
                                                                0.4 * (zWingQuarerRootChordPosition / maxFuselageDepth) + 0.009 * wing.aspectRatio;
                }
                }

                // Calculate dCn/dBeta
                deltaCnDeltaBetaVerticalTailContribution = verticalTailLiftSlope * sideWashGradientTimesDynamicPressureRatio * (verticalTail.planformArea / wing.planformArea) *
                                                           (tailArmVerticalTail / wing.totalSpan);

                // Calculate dCy/dBeta
                if (spanVerticalToDepthFuselageRatio >= 0.0 && spanVerticalToDepthFuselageRatio < 2.0)
                {

                    factorKvForSingleVerticalTail = 0.750;
                }

                else if (spanVerticalToDepthFuselageRatio >= 2.0 && spanVerticalToDepthFuselageRatio <= 3.5)
                {

                    Interpolant kvForSingleVerticalTailInterpolator(Kv, verticalTailSpanToFuselageDepthInRegionOfVerticalRatio, 1, RegressionMethod::LINEAR);

                    factorKvForSingleVerticalTail = kvForSingleVerticalTailInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                else if (spanVerticalToDepthFuselageRatio > 3.5)
                {

                    factorKvForSingleVerticalTail = 1.0;
                }

                // The original contribution is given 1/rad Roskam Part VI - Converted to 1/deg by dividing by 57.3
                deltaCyDeltaBetaVerticalTailContribution = -(factorKvForSingleVerticalTail * verticalTailLiftSlope *
                                                             sideWashGradientTimesDynamicPressureRatio * (verticalTail.planformArea / wing.planformArea)) /
                                                           57.3;

            } // Fine dello switch

            break;

            case TypeOfTail::T_TAIL:

            {

                Interpolant depthFuselageInterpolator(xStationTailConeFuselage, diamterTailConeFuselage, 1, RegressionMethod::LINEAR);

                fuselageDepthInTheVerticalLocation = depthFuselageInterpolator.getYValueFromRegression(verticalTail.xloc);

                spanVerticalToDepthFuselageRatio = verticalTail.totalSpan / fuselageDepthInTheVerticalLocation;

                // Detrmaine which curves to interpolate for the ratio of the aspect ratio related to the vertical tail in the presence of the fuselage

                if (verticalTail.taperRatio == 1)
                {

                    Interpolant ratioInterpolator(x_lambda1, y_lambda1, 5, RegressionMethod::POLYNOMIAL);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                else if (verticalTail.taperRatio < 1 && verticalTail.taperRatio > 0.6)
                {

                    Interpolant2D ratioInterpolator(5, RegressionMethod::POLYNOMIAL);

                    ratioInterpolator.addCurve(1.0, x_lambda1, y_lambda1);
                    ratioInterpolator.addCurve(0.6, x_lambda06, y_lambda06);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.interpolate(verticalTail.taperRatio, spanVerticalToDepthFuselageRatio);
                }

                else if (verticalTail.taperRatio <= 0.6)
                {

                    Interpolant ratioInterpolator(x_lambda06, y_lambda06, 5, RegressionMethod::POLYNOMIAL);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                // Detrmine the ratio of the aspect ratio of vertical tail in the presence of the horizontal

                xLocationOfTheAerodynamicCenterHorizontalTail = 0.25 * horizontalTail.MAC;
                relativePositionBetweenHorizontalAndVertical = xLocationOfTheAerodynamicCenterHorizontalTail / verticalTail.croot.front();
                if (horizontalTail.zloc > 0.0)
                {
                    zLocationHorizontalToSpanVerticalRatio = -std::abs(horizontalTail.zloc) / verticalTail.totalSpan;
                }

                else
                {

                    zLocationHorizontalToSpanVerticalRatio = std::abs(horizontalTail.zloc) / verticalTail.totalSpan;
                }

                Interpolant2D aspectRatioInThePresenceOfTheHorizontalTailInterpolator(3, RegressionMethod::POLYNOMIAL);

                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.5, x_xcv05, y_xcv05);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.6, x_xcv06, y_xcv06);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.7, x_xcv07, y_xcv07);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.8, x_xcv08, y_xcv08);

                ratioToAspectRatioVerticalInThePresenceOfTheHorizontal = aspectRatioInThePresenceOfTheHorizontalTailInterpolator.interpolate(zLocationHorizontalToSpanVerticalRatio, relativePositionBetweenHorizontalAndVertical);

                // Calculate Kvh factor Roskam Part VI page.390
                Interpolant kvhInterpolator(x_Kvh, y_Kvh, 3, RegressionMethod::POLYNOMIAL);

                factorKvh = kvhInterpolator.getYValueFromRegression(horizontalTail.planformArea / verticalTail.planformArea);

                // Calculate tail arm from vertical tail aerodynamic center to COG
                // Assume aerodynamic center is at quarter chord of vertical tail

                tailArmVerticalTail = (verticalTail.xloc + verticalTail.deltaXtoLEMAC + 0.25 * verticalTail.MAC) - cogData.xCG;

                equivalentVerticalTailAspectRatio = ratioToAspectRatioVerticalInThePresenceOfTheFuselage * verticalTail.aspectRatio *
                                                    (1 + factorKvh * (ratioToAspectRatioVerticalInThePresenceOfTheHorizontal - 1));

                Interpolant slopeVerticalTailInterpolator(equivalentAspectRatioVerticalTail, liftSlopeVerticalTail, 3, RegressionMethod::POLYNOMIAL);

                verticalTailLiftSlope = slopeVerticalTailInterpolator.getYValueFromRegression(equivalentVerticalTailAspectRatio);

                // DATCOM Method to estimate (1 + deltaSigma/deltaBeta) * etaV - Roskam part VI pag 389

                switch (builder.getWingData().getWingPosition())
                {

                case WingPosition::HIGH_WING:

                {

                    zWingQuarerRootChordPosition = -std::abs(wing.zloc);

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((verticalTail.planformArea / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) +
                                                                0.4 * (zWingQuarerRootChordPosition / maxFuselageDepth) + 0.009 * wing.aspectRatio;
                }

                break;

                case WingPosition::MID_WING:

                {

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((verticalTail.planformArea / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) +
                                                                +0.009 * equivalentVerticalTailAspectRatio;
                }

                break;

                case WingPosition::LOW_WING:

                {

                    zWingQuarerRootChordPosition = std::abs(wing.zloc);

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((verticalTail.planformArea / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) -
                                                                0.4 * (zWingQuarerRootChordPosition / maxFuselageDepth) + 0.009 * wing.aspectRatio;
                }
                }

                // Calculate dCn/dBeta
                deltaCnDeltaBetaVerticalTailContribution = verticalTailLiftSlope * sideWashGradientTimesDynamicPressureRatio * (verticalTail.planformArea / wing.planformArea) *
                                                           (tailArmVerticalTail / wing.totalSpan);

                // Calculate dCy/dBeta
                if (spanVerticalToDepthFuselageRatio >= 0.0 && spanVerticalToDepthFuselageRatio < 2.0)
                {

                    factorKvForSingleVerticalTail = 0.750;
                }

                else if (spanVerticalToDepthFuselageRatio >= 2.0 && spanVerticalToDepthFuselageRatio <= 3.5)
                {

                    Interpolant kvForSingleVerticalTailInterpolator(Kv, verticalTailSpanToFuselageDepthInRegionOfVerticalRatio, 1, RegressionMethod::LINEAR);

                    factorKvForSingleVerticalTail = kvForSingleVerticalTailInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                else if (spanVerticalToDepthFuselageRatio > 3.5)
                {

                    factorKvForSingleVerticalTail = 1.0;
                }

                // The original contribution is given 1/rad Roskam Part VI - Converted to 1/deg by dividing by 57.3
                deltaCyDeltaBetaVerticalTailContribution = -(factorKvForSingleVerticalTail * verticalTailLiftSlope *
                                                             sideWashGradientTimesDynamicPressureRatio * (verticalTail.planformArea / wing.planformArea)) /
                                                           57.3;
            } // Fine dello switch

            break;

            case TypeOfTail::V_TAIL:

            {

                projectedPlanformAreaToVTail = verticalTail.planformArea * std::pow(sin(verticalTail.averageDihedral / 57.3), 2.0);
                projectedSpanToVtail = 2 * verticalTail.totalSpan * std::sqrt(projectedPlanformAreaToVTail / verticalTail.planformArea);

                equivalentHorizontalTailArea = verticalTail.planformArea * std::pow(cos(verticalTail.averageDihedral / 57.3), 2.0);

                Interpolant depthFuselageInterpolator(xStationTailConeFuselage, diamterTailConeFuselage, 1, RegressionMethod::LINEAR);

                fuselageDepthInTheVerticalLocation = depthFuselageInterpolator.getYValueFromRegression(verticalTail.xloc);

                spanVerticalToDepthFuselageRatio = projectedSpanToVtail / fuselageDepthInTheVerticalLocation;

                // Determaine which curves to interpolate for the ratio of the aspect ratio related to the vertical tail in the presence of the fuselage

                if (verticalTail.taperRatio == 1)
                {

                    Interpolant ratioInterpolator(x_lambda1, y_lambda1, 5, RegressionMethod::POLYNOMIAL);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                else if (verticalTail.taperRatio < 1 && verticalTail.taperRatio > 0.6)
                {

                    Interpolant2D ratioInterpolator(5, RegressionMethod::POLYNOMIAL);

                    ratioInterpolator.addCurve(1.0, x_lambda1, y_lambda1);
                    ratioInterpolator.addCurve(0.6, x_lambda06, y_lambda06);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.interpolate(verticalTail.taperRatio, spanVerticalToDepthFuselageRatio);
                }

                else if (verticalTail.taperRatio <= 0.6)
                {

                    Interpolant ratioInterpolator(x_lambda06, y_lambda06, 5, RegressionMethod::POLYNOMIAL);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                // Determine the ratio of the aspect ratio of vertical tail in the presence of the horizontal

                xLocationOfTheAerodynamicCenterHorizontalTail = 0.25 * verticalTail.MAC;
                relativePositionBetweenHorizontalAndVertical = xLocationOfTheAerodynamicCenterHorizontalTail / verticalTail.croot.front();
                zLocationHorizontalToSpanVerticalRatio = 0.0; // Assumption : For V-tail, horizontal tail is at the bottom of the vertical tali, lied on the fuselage center line

                Interpolant2D aspectRatioInThePresenceOfTheHorizontalTailInterpolator(3, RegressionMethod::POLYNOMIAL);

                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.5, x_xcv05, y_xcv05);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.6, x_xcv06, y_xcv06);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.7, x_xcv07, y_xcv07);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.8, x_xcv08, y_xcv08);

                ratioToAspectRatioVerticalInThePresenceOfTheHorizontal = aspectRatioInThePresenceOfTheHorizontalTailInterpolator.interpolate(zLocationHorizontalToSpanVerticalRatio, relativePositionBetweenHorizontalAndVertical);

                // Calculate Kvh factor Roskam Part VI page.390
                Interpolant kvhInterpolator(x_Kvh, y_Kvh, 3, RegressionMethod::POLYNOMIAL);

                factorKvh = kvhInterpolator.getYValueFromRegression(equivalentHorizontalTailArea / projectedPlanformAreaToVTail);

                // Calculate tail arm from vertical tail aerodynamic center to COG
                // Assume aerodynamic center is at quarter chord of vertical tail

                tailArmVerticalTail = (verticalTail.xloc + verticalTail.deltaXtoLEMAC + 0.25 * verticalTail.MAC) - cogData.xCG;

                // Assumption : The aspect ratio of the Vee Tail is equal to the conventional one
                equivalentVerticalTailAspectRatio = ratioToAspectRatioVerticalInThePresenceOfTheFuselage * verticalTail.aspectRatio *
                                                    (1 + factorKvh * (ratioToAspectRatioVerticalInThePresenceOfTheHorizontal - 1));

                Interpolant slopeVerticalTailInterpolator(equivalentAspectRatioVerticalTail, liftSlopeVerticalTail, 3, RegressionMethod::POLYNOMIAL);

                verticalTailLiftSlope = slopeVerticalTailInterpolator.getYValueFromRegression(equivalentVerticalTailAspectRatio);

                // DATCOM Method to estimate (1 + deltaSigma/deltaBeta) * etaV - Roskam part VI pag 389

                switch (builder.getWingData().getWingPosition())
                {

                case WingPosition::HIGH_WING:

                {

                    zWingQuarerRootChordPosition = -std::abs(wing.zloc);

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((projectedPlanformAreaToVTail / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) +
                                                                0.4 * (zWingQuarerRootChordPosition / maxFuselageDepth) + 0.009 * wing.aspectRatio;
                }

                break;

                case WingPosition::MID_WING:

                {

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((projectedPlanformAreaToVTail / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) +
                                                                +0.009 * equivalentVerticalTailAspectRatio;
                }

                break;

                case WingPosition::LOW_WING:

                {

                    zWingQuarerRootChordPosition = std::abs(wing.zloc);

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((projectedPlanformAreaToVTail / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) -
                                                                0.4 * (zWingQuarerRootChordPosition / maxFuselageDepth) + 0.009 * wing.aspectRatio;
                }
                }

                // NACA Report n.823
                Interpolant2D correctionFactorVeeTailLiftSlopeInterpolator(1, RegressionMethod::LOGARITHMIC);

                correctionFactorVeeTailLiftSlopeInterpolator.addCurve(0.2, K_factor_x_lambda_025, K_factor_y_lambda_025);
                correctionFactorVeeTailLiftSlopeInterpolator.addCurve(0.5, K_factor_x_lambda_05, K_factor_y_lambda_05);
                correctionFactorVeeTailLiftSlopeInterpolator.addCurve(1.0, K_factor_x_lambda_1, K_factor_y_lambda_1);

                kFactorToLiftSlopeVeeTail = correctionFactorVeeTailLiftSlopeInterpolator.interpolate(verticalTail.aspectRatio, verticalTail.taperRatio);

                // Calculate dCn/dBeta
                deltaCnDeltaBetaVerticalTailContribution = kFactorToLiftSlopeVeeTail * verticalTailLiftSlope * sideWashGradientTimesDynamicPressureRatio * (projectedPlanformAreaToVTail / wing.planformArea) *
                                                           (tailArmVerticalTail / wing.totalSpan);

                // Calculate dCy/dBeta
                if (spanVerticalToDepthFuselageRatio >= 0.0 && spanVerticalToDepthFuselageRatio < 2.0)
                {

                    factorKvForSingleVerticalTail = 0.750;
                }

                else if (spanVerticalToDepthFuselageRatio >= 2.0 && spanVerticalToDepthFuselageRatio <= 3.5)
                {

                    Interpolant kvForSingleVerticalTailInterpolator(Kv, verticalTailSpanToFuselageDepthInRegionOfVerticalRatio, 1, RegressionMethod::LINEAR);

                    factorKvForSingleVerticalTail = kvForSingleVerticalTailInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                else if (spanVerticalToDepthFuselageRatio > 3.5)
                {

                    factorKvForSingleVerticalTail = 1.0;
                }

                // The original contribution is given 1/rad Roskam Part VI - Converted to 1/deg by dividing by 57.3
                deltaCyDeltaBetaVerticalTailContribution = -(factorKvForSingleVerticalTail * verticalTailLiftSlope *
                                                             sideWashGradientTimesDynamicPressureRatio * (projectedPlanformAreaToVTail / wing.planformArea)) /
                                                           57.3;
            } // Fine dello switch

            break;

            case TypeOfTail::V_REV_TAIL:

            {

                projectedPlanformAreaToVTail = verticalTail.planformArea * std::pow(cos(verticalTail.averageDihedral / 57.3), 2.0);
                projectedSpanToVtail = 2 * verticalTail.totalSpan * std::sqrt(projectedPlanformAreaToVTail / verticalTail.planformArea);

                equivalentHorizontalTailArea = verticalTail.planformArea * std::pow(cos(verticalTail.averageDihedral / 57.3), 2.0);

                Interpolant depthFuselageInterpolator(xStationTailConeFuselage, diamterTailConeFuselage, 1, RegressionMethod::LINEAR);

                fuselageDepthInTheVerticalLocation = depthFuselageInterpolator.getYValueFromRegression(verticalTail.xloc);

                spanVerticalToDepthFuselageRatio = projectedSpanToVtail / fuselageDepthInTheVerticalLocation;

                // Determaine which curves to interpolate for the ratio of the aspect ratio related to the vertical tail in the presence of the fuselage

                if (verticalTail.taperRatio == 1)
                {

                    Interpolant ratioInterpolator(x_lambda1, y_lambda1, 5, RegressionMethod::POLYNOMIAL);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                else if (verticalTail.taperRatio < 1 && verticalTail.taperRatio > 0.6)
                {

                    Interpolant2D ratioInterpolator(5, RegressionMethod::POLYNOMIAL);

                    ratioInterpolator.addCurve(1.0, x_lambda1, y_lambda1);
                    ratioInterpolator.addCurve(0.6, x_lambda06, y_lambda06);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.interpolate(verticalTail.taperRatio, spanVerticalToDepthFuselageRatio);
                }

                else if (verticalTail.taperRatio <= 0.6)
                {

                    Interpolant ratioInterpolator(x_lambda06, y_lambda06, 5, RegressionMethod::POLYNOMIAL);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                // Determine the ratio of the aspect ratio of vertical tail in the presence of the horizontal

                xLocationOfTheAerodynamicCenterHorizontalTail = 0.25 * verticalTail.MAC;
                relativePositionBetweenHorizontalAndVertical = xLocationOfTheAerodynamicCenterHorizontalTail / verticalTail.croot.front();
                zLocationHorizontalToSpanVerticalRatio = 0.0; // Assumption : For V-tail, horizontal tail is at the bottom of the vertical tali, lied on the fuselage center line

                Interpolant2D aspectRatioInThePresenceOfTheHorizontalTailInterpolator(3, RegressionMethod::POLYNOMIAL);

                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.5, x_xcv05, y_xcv05);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.6, x_xcv06, y_xcv06);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.7, x_xcv07, y_xcv07);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.8, x_xcv08, y_xcv08);

                ratioToAspectRatioVerticalInThePresenceOfTheHorizontal = aspectRatioInThePresenceOfTheHorizontalTailInterpolator.interpolate(zLocationHorizontalToSpanVerticalRatio, relativePositionBetweenHorizontalAndVertical);

                // Calculate Kvh factor Roskam Part VI page.390
                Interpolant kvhInterpolator(x_Kvh, y_Kvh, 3, RegressionMethod::POLYNOMIAL);

                factorKvh = kvhInterpolator.getYValueFromRegression(equivalentHorizontalTailArea / projectedPlanformAreaToVTail);

                // Calculate tail arm from vertical tail aerodynamic center to COG
                // Assume aerodynamic center is at quarter chord of vertical tail

                tailArmVerticalTail = (verticalTail.xloc + verticalTail.deltaXtoLEMAC + 0.25 * verticalTail.MAC) - cogData.xCG;

                // Assumption : The aspect ratio of the Vee Tail is equal to the conventional one
                equivalentVerticalTailAspectRatio = ratioToAspectRatioVerticalInThePresenceOfTheFuselage * verticalTail.aspectRatio *
                                                    (1 + factorKvh * (ratioToAspectRatioVerticalInThePresenceOfTheHorizontal - 1));

                Interpolant slopeVerticalTailInterpolator(equivalentAspectRatioVerticalTail, liftSlopeVerticalTail, 3, RegressionMethod::POLYNOMIAL);

                verticalTailLiftSlope = slopeVerticalTailInterpolator.getYValueFromRegression(equivalentVerticalTailAspectRatio);

                // DATCOM Method to estimate (1 + deltaSigma/deltaBeta) * etaV - Roskam part VI pag 389

                switch (builder.getWingData().getWingPosition())
                {

                case WingPosition::HIGH_WING:

                {

                    zWingQuarerRootChordPosition = -std::abs(wing.zloc);

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((projectedPlanformAreaToVTail / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) +
                                                                0.4 * (zWingQuarerRootChordPosition / maxFuselageDepth) + 0.009 * wing.aspectRatio;
                }

                break;

                case WingPosition::MID_WING:

                {

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((projectedPlanformAreaToVTail / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) +
                                                                +0.009 * equivalentVerticalTailAspectRatio;
                }

                break;

                case WingPosition::LOW_WING:

                {

                    zWingQuarerRootChordPosition = std::abs(wing.zloc);

                    sideWashGradientTimesDynamicPressureRatio = 0.724 + 3.06 * ((projectedPlanformAreaToVTail / wing.planformArea) / (1 + std::cos(wing.sweepC4 / 57.3))) -
                                                                0.4 * (zWingQuarerRootChordPosition / maxFuselageDepth) + 0.009 * wing.aspectRatio;
                }
                }

                // NACA Report n.823
                Interpolant2D correctionFactorVeeTailLiftSlopeInterpolator(1, RegressionMethod::LOGARITHMIC);

                correctionFactorVeeTailLiftSlopeInterpolator.addCurve(0.2, K_factor_x_lambda_025, K_factor_y_lambda_025);
                correctionFactorVeeTailLiftSlopeInterpolator.addCurve(0.5, K_factor_x_lambda_05, K_factor_y_lambda_05);
                correctionFactorVeeTailLiftSlopeInterpolator.addCurve(1.0, K_factor_x_lambda_1, K_factor_y_lambda_1);

                kFactorToLiftSlopeVeeTail = correctionFactorVeeTailLiftSlopeInterpolator.interpolate(verticalTail.aspectRatio, verticalTail.taperRatio);

                // Calculate dCn/dBeta
                deltaCnDeltaBetaVerticalTailContribution = kFactorToLiftSlopeVeeTail * verticalTailLiftSlope * sideWashGradientTimesDynamicPressureRatio * (projectedPlanformAreaToVTail / wing.planformArea) *
                                                           (tailArmVerticalTail / wing.totalSpan);

                // Calculate dCy/dBeta
                if (spanVerticalToDepthFuselageRatio >= 0.0 && spanVerticalToDepthFuselageRatio < 2.0)
                {

                    factorKvForSingleVerticalTail = 0.750;
                }

                else if (spanVerticalToDepthFuselageRatio >= 2.0 && spanVerticalToDepthFuselageRatio <= 3.5)
                {

                    Interpolant kvForSingleVerticalTailInterpolator(Kv, verticalTailSpanToFuselageDepthInRegionOfVerticalRatio, 1, RegressionMethod::LINEAR);

                    factorKvForSingleVerticalTail = kvForSingleVerticalTailInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                else if (spanVerticalToDepthFuselageRatio > 3.5)
                {

                    factorKvForSingleVerticalTail = 1.0;
                }

                // The original contribution is given 1/rad Roskam Part VI - Converted to 1/deg by dividing by 57.3
                deltaCyDeltaBetaVerticalTailContribution = -(factorKvForSingleVerticalTail * verticalTailLiftSlope *
                                                             sideWashGradientTimesDynamicPressureRatio * (projectedPlanformAreaToVTail / wing.planformArea)) /
                                                           57.3;
            } // Fine dello switch

            break;

            case TypeOfTail::H_TAIL:
            case TypeOfTail::U_TAIL:

            {

                Interpolant depthFuselageInterpolator(xStationTailConeFuselage, diamterTailConeFuselage, 1, RegressionMethod::LINEAR);

                fuselageDepthInTheVerticalLocation = depthFuselageInterpolator.getYValueFromRegression(verticalTail.xloc);

                spanVerticalToDepthFuselageRatio = verticalTail.totalSpan / fuselageDepthInTheVerticalLocation;

                // Detrmaine which curves to interpolate for the ratio of the aspect ratio related to the vertical tail in the presence of the fuselage

                if (verticalTail.taperRatio == 1)
                {

                    Interpolant ratioInterpolator(x_lambda1, y_lambda1, 5, RegressionMethod::POLYNOMIAL);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                else if (verticalTail.taperRatio < 1 && verticalTail.taperRatio > 0.6)
                {

                    Interpolant2D ratioInterpolator(5, RegressionMethod::POLYNOMIAL);

                    ratioInterpolator.addCurve(1.0, x_lambda1, y_lambda1);
                    ratioInterpolator.addCurve(0.6, x_lambda06, y_lambda06);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.interpolate(verticalTail.taperRatio, spanVerticalToDepthFuselageRatio);
                }

                else
                {

                    Interpolant ratioInterpolator(x_lambda06, y_lambda06, 5, RegressionMethod::POLYNOMIAL);

                    ratioToAspectRatioVerticalInThePresenceOfTheFuselage = ratioInterpolator.getYValueFromRegression(spanVerticalToDepthFuselageRatio);
                }

                // Detrmine the ratio of the aspect ratio of vertical tail in the presence of the horizontal

                xLocationOfTheAerodynamicCenterHorizontalTail = 0.25 * horizontalTail.MAC;
                relativePositionBetweenHorizontalAndVertical = xLocationOfTheAerodynamicCenterHorizontalTail / verticalTail.croot.front();
                if (horizontalTail.zloc > 0.0)
                {
                    zLocationHorizontalToSpanVerticalRatio = -std::abs(horizontalTail.zloc) / verticalTail.totalSpan;
                }

                else
                {

                    zLocationHorizontalToSpanVerticalRatio = std::abs(horizontalTail.zloc) / verticalTail.totalSpan;
                }

                Interpolant2D aspectRatioInThePresenceOfTheHorizontalTailInterpolator(3, RegressionMethod::POLYNOMIAL);

                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.5, x_xcv05, y_xcv05);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.6, x_xcv06, y_xcv06);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.7, x_xcv07, y_xcv07);
                aspectRatioInThePresenceOfTheHorizontalTailInterpolator.addCurve(0.8, x_xcv08, y_xcv08);

                ratioToAspectRatioVerticalInThePresenceOfTheHorizontal = aspectRatioInThePresenceOfTheHorizontalTailInterpolator.interpolate(zLocationHorizontalToSpanVerticalRatio, relativePositionBetweenHorizontalAndVertical);

                // Calculate Kvh factor Roskam Part VI page.390
                Interpolant kvhInterpolator(x_Kvh, y_Kvh, 3, RegressionMethod::POLYNOMIAL);

                factorKvh = kvhInterpolator.getYValueFromRegression(horizontalTail.planformArea / verticalTail.planformArea);

                // Calculate tail arm from vertical tail aerodynamic center to COG
                // Assume aerodynamic center is at quarter chord of vertical tail

                tailArmVerticalTail = (verticalTail.xloc + verticalTail.deltaXtoLEMAC + 0.25 * verticalTail.MAC) - cogData.xCG;

                equivalentVerticalTailAspectRatio = ratioToAspectRatioVerticalInThePresenceOfTheFuselage * verticalTail.aspectRatio *
                                                    (1 + factorKvh * (ratioToAspectRatioVerticalInThePresenceOfTheHorizontal - 1));

                Interpolant2D ratioSideForceWithRespectToBetaRatioInterpolator(3, RegressionMethod::POLYNOMIAL);

                ratioSideForceWithRespectToBetaRatioInterpolator.addCurve(1.0, ratio_WHF_x_bh_lf_1, ratio_WHF_y_bh_lf_1);
                ratioSideForceWithRespectToBetaRatioInterpolator.addCurve(0.8, ratio_WHF_x_bh_lf_08, ratio_WHF_y_bh_lf_08);
                ratioSideForceWithRespectToBetaRatioInterpolator.addCurve(0.6, ratio_WHF_x_bh_lf_06, ratio_WHF_y_bh_lf_06);
                ratioSideForceWithRespectToBetaRatioInterpolator.addCurve(0.4, ratio_WHF_x_bh_lf_04, ratio_WHF_y_bh_lf_04);
                ratioSideForceWithRespectToBetaRatioInterpolator.addCurve(0.2, ratio_WHF_x_bh_lf_02, ratio_WHF_y_bh_lf_02);

                ratioSideForceWithResepectToBeta = ratioSideForceWithRespectToBetaRatioInterpolator.interpolate(std::pow(spanVerticalToDepthFuselageRatio, -1.0), horizontalTail.totalSpan / fuselage.length);

                Interpolant2D effectiveSideForceWRTBetaInterpolator(2, RegressionMethod::POLYNOMIAL);

                effectiveSideForceWRTBetaInterpolator.addCurve(0.0, effectiveAspectRatioVerticalTwin_0, sideForceWRTBetaEffective_0);
                effectiveSideForceWRTBetaInterpolator.addCurve(20.0, effectiveAspectRaticoVerticalTwin_20, sideForceWRTBetaEffective_20);

                effectiveSideForceWRTBetaVerticalTwin = effectiveSideForceWRTBetaInterpolator.interpolate(equivalentVerticalTailAspectRatio, verticalTail.averageLeadingEdgeSweep);

                // Calculate dCy/dBeta
                deltaCyDeltaBetaVerticalTailContribution = -(2 * ratioSideForceWithResepectToBeta * effectiveSideForceWRTBetaVerticalTwin * verticalTail.planformArea / wing.planformArea) / 57.3;

                // Calculate dCn/dBeta

                zCoordinatesOfVerticalTailAerodynamicCenter = verticalTail.zloc + verticalTail.yMAC;

                deltaCnDeltaBetaVerticalTailContribution = -deltaCyDeltaBetaVerticalTailContribution * (tailArmVerticalTail * std::cos(settings.AoA.front() / 57.3) + zCoordinatesOfVerticalTailAerodynamicCenter * std::sin(settings.AoA.front() / 57.3)) / wing.totalSpan;
            }

            break;
            }

            // Calculate total dCn/dBeta - Total aircraft
            deltaCnDeltaBetaAircraft = deltaCnDeltaBetaWingContribution + deltaCnDeltaBetaDueToWingPosition + deltaCnDeltaBetaCanardContribution + deltaCnDeltaBetaHorizontalTailContribution +
                                       deltaCnDeltaBetaFuselageContribution + deltaCnDeltaBetaNacelleContribution + deltaCnDeltaBetaPropeller + deltaCnDeltaBetaVerticalTailContribution;

            // Calculate total dCy/dBeta - Total aircraft
            deltaCyDeltaBetaAircraft = deltaCyDeltaBetaWingContribution + deltaCyDeltaBetaCanardContribution + deltaCyDeltaBetaHorizontalTailContribution +
                                       deltaCyDeltaBetaFuselageContribution + deltaCyDeltaBetaNacelleContribution + deltaCyDeltaBetaWindMillingPropeller + deltaCyDeltaBetaVerticalTailContribution;

            // ========================================================================
            // STEP 8: Calculate Flap Effectivness - From : Experimental analysis of aircrfat directional control effectivness - Nicolosi F., Ciliberti D., Della Vecchia P., Corcione S., Elsevir
            // ========================================================================

            for (size_t n = 0; n < verticalTail.mov.type.size(); n++)
            {

                if (verticalTail.mov.type[n] == 'r')
                {

                    // Larghezza del segmento n-esimo
                    double segmentSpan = (verticalTail.mov.eta_outer[n] - verticalTail.mov.eta_inner[n]) * verticalTail.totalProjectedSpan;

                    // cf/c medio del segmento = media tra inner e outer
                    double cfcMean = 0.5 * (verticalTail.mov.cf_c_inner[n] + verticalTail.mov.cf_c_outer[n]);

                    numeratorAverageChordRatio += cfcMean * segmentSpan;
                    weightsOfWeightedAverageChordRatio += segmentSpan;
                }
            }

            averageChordRatio = numeratorAverageChordRatio / weightsOfWeightedAverageChordRatio;

            // for (size_t i = 0; i < verticalTail.mov.type.size(); i++)
            // {

            //     if (verticalTail.mov.type[i] == 'r')
            //     {

            //         etaInner = verticalTail.mov.eta_inner[i];
            //         etaOuter = verticalTail.mov.eta_outer[i];

            //         Interpolant2D kbFactorInterpolator(3, RegressionMethod::POLYNOMIAL);

            //         kbFactorInterpolator.addCurve(0.0, x_Kb_taper0, y_Kb_taper0);
            //         kbFactorInterpolator.addCurve(0.5, x_Kb_taper05, y_Kb_taper05);
            //         kbFactorInterpolator.addCurve(1.0, x_Kb_taper1, y_Kb_taper1);

            //         factorKbInner = kbFactorInterpolator.interpolate(etaInner, verticalTail.taperRatio);
            //         factorKbOuter = kbFactorInterpolator.interpolate(etaOuter, verticalTail.taperRatio);

            //         thetaFactor = std::acos(2 * averageChordRatio - 1);

            //         twoDimensionalFlapEffectivness = 1 - ((thetaFactor - std::sin(thetaFactor)) / M_PI);

            //         flapSpanFactorKb.push_back(factorKbOuter - factorKbInner);

            //         Interpolant2D ratioFlapEffectivnessInterpolator(1, RegressionMethod::POWER);

            //         ratioFlapEffectivnessInterpolator.addCurve(0.1, x_RatioFlapEff_cf01, y_RatioFlapEff_cf01);
            //         ratioFlapEffectivnessInterpolator.addCurve(0.2, x_RatioFlapEff_cf02, y_RatioFlapEff_cf02);
            //         ratioFlapEffectivnessInterpolator.addCurve(0.3, x_RatioFlapEff_cf03, y_RatioFlapEff_cf03);
            //         ratioFlapEffectivnessInterpolator.addCurve(0.4, x_RatioFlapEff_cf04, y_RatioFlapEff_cf04);
            //         ratioFlapEffectivnessInterpolator.addCurve(0.5, x_RatioFlapEff_cf05, y_RatioFlapEff_cf05);
            //         ratioFlapEffectivnessInterpolator.addCurve(0.6, x_RatioFlapEff_cf06, y_RatioFlapEff_cf06);
            //         ratioFlapEffectivnessInterpolator.addCurve(0.7, x_RatioFlapEff_cf07, y_RatioFlapEff_cf07);
            //         ratioFlapEffectivnessInterpolator.addCurve(0.8, x_RatioFlapEff_cf08, y_RatioFlapEff_cf08);
            //         ratioFlapEffectivnessInterpolator.addCurve(0.9, x_RatioFlapEff_cf09, y_RatioFlapEff_cf09);

            //         ratioEffectivnessFactorKc.push_back(ratioFlapEffectivnessInterpolator.interpolate(verticalTail.aspectRatio, twoDimensionalFlapEffectivness));

            //         tauRudder += flapSpanFactorKb.back() * ratioEffectivnessFactorKc.back() * twoDimensionalFlapEffectivness * verticalTailLiftSlope / builder.getCommonData().getMeanAirfoilSlopeVerticalTail();
            //     }
            // }

            Interpolant3D flapEffectivnessInterpolator(5, RegressionMethod::POLYNOMIAL);

            flapEffectivnessInterpolator.addCurve(1.5,0.3, x_rudder_AR1_5_cr_c_0_30, y_rudder_AR1_5_cr_c_0_30);
            flapEffectivnessInterpolator.addCurve(1.5,0.37, x_rudder_AR1_5_cr_c_0_37, y_rudder_AR1_5_cr_c_0_37);
            flapEffectivnessInterpolator.addCurve(1.5,0.45, x_rudder_AR1_5_cr_c_0_45, y_rudder_AR1_5_cr_c_0_45);
            flapEffectivnessInterpolator.addCurve(2.0,0.3, x_rudder_AR2_0_cr_c_0_30, y_rudder_AR2_0_cr_c_0_30);
            flapEffectivnessInterpolator.addCurve(2.0,0.37, x_rudder_AR2_0_cr_c_0_37, y_rudder_AR2_0_cr_c_0_37);
            flapEffectivnessInterpolator.addCurve(2.0,0.45, x_rudder_AR2_0_cr_c_0_45, y_rudder_AR2_0_cr_c_0_45);


            tauRudder = flapEffectivnessInterpolator.interpolate(builder.getWingData().getMaximumRudderDeflection(),averageChordRatio, verticalTail.aspectRatio);


            Interpolant deltaCnBetaDueToVerticalSurfaceInterpolator(wingToMaximumFuselageDiameterRatio, deltaCnBetaDueToVerticalTailSurface, 2, RegressionMethod::POLYNOMIAL);

            deltaCnDeltaBetaDueToVerticalTailSurface = deltaCnBetaDueToVerticalSurfaceInterpolator.getYValueFromRegression(std::abs(wing.zloc) / fuselage.diameter);

            dynamicPressureRatioAtVerticalTail = (deltaCnDeltaBetaVerticalTailContribution - deltaCnDeltaBetaDueToVerticalTailSurface) / (verticalTailLiftSlope * (verticalTail.planformArea / wing.planformArea) * (tailArmVerticalTail / wing.totalSpan));

            deltaCnDeltaRudderDeflection = -tauRudder * verticalTailLiftSlope * (verticalTail.planformArea / wing.planformArea) * (tailArmVerticalTail / wing.totalSpan) * dynamicPressureRatioAtVerticalTail;

            // ========================================================================
            // STEP 9: Saving results
            // ========================================================================

            singleComponentsDerivativesYaw = {.deltaCnDeltaBetaWingContribution = deltaCnDeltaBetaWingContribution,
                                              .deltaCnDeltaBetaCanardContribution = deltaCnDeltaBetaCanardContribution,
                                              .deltaCnDeltaBetaHorizontalTailContribution = deltaCnDeltaBetaHorizontalTailContribution,
                                              .deltaCnDeltaBetaVerticalTailContribution = deltaCnDeltaBetaVerticalTailContribution,
                                              .deltaCnDeltaBetaFuselageContribution = deltaCnDeltaBetaFuselageContribution,
                                              .deltaCnDeltaBetaNacelleContribution = deltaCnDeltaBetaNacelleContribution};

            singleComponentsDerivativesSideForce = {.deltaCyDeltaBetaWingContribution = deltaCyDeltaBetaWingContribution,
                                                    .deltaCyDeltaBetaCanardContribution = deltaCyDeltaBetaCanardContribution,
                                                    .deltaCyDeltaBetaHorizontalTailContribution = deltaCyDeltaBetaHorizontalTailContribution,
                                                    .deltaCyDeltaBetaVerticalTailContribution = deltaCyDeltaBetaVerticalTailContribution,
                                                    .deltaCyDeltaBetaFuselageContribution = deltaCyDeltaBetaFuselageContribution,
                                                    .deltaCyDeltaBetaNacelleContribution = deltaCyDeltaBetaNacelleContribution,
                                                    .deltaCyDeltaBetaWindMillingPropeller = deltaCyDeltaBetaWindMillingPropeller};

            aircraftDirectionalDerivatives = {.deltaCnDeltaBetaAircraft = deltaCnDeltaBetaAircraft,
                                              .deltaCyDeltaBetaAircraft = deltaCyDeltaBetaAircraft,
                                              .deltaCnDeltaRudderDeflection = deltaCnDeltaRudderDeflection

            };
        }

        // Getters for the results

        DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesYawToSingleComponent getSingleComponentsDerivativesYaw() const
        {
            return singleComponentsDerivativesYaw;
        }

        DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesSideForceToSingleComponent getSingleComponentsDerivativesSideForce() const
        {
            return singleComponentsDerivativesSideForce;
        }

        DIRECTIONAL_STABILITY::DirectionalStabilityDerivatives getAircraftDirectionalDerivatives() const
        {
            return aircraftDirectionalDerivatives;
        }
    };
};
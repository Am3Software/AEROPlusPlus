#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "Interpolant.h"
#include "RegressionMethod.h"
#include "WINGBASEDATA.h"
#include "FUSELAGEBASEDATA.h"
#include "ENGINEBASEDATA.h"
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
#include "ATMOSISA.h"
#include "ODE45.h"
#include "PLOT.h"

class FuelCalculatorBreguet
{

private:
    double availablePower = 0.0;
    double ramEffect = 0.0;
    double kFactotToTheThrust = 0.0;
    double availableThrust = 0.0;
    double averageSpeedKmh = 0.0;
    double densityRatio = 0.0;
    double integrationStep = 1.0 / 99.0;
    double weightFuelTaxiTakeOff = 0.0;
    double weightFuelClimb = 0.0;
    double designFuelMass = 0.0;
    double weightFuelDescentLanding = 0.0;
    double rerserveFuel = 0.0;

    double weightAfterTaxiTakeOff = 0.0;
    double weightAfterClimb = 0.0;
    double weightAfterDescentLanding = 0.0;

    double ratioWeightTaxiTakeOffPropeller = 0.995;
    double ratioWeightTaxiTakeOffJet = 0.995;
    double ratioWeightTakeOffClimbPropeller = 0.980;
    double ratioWeightTakeOffClimbJet = 0.980;
    double ratioWeightClimbDescentLandingPropeller = 0.5 * (0.99 + 1.0);
    double ratioWeightClimbDescentLandingJet = 0.5 * (0.99 + 1.0);

public:
    FuelCalculatorBreguet() = default;

    /// @brief Calculate fuel mass for propeller-driven aircraft using Breguet's formula
    /// @param engineType Type of aircraft engine (propeller-driven  e.g. piston, turboprop, turbocharged)
    /// @param MTOM Maximum take-off mass (in kg)
    /// @param range Range to be covered (in nautical miles)
    /// @param specificFuelConsumption Specific fuel consumption (in lb/hp/hr)
    /// @param BSHP Brake specific horsepower (in hp)
    /// @param throttle Throttle setting (0 to 1 for the cruise evaluation)
    /// @param averageSpeed Average speed during the cruise (in m/s)
    /// @param altitude Cruise altitude (in meters)
    /// @param etaP Propeller efficiency (0 to 1)
    /// @param numberOfEngines Number of engines
    /// @param criticalAltitude Critical altitude for turbocharged engines (in meters, default 0 for non-turbocharged)
    /// @param reserveFraction Fraction of fuel reserved for contingencies (default 0.05 for 5%)
    /// @return Estimated fuel mass required for the mission (in kg)
    double calculateFuelMassPropellerDriven(AircraftEngineType engineType, double MTOM, double range, double specificFuelConsumption,
                                            double BSHP, double throttle, double averageSpeed, double altitude, double etaP,
                                            double numberOfEngines, double criticalAltitude = 0.0, double reserveFraction = 0.05)
    {

        switch (engineType)
        {
        case AircraftEngineType::PISTON_PROPELLER:

        {

            ConvMass massConverter(Mass::KG, Mass::LB, MTOM);

            MTOM = massConverter.getConvertedValues();

            // Gudmunsson suggests this formula
            ratioWeightTaxiTakeOffPropeller = 1 - 0.004 * specificFuelConsumption;

            weightAfterTaxiTakeOff = MTOM * ratioWeightTaxiTakeOffPropeller;

            weightFuelTaxiTakeOff = MTOM * (1 - ratioWeightTaxiTakeOffPropeller);

            weightAfterClimb = weightAfterTaxiTakeOff * ratioWeightTakeOffClimbPropeller;

            weightFuelClimb = weightAfterTaxiTakeOff * (1 - ratioWeightTakeOffClimbPropeller);

            densityRatio = Atmosphere::ISA::densityRatio(altitude);

            availablePower = numberOfEngines * BSHP * densityRatio * throttle * etaP;

            ConvVel velocityConverter(Speed::M_TO_S, Speed::KTS, averageSpeed);

            averageSpeed = velocityConverter.getConvertedValues();

            ODE45 odeSolver45([=](double x, const double &y)
                              { return -specificFuelConsumption * availablePower / averageSpeed; },
                              0,
                              range,
                              weightAfterClimb,
                              integrationStep);

            // Get solution arrays (angle of attack and downwash angle)
            odeSolver45.getT();
            odeSolver45.getY();

            designFuelMass = weightAfterClimb - odeSolver45.getY().back();

            weightFuelDescentLanding = odeSolver45.getY().back() * (1 - ratioWeightClimbDescentLandingPropeller);

            rerserveFuel = designFuelMass * reserveFraction;

            ConvMass fuelMassConverter(Mass::LB, Mass::KG, (weightFuelTaxiTakeOff + weightFuelClimb + designFuelMass + weightFuelDescentLanding + rerserveFuel));

            Plot plotfuel(odeSolver45.getT(), odeSolver45.getY(),
                          "Range (nm)",
                          "Fuel mass (lb)",
                          "Fuel consumption",
                          "FC response",
                          "lines", "1", "1", "", "", "blue");

            return fuelMassConverter.getConvertedValues();
        }
        break;

        case AircraftEngineType::TURBOCHARGED:

        {

            ConvMass massConverter(Mass::KG, Mass::LB, MTOM);

            MTOM = massConverter.getConvertedValues();

            // Gudmunsson suggests this formula
            ratioWeightTaxiTakeOffPropeller = 1 - 0.004 * specificFuelConsumption;

            weightAfterTaxiTakeOff = MTOM * ratioWeightTaxiTakeOffPropeller;

            weightFuelTaxiTakeOff = MTOM * (1 - ratioWeightTaxiTakeOffPropeller);

            weightAfterClimb = weightAfterTaxiTakeOff * ratioWeightTakeOffClimbPropeller;

            weightFuelClimb = weightAfterTaxiTakeOff * (1 - ratioWeightTakeOffClimbPropeller);

            if (altitude > criticalAltitude)
            {
                availablePower = numberOfEngines * BSHP * throttle * etaP * (1.0 - (altitude - criticalAltitude) * 0.1 / 1e3);
            }

            else
            {

                availablePower = numberOfEngines * BSHP * throttle * etaP;
            }

            ConvVel velocityConverter(Speed::M_TO_S, Speed::KTS, averageSpeed);

            averageSpeed = velocityConverter.getConvertedValues();

            ODE45 odeSolver45([=](double x, const double &y)
                              { return -specificFuelConsumption * availablePower / averageSpeed; },
                              0,
                              range,
                              weightAfterClimb,
                              integrationStep);

            // Get solution arrays (angle of attack and downwash angle)
            odeSolver45.getT();
            odeSolver45.getY();

            designFuelMass = weightAfterClimb - odeSolver45.getY().back();

            weightFuelDescentLanding = odeSolver45.getY().back() * (1 - ratioWeightClimbDescentLandingPropeller);

            rerserveFuel = designFuelMass * reserveFraction;

            ConvMass fuelMassConverter(Mass::LB, Mass::KG, (weightFuelTaxiTakeOff + weightFuelClimb + designFuelMass + weightFuelDescentLanding + rerserveFuel));

            Plot plotfuel(odeSolver45.getT(), odeSolver45.getY(),
                          "Range (nm)",
                          "Fuel mass (lb)",
                          "Fuel consumption",
                          "FC response",
                          "lines", "1", "1", "", "", "blue");

            return fuelMassConverter.getConvertedValues();
        }
        break;

        case AircraftEngineType::TURBOPROP:

        {
            ConvMass massConverter(Mass::KG, Mass::LB, MTOM);

            MTOM = massConverter.getConvertedValues();

            densityRatio = Atmosphere::ISA::densityRatio(altitude);

            ConvVel velocityConvertertoKmh(Speed::M_TO_S, Speed::KM_TO_H, averageSpeed);

            averageSpeedKmh = velocityConvertertoKmh.getConvertedValues();

            ramEffect = 1.0 + 0.0080 * std::pow((averageSpeedKmh / 100.0), 2.0);

            availablePower = numberOfEngines * BSHP * throttle * densityRatio * etaP * ramEffect;

            // Gudmunsson suggests this formula
            ratioWeightTaxiTakeOffPropeller = 1 - 0.004 * specificFuelConsumption;

            weightAfterTaxiTakeOff = MTOM * ratioWeightTaxiTakeOffPropeller;

            weightFuelTaxiTakeOff = MTOM * (1 - ratioWeightTaxiTakeOffPropeller);

            weightAfterClimb = weightAfterTaxiTakeOff * ratioWeightTakeOffClimbPropeller;

            weightFuelClimb = weightAfterTaxiTakeOff * (1 - ratioWeightTakeOffClimbPropeller);

            ConvVel velocityConverter(Speed::M_TO_S, Speed::KTS, averageSpeed);

            averageSpeed = velocityConverter.getConvertedValues();

            ODE45 odeSolver45([=](double x, const double &y)
                              { return -specificFuelConsumption * availablePower / averageSpeed; },
                              0,
                              range,
                              weightAfterClimb,
                              integrationStep);

            // Get solution arrays (angle of attack and downwash angle)
            odeSolver45.getT();
            odeSolver45.getY();

            designFuelMass = weightAfterClimb - odeSolver45.getY().back();

            weightFuelDescentLanding = odeSolver45.getY().back() * (1 - ratioWeightClimbDescentLandingPropeller);

            rerserveFuel = designFuelMass * reserveFraction;

            ConvMass fuelMassConverter(Mass::LB, Mass::KG, (weightFuelTaxiTakeOff + weightFuelClimb + designFuelMass + weightFuelDescentLanding + rerserveFuel));

            Plot plotfuel(odeSolver45.getT(), odeSolver45.getY(),
                          "Range (nm)",
                          "Fuel mass (lb)",
                          "Fuel consumption",
                          "FC response",
                          "lines", "1", "1", "", "", "blue");

            return fuelMassConverter.getConvertedValues();
        }
        break;

        default:
            throw std::invalid_argument("Invalid engine type for propeller-driven fuel calculation.");
        }

        return 0.0;
    }

    double calculateFuelMassJetDriven(AircraftEngineType engineType, double MTOM, double range, double specificFuelConsumption,
                                      double takeOffThrust, double throttle, double averageSpeed, double altitude,
                                      double numberOfEngines, double reserveFraction = 0.05)
    {
        switch (engineType)
        {

        case AircraftEngineType::JET_TURBOFAN:

        {

            densityRatio = Atmosphere::ISA::densityRatio(altitude);

            ConvLength lengthConverter(Length::M, Length::FT, altitude);

            altitude = lengthConverter.getConvertedValues();

            if (altitude <= 15000)
            {

                kFactotToTheThrust = 1 - 0.20 * averageSpeed / 100.0;

                availableThrust = 0.83 * std::sqrt(densityRatio) * kFactotToTheThrust * takeOffThrust * numberOfEngines * throttle;
            }

            else if (altitude > 15000 && altitude <= 45000)
            {

                availableThrust = 0.75 * densityRatio * takeOffThrust * numberOfEngines * throttle;
            }

            ConvMass massConverter(Mass::KG, Mass::LB, MTOM);

            MTOM = massConverter.getConvertedValues();

            // Gudmunsson suggests this formula
            ratioWeightTaxiTakeOffJet= 1 - 0.015 * specificFuelConsumption;

            weightAfterTaxiTakeOff = MTOM * ratioWeightTaxiTakeOffJet;

            weightFuelTaxiTakeOff = MTOM * (1 - ratioWeightTaxiTakeOffJet);

            weightAfterClimb = weightAfterTaxiTakeOff * ratioWeightTakeOffClimbJet;

            weightFuelClimb = weightAfterTaxiTakeOff * (1 - ratioWeightTakeOffClimbJet);

            ConvVel velocityConverter(Speed::M_TO_S, Speed::KTS, averageSpeed);

            averageSpeed = velocityConverter.getConvertedValues();

            ODE45 odeSolver45([=](double x, const double &y)
                              { return -specificFuelConsumption * availableThrust / averageSpeed; },
                              0,
                              range,
                              weightAfterClimb,
                              integrationStep);

            // Get solution arrays (angle of attack and downwash angle)
            odeSolver45.getT();
            odeSolver45.getY();

            designFuelMass = weightAfterClimb - odeSolver45.getY().back();

            weightFuelDescentLanding = odeSolver45.getY().back() * (1 - ratioWeightClimbDescentLandingJet);

            rerserveFuel = designFuelMass * reserveFraction;

            ConvMass fuelMassConverter(Mass::LB, Mass::KG, (weightFuelTaxiTakeOff + weightFuelClimb + designFuelMass + weightFuelDescentLanding + rerserveFuel));

            // Plot plotfuel(odeSolver45.getT(), odeSolver45.getY(),
            //               "Range (nm)",
            //               "Fuel mass (lb)",
            //               "Fuel consumption",
            //               "FC response",
            //               "lines", "1", "1", "", "", "blue");

            return fuelMassConverter.getConvertedValues();
        }
        break;

        default:
            throw std::invalid_argument("Invalid engine type for propeller-driven fuel calculation.");
        }

        return 0.0;
    }
};
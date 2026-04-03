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
#include "CD0Calculator.h"
#include "SILENTORCOMPONENT.h"
#include "OswaldFactorCalculator.h"
#include "CD0Calculator.h"

/// @brief Constructor for the CDCalculator class. Initializes the calculator with the necessary aircraft data, settings, and geometric information for the various components of the aircraft. The constructor takes in references to the builder object, aircraft data, aerodynamic settings, and geometric data for the wing, horizontal tail, vertical tail, fuselage, nacelle, canard (optional), boom (optional), and EOIR (optional). This allows the calculator to have access to all relevant information needed for accurate CD calculations across different components of the aircraft.
/// @param nameOfAircraft The name of the aircraft for which CD is being calculated.
/// @param builder A reference to the BuildAircraft object that contains the aircraft's design.
/// @param aircraftData A reference to the VSP::Aircraft object that contains the geometric data of the aircraft.
/// @param settings A reference to the VSP::AeroSettings object that contains the aerodynamic settings.
/// @param wing A reference to the VSP::Wing object that contains the geometric data of the wing.
/// @param horizontal A reference to the VSP::Wing object that contains the geometric data of the horizontal tail.
/// @param vertical A reference to the VSP::Wing object that contains the geometric data of the vertical tail.
/// @param fus A reference to the VSP::Fuselage object that contains the geometric data of the fuselage.
/// @param nac A reference to the VSP::Nacelle object that contains the geometric data of the nacelle.
/// @param canard An optional reference to the VSP::Wing object that contains the geometric data of the canard. Default is an empty wing object.
/// @param boom An optional reference to the VSP::Boom object that contains the geometric data of the boom. Default is an empty boom object.
/// @param eoir An optional reference to the VSP::EOIR object that contains the geometric data of the EOIR. Default is an empty EOIR object.
class CDCalculator
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
    double liftCoeffcient = 0.0;
    double dragCoefficient = 0.0;

public:
    /// @brief Constructor for the CDCalculator class. Initializes the calculator with the necessary aircraft data, settings, and geometric information for the various components of the aircraft. The constructor takes in references to the builder object, aircraft data, aerodynamic settings, and geometric data for the wing, horizontal tail, vertical tail, fuselage, nacelle, canard (optional), boom (optional), and EOIR (optional). This allows the calculator to have access to all relevant information needed for accurate CD calculations across different components of the aircraft.
    /// @param nameOfAircraft The name of the aircraft for which CD is being calculated.
    /// @param builder A reference to the BuildAircraft object that contains the aircraft's design.
    /// @param aircraftData A reference to the VSP::Aircraft object that contains the geometric data of the aircraft.
    /// @param settings A reference to the VSP::AeroSettings object that contains the aerodynamic settings.
    /// @param wing A reference to the VSP::Wing object that contains the geometric data of the wing.
    /// @param horizontal A reference to the VSP::Wing object that contains the geometric data of the horizontal tail.
    /// @param vertical A reference to the VSP::Wing object that contains the geometric data of the vertical tail.
    /// @param fus A reference to the VSP::Fuselage object that contains the geometric data of the fuselage.
    /// @param nac A reference to the VSP::Nacelle object that contains the geometric data of the nacelle.
    /// @param canard An optional reference to the VSP::Wing object that contains the geometric data of the canard. Default is an empty wing object.
    /// @param boom An optional reference to the VSP::Boom object that contains the geometric data of the boom. Default is an empty boom object.
    /// @param eoir An optional reference to the VSP::EOIR object that contains the geometric data of the EOIR. Default is an empty EOIR object.
    CDCalculator(std::string nameOfAircraft,
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

    /// @brief Calculates the total drag coefficient (CD) of the aircraft for a given angle of attack, Mach number, altitude, and flight phase.
    /// @param AngleOfAttack The angle of attack in degrees.
    /// @param Mach The Mach number.
    /// @param altitude The altitude in meters.
    /// @param flightPhase The flight phase, which can be "TAKE_OFF", "CLIMB", "CRUISE", "DESCENT", or "LANDING".
    double getCDTotalAircraft(double AngleOfAttack, double Mach, double altitude, std::string flightPhase)
    {

        RestoreSettings settingsRestorer; // Create an instance of RestoreSettings to automatically restore settings when going out of scope

        settingsRestorer.setSavePrevoiusSettings(settings);

        settings.AoA = {AngleOfAttack};
        settings.Mach = Mach;
        settings.altitude = altitude;

        VSPGEOMTRYEXTRACTOR::GeometryExtractor geomExtractor;
        geomExtractor.extractAllGeoms(builder.getCommonData().getNameOfAircraft(),
                                      builder.getCommonData().getNameOfAircraft() + "_AllGeoms.vspscript");

        VSPGEOMTRYEXTRACTOR::AircraftGeometryData allGeomData = geomExtractor.getGeometryData();

        try
        {
            SILENTORCOMPONENT::SilentorComponent silentorAnalysis(builder.getCommonData().getNameOfAircraft(),
                                                                  "SilentorComponentAnalysis.vspscript");

            std::map<std::string, bool> componentDetected;

            for (const auto &geom : allGeomData.nameOfComponentGeom)
            {

                if (geom == canard.id)
                {

                    componentDetected[canard.id] = true;
                }

                else
                {
                    componentDetected[canard.id] = false;
                }

                if (geom == boom.id)
                {

                    componentDetected[boom.id] = true;
                }

                else
                {
                    componentDetected[boom.id] = false;
                }

                if (geom == eoir.id)
                {

                    componentDetected[eoir.id] = true;
                }

                else
                {
                    componentDetected[eoir.id] = false;
                }
            }

            std::vector<std::string> componentToSilentor{wing.id, horizontal.id, vertical.id, fus.id};

            for (const auto &entry : componentDetected)
            {

                if (entry.second)
                {
                    componentToSilentor.push_back(entry.first);
                }
            }

            silentorAnalysis.GetGeometryWithThisComponent(aircraftData, allGeomData, componentToSilentor);

            silentorAnalysis.executeAnalysis(settings);

            liftCoeffcient = silentorAnalysis.getAerodynamicCoefficients().liftCoefficient.front();
        }

        catch (const std::exception &e)
        {
            std::cout << "[CATCH] " << e.what() << std::endl;
            std::cout.flush();
            return -1.0;
        }
        catch (...)
        {
            std::cout << "[CATCH] Unknown exception occurred!" << std::endl;
            std::cout.flush();
            return -1.0;
        }

        CD0Calculator cd0Calc(nameOfAircraft, builder, aircraftData, settings, wing, horizontal, vertical, fus, nac);
        OswaldFactorCalculator oswaldCalc(builder, wing, fus);

        auto [oswaldTakeOff, oswaldClimb, oswaldCruise, oswaldDescent, oswaldLanding] = oswaldCalc.getOswaldFactor();

        if (flightPhase == "TAKE_OFF")
        {

            // The boolan argument is needed to activate the landing-gear CD0 calculous.
            dragCoefficient = cd0Calc.getTotalCD0Aircraft(true) + std::pow(liftCoeffcient, 2) / (M_PI * oswaldTakeOff * wing.aspectRatio);
        }

        else if (flightPhase == "CLIMB")
        {

            dragCoefficient = cd0Calc.getTotalCD0Aircraft() + std::pow(liftCoeffcient, 2) / (M_PI * oswaldClimb * wing.aspectRatio);
        }

        else if (flightPhase == "CRUISE")
        {

            dragCoefficient = cd0Calc.getTotalCD0Aircraft() + std::pow(liftCoeffcient, 2) / (M_PI * oswaldCruise * wing.aspectRatio);
        }

        else if (flightPhase == "DESCENT")
        {

            dragCoefficient = cd0Calc.getTotalCD0Aircraft() + std::pow(liftCoeffcient, 2) / (M_PI * oswaldDescent * wing.aspectRatio);
        }

        else if (flightPhase == "LANDING")
        {

            // The boolan argument is needed to activate the landing-gear CD0 calculous.
            dragCoefficient = cd0Calc.getTotalCD0Aircraft(true) + std::pow(liftCoeffcient, 2) / (M_PI * oswaldLanding * wing.aspectRatio);
        }

        else
        {

            std::cerr << "Invalid flight phase specified. Valid options are: TAKE_OFF, CLIMB, CRUISE, DESCENT, LANDING." << std::endl;
            return -1.0; // Return an error code or handle as appropriate
        }

        settings = settingsRestorer.getSettingsToRestore();

        return dragCoefficient;
    }

    double getLiftCoefficient() const
    {
        return liftCoeffcient;
    }
};
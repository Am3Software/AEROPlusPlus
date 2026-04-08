#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "WINGBASEDATA.h"
#include "FUSELAGEBASEDATA.h"
#include "ENGINEBASEDATA.h"
#include "LANDINGGEARSBASEDATA.h"
#include "EnumAircraftCategory.h"
#include "EnumTypeOfComposite.h"
#include "EnumTypeOfStabilizer.h"
#include "EnumTypeOfTail.h"
#include "EnumEnginePosition.h"
#include "EnumWingPosition.h"
#include "VSPGEOMETRYEXTRACTOR.h"
#include "VSPScriptGenerator.h"
#include "VSPAeroGenerator.h"
#include "BUILDAIRCRAFT.h"
#include "WETTEDAREA.h"
#include "RestoreSettings.h"
#include "SILENTORCOMPONENT.h"
#include "Interpolant.h"


class CalculateAircrfatAlphaZeroLiftAngle
{

protected:
    const BuildAircraft &builder;
    const VSP::Wing &wing;
    const VSP::Wing &horizontal;
    const VSP::Wing &vertical;
    const VSP::Fuselage &fus;
    const VSP::Nacelle &nac;
    const VSP::Wing canard;
    const VSP::Boom boom;
    const VSP::EOIR eoir;
    VSP::AeroSettings &settings;
    const VSP::Aircraft &aircraftData;

private:
    double beta = 0.0;
    double kFactorWing = 0.0;
    double kFactorHorizontal = 0.0;
    double downWashGradient = 0.0;
    double finiteWingLiftSlope = 0.0;
    double finiteHorizontalSlope = 0.0;
    double aircraftLiftSlope = 0.0;
    double liftSlope2DCorrectedCompressibilityWing = 0.0;
    double liftSlope2DCorrectedCompressibilityHorizontal = 0.0;
    double etaMinimumConventionalTail = 0.85;
    double etaMaximumConventionalTail = 0.95;
    std::vector<double> liftCoefficents ;
    double aircrfatAlphaZeroLiftAngle = 0.0;
    double liftCoeffcientAtAlphaZero = 0.0;
    double vspAircrfatLiftSlope = 0.0;

public:
    CalculateAircrfatAlphaZeroLiftAngle(const BuildAircraft &builder,
                                        const VSP::Aircraft &aircraftData,
                                        VSP::AeroSettings &settings,
                                        const VSP::Wing &wing,
                                        const VSP::Wing &horizontal,
                                        const VSP::Wing &vertical,
                                        const VSP::Fuselage &fus,
                                        const VSP::Nacelle &nac,
                                        const VSP::Wing canard = VSP::Wing(),
                                        const VSP::Boom boom = VSP::Boom(),
                                        const VSP::EOIR eoir = VSP::EOIR()) : builder(builder),
                                                                              aircraftData(aircraftData),
                                                                              wing(wing),
                                                                              horizontal(horizontal),
                                                                              vertical(vertical),
                                                                              fus(fus),
                                                                              nac(nac),
                                                                              canard(canard),
                                                                              boom(boom),
                                                                              eoir(eoir),
                                                                              settings(settings)
    {
    }

    double getAircraftAlphaZeroLiftAngle()
    {

     
        // Calculate di CL0 using VSPAERO

        RestoreSettings settingsRestorer; // Create an instance of RestoreSettings to automatically restore settings when going out of scope

        settingsRestorer.setSavePrevoiusSettings(settings);

        settings.AoA = {0.0,1.0};
        

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

            liftCoefficents = silentorAnalysis.getAerodynamicCoefficients().liftCoefficient;
        }

        catch (const std::exception &e)
        {
            std::cout << "[CATCH] " << e.what() << std::endl;
            std::cout.flush();
            return -1.0;
        }
        catch (...)
        {
            std::cout << "[CATCH] Eccezione sconosciuta!" << std::endl;
            std::cout.flush();
            return -1.0;
        }


        // Calculate alpha zero lift angle

        Interpolant zeroLiftAngelInterpolator (liftCoefficents, settings.AoA, 1, RegressionMethod::LINEAR);
        
        aircrfatAlphaZeroLiftAngle = zeroLiftAngelInterpolator.getYValueFromRegression(0.0);

        vspAircrfatLiftSlope = std::pow(zeroLiftAngelInterpolator.getCoefficients().front(), -1); // Vsp Aircrfat lift slope 1/deg

        settings = settingsRestorer.getSettingsToRestore();

        return aircrfatAlphaZeroLiftAngle;

    }

    double getLiftCoefficientAtAlphaZero()
    {

        liftCoeffcientAtAlphaZero = liftCoefficents.front(); // Assuming the first element corresponds to the lift coefficient at alpha zero

        return liftCoeffcientAtAlphaZero;
    }

    double getVSPAircraftLiftSlope()
    {

        return vspAircrfatLiftSlope;
    }
};
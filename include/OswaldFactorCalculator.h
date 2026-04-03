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


class OswaldFactorCalculator
{

protected:
    const BuildAircraft &builder;
    VSP::Wing wing;
    VSP::Fuselage fuselage;


private:
    double oswaldFactorDueToFuseDiamterToSpanWingRatio = 0.0;
    double oswaldFactorCruise = 0.0;
    double oswaldFactorAircraftTakeOff = 0.0;
    double oswaldFactorAircraftClimb = 0.0;
    double oswaldFactorAircraftDescent = 0.0;
    double oswaldFactorAircraftLanding = 0.0;

public:
    OswaldFactorCalculator(const BuildAircraft &builder, const VSP::Wing &wing, const VSP::Fuselage &fuselage)
        : builder(builder), wing(wing), fuselage(fuselage)
    {
    }

    std::array<double, 5> getOswaldFactor() {

        // Obert Method with correction factors

        oswaldFactorDueToFuseDiamterToSpanWingRatio = 1 - 2 * std::pow(fuselage.diameter/ wing.totalProjectedSpan, 2);

        oswaldFactorCruise = oswaldFactorDueToFuseDiamterToSpanWingRatio * 1/(1.05 + 0.007*M_PI*wing.aspectRatio);


        oswaldFactorAircraftTakeOff = oswaldFactorCruise*builder.getOswaldCorrectionFactor().getKOswaldTakeOff();
        oswaldFactorAircraftClimb = oswaldFactorCruise*builder.getOswaldCorrectionFactor().getKOswaldClimb();
        oswaldFactorAircraftDescent = oswaldFactorCruise*builder.getOswaldCorrectionFactor().getKOswaldDescent();
        oswaldFactorAircraftLanding = oswaldFactorCruise*builder.getOswaldCorrectionFactor().getKOswaldLanding();

        

        return {oswaldFactorAircraftTakeOff, oswaldFactorAircraftClimb, oswaldFactorCruise,oswaldFactorAircraftDescent, oswaldFactorAircraftLanding};
    }

};
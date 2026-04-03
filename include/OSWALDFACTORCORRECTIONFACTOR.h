#pragma once

#include <string>
#include <cmath>
#include "BASEAIRCRAFTDATA.h"

class OswaldCorrectionFactor : public BaseAircraftData
{

private:
    
    // Correction factors
    double kOswaldTakeOff = 1.0;
    double kOswaldClimb = 1.0;
    double kOswaldDescent = 1.0;
    double kOswaldLanding = 1.0;
   
public:
    OswaldCorrectionFactor() = default;

    // Setters

    OswaldCorrectionFactor &setKOswaldTakeOff(double value)
    {
        this->kOswaldTakeOff = value;
        return *this;
    }

    OswaldCorrectionFactor &setKOswaldClimb(double value)
    {
        this->kOswaldClimb = value;
        return *this;
    }

    OswaldCorrectionFactor &setKOswaldDescent(double value)
    {
        this->kOswaldDescent = value;
        return *this;
    }

    OswaldCorrectionFactor &setKOswaldLanding(double value)
    {
        this->kOswaldLanding = value;
        return *this;
    }

  
        // Getters
  
    double getKOswaldTakeOff() const { return kOswaldTakeOff; }
    double getKOswaldClimb() const { return kOswaldClimb; }
    double getKOswaldDescent() const { return kOswaldDescent; }
    double getKOswaldLanding() const { return kOswaldLanding; }
    
};
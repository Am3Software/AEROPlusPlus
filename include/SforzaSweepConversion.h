#ifndef SWEEP_H
#define SWEEP_H


#include <vector>
#include <iostream>
#include <cmath>

/// @brief Constructor for ConvSweep class
/// @param inputKnownSweep Known sweep angle in degrees
/// @param taperRatio Taper ratio of the wing
/// @param aspectRatio Aspect ratio of the wing
/// @param knwonSweepInPercentofTheChord Known sweep in percent of the chord
/// @param unknownSweepInPercentofTheChord Unknown sweep in percent of the chord
class ConvSweep {
   
    private:


    double inputKnownSweep;
    double taperRatio;
    double aspectRatio;
    double knwonSweepInPercentofTheChord;
    double unknownSweepInPercentofTheChord;
    double outputSweep = 0.0;

    double convertedValues() {
    
        
            outputSweep = std::atan( std::tan(inputKnownSweep/57.3) - (4.0 / aspectRatio) * ((1.0 - taperRatio) / (1.0 + taperRatio)) * (unknownSweepInPercentofTheChord - knwonSweepInPercentofTheChord) ) * 57.3;


            return outputSweep;
     
    }

    public:

    
    ConvSweep(double inputKnownSweep, double taperRatio, double aspectRatio, double knwonSweepInPercentofTheChord, double unknownSweepInPercentofTheChord) {

        this->inputKnownSweep = inputKnownSweep;
        this->taperRatio = taperRatio;
        this->aspectRatio = aspectRatio;
        this->knwonSweepInPercentofTheChord = knwonSweepInPercentofTheChord;
        this->unknownSweepInPercentofTheChord = unknownSweepInPercentofTheChord;

       convertedValues();


        
    }


   /// @brief Returns the calculated sweep angle
   /// @return Calculated sweep angle in degrees
   const double& getSweepAngle() const{

        return outputSweep;
    }



    
};

#endif
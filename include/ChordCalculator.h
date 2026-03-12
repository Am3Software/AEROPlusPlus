#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <array>
#include <memory>
#include <iostream>
#include "VSPGEOMETRYEXTRACTOR.h"
#include "VSPAeroGenerator.h"
#include "VSPScriptGenerator.h"
#include "READPOLARFILE.h"
#include "ControlSurfaceBuilder.h"
#include "EQUALSIGNORECASE.h"
#include "Interpolant.h"
#include "EnumTypeOfWing.h"

class ChordCalculator
{

protected:
    VSP::Wing &wing;

private:
    inline static VSP::Wing *wingPtr = nullptr; // Static pointer to hold the reference to the wing
    constexpr static double eps = 1e-9;

public:
    ChordCalculator(VSP::Wing &wing) : wing(wing)
    {

        wingPtr = &wing; // Initialize the static pointer with the reference to the wing
    }

    /// @brief Calculates the chord length at a given eta station using linear interpolation.
    /// @param etaStation The dimensionless spanwise station (0 at root, 1 at tip) where the chord length is to be evaluated.
    /// @return The chord length at the specified eta station.
    double static getChordAtEtaStation(double etaStation, TypeOfWing typeOfWing = TypeOfWing::UNKNOWN)
    {
        int j = 0;

        switch (typeOfWing)
        {

        case TypeOfWing::STRAIGHT_TAPERED:

        {

            std::vector<double> etaStationInboard;
            std::vector<double> etaStationMid;
            std::vector<double> etaStationOutboard;
            std::vector<double> chordInboard;
            std::vector<double> chordMid;
            std::vector<double> chordOutboard;

            if (wingPtr->averageLeadingEdgeSweep == 0.0)
            {

                for (size_t i = 0; i < wingPtr->panelsYStation.size(); ++i)
                {

                    if (i == 0) 
                    {
                        etaStationInboard.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        j += 1;
                        chordInboard.push_back(wingPtr->croot[j - 1]);
                    }

                    else if ((wingPtr->croot[j] == wingPtr->croot[j - 1]) && i <= wingPtr->croot.size() - 1)
                    { 

                        etaStationInboard.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        chordInboard.push_back(wingPtr->croot[j]);
                        j += 1;
                    }
                    else if ((wingPtr->croot[j] != wingPtr->croot[j - 1]) && i <= wingPtr->croot.size() - 1)  //Mid and Outboard region
                    {
                        etaStationMid.push_back(2 * wingPtr->panelsYStation[i - 1] / wingPtr->totalProjectedSpan);
                        chordMid.push_back(wingPtr->croot[j - 1]);
                        etaStationMid.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        chordMid.push_back(wingPtr->croot[j]);
                        etaStationOutboard.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        chordOutboard.push_back(wingPtr->croot[j]);
                    }

                    else if (i > wingPtr->croot.size() - 1) // If we have reached the end of the croot vector, we are in the outboard region
                    {
                        etaStationOutboard.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        chordOutboard.push_back(wingPtr->ctip[i-1]);
                    }
                }

                

                double inboardMin = etaStationInboard.front();
                double inboardMidMin = etaStationMid.front();
                double inboardMax = etaStationInboard.back();

                double outboardMin = etaStationOutboard.front();
                double outboardMidMax = etaStationMid.back();
                double outboardMax = etaStationOutboard.back();

                bool foundInboard = (etaStation >= inboardMin - eps && etaStation <= inboardMax - eps);
                bool foundMid     = (etaStation >= inboardMidMin - eps && etaStation <= outboardMidMax - eps);
                bool foundOutboard = (etaStation >= outboardMin - eps && etaStation <= outboardMax - eps);

                if (foundInboard)
                {
                    Interpolant chordInterp(etaStationInboard, chordInboard, 1, RegressionMethod::LINEAR);
                    return chordInterp.getYValueFromRegression(etaStation);
                }
                else if (foundMid)
                {
                    Interpolant chordInterp(etaStationMid, chordMid, 1, RegressionMethod::LINEAR);
                    return chordInterp.getYValueFromRegression(etaStation);
                }
                else if (foundOutboard)
                {
                    Interpolant chordInterp(etaStationOutboard, chordOutboard, 1, RegressionMethod::LINEAR);
                    return chordInterp.getYValueFromRegression(etaStation);
                }
                else
                {
                    throw std::invalid_argument("Eta station not found in either inboard or outboard stations.");
                }
            }

            else
            {

                std::vector<double> etaStationVec;
                std::vector<double> etaStationMid;
                std::vector<double> chords;

                for (size_t i = 0; i < wingPtr->panelsYStation.size(); ++i)
                {

                    if (i == 0) 
                    {
                        etaStationVec.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        j += 1;
                        chords.push_back(wingPtr->croot[j - 1]);
                    }

                    else if (i <= wingPtr->croot.size() - 1)
                    { 

                        etaStationVec.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        chords.push_back(wingPtr->croot[j]);
                        j += 1;
                    }

                    else if (i > wingPtr->croot.size() - 1) 
                    {
                        etaStationVec.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        chords.push_back(wingPtr->ctip[i - 1]);
                    }
                }

                double etaStationVecMin = etaStationInboard.front();
                double etaStationVecMax = etaStationInboard.back();

                bool foundEta = (etaStation >= etaStationVecMin - eps && etaStation <= etaStationVecMax - eps);
               

                Interpolant chordInterp(etaStationVec, chords, 1, RegressionMethod::LINEAR);

                return foundEta ? chordInterp.getYValueFromRegression(etaStation) : throw std::invalid_argument("Eta station not found in the stations vector.");
            }
        }

        break;

        case TypeOfWing::CRANKED:

        {

            std::vector<double> etaStationInboard;
            std::vector<double> etaStationOutboard;
            std::vector<double> chordInboard;
            std::vector<double> chordOutboard;

            bool addedKinkStation = false;

            
                for (size_t i = 0; i < wingPtr->panelsYStation.size(); ++i)
                {

                    if (i == 0 || (2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan <= 2* wingPtr->kinkStation/ wingPtr->totalProjectedSpan)) 
                    { 
                        etaStationInboard.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        j += 1;
                        chordInboard.push_back(wingPtr->croot[j - 1]);
                    }

                    else if ((2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan > 2* wingPtr->kinkStation/ wingPtr->totalProjectedSpan))
                    {
                        if (!addedKinkStation)
                        {
                            etaStationOutboard.push_back(2 * wingPtr->kinkStation / wingPtr->totalProjectedSpan);
                            chordOutboard.push_back(wingPtr->taperInbord * wingPtr->croot.front());

                            addedKinkStation = true;
                        }

                        etaStationOutboard.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        chordOutboard.push_back(wingPtr->croot[j]);
                        j += 1;
                    }
                   
                    else if (i > wingPtr->croot.size() - 1) // If we have reached the end of the croot vector, we are in the outboard region
                    {
                        etaStationOutboard.push_back(2 * wingPtr->panelsYStation[i] / wingPtr->totalProjectedSpan);
                        chordOutboard.push_back(wingPtr->ctip[i - 1]);
                    }
                }

                // Dopo aver popolato etaStationInboard e etaStationOutboard:

                double inboardMin = etaStationInboard.front();
                double inboardMax = etaStationInboard.back();

                double outboardMin = etaStationOutboard.front();
                double outboardMax = etaStationOutboard.back();

                bool foundInboard = (etaStation >= inboardMin - eps && etaStation <= inboardMax - eps);
                bool foundOutboard = (etaStation >= outboardMin - eps && etaStation <= outboardMax - eps);

                if (foundInboard)
                {
                    Interpolant chordInterp(etaStationInboard, chordInboard, 1, RegressionMethod::LINEAR);
                    return chordInterp.getYValueFromRegression(etaStation);
                }
                else if (foundOutboard)
                {
                    Interpolant chordInterp(etaStationOutboard, chordOutboard, 1, RegressionMethod::LINEAR);
                    return chordInterp.getYValueFromRegression(etaStation);
                }
                else
                {
                    throw std::invalid_argument("Eta station not found in either inboard or outboard stations.");
                }
          
        }
        }

        return 0.0;
    }
};
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


class RestoreSettings {



    private:
    VSP::AeroSettings settingsToRestore;
    VSP::Aircraft aircrfatInfoToRestore;



    public:

    RestoreSettings() = default;


    inline void setSavePrevoiusSettings(VSP::AeroSettings settings) {
        this->settingsToRestore = settings;
    }

    inline void setSavePrevoiusAircraftInfo(VSP::Aircraft aircraftInfo) {
        this->aircrfatInfoToRestore = aircraftInfo;
    }


    // Getters
    VSP::AeroSettings getSettingsToRestore() const {
        return settingsToRestore;
    }

    VSP::Aircraft getAircrfatInfoToRestore() const {
        return aircrfatInfoToRestore;
    }



};
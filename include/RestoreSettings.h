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

    /**
     * @brief Default constructor.
     */
    RestoreSettings() = default;


    /**
     * @brief Stores aerodynamic settings for later restore operations.
     * @param settings Aerodynamic settings to save.
     */
    inline void setSavePrevoiusSettings(VSP::AeroSettings settings) {
        this->settingsToRestore = settings;
    }

    /**
     * @brief Stores aircraft information for later restore operations.
     * @param aircraftInfo Aircraft information to save.
     */
    inline void setSavePrevoiusAircraftInfo(VSP::Aircraft aircraftInfo) {
        this->aircrfatInfoToRestore = aircraftInfo;
    }


    /**
     * @brief Returns the stored aerodynamic settings.
     * @return Saved aerodynamic settings.
     */
    VSP::AeroSettings getSettingsToRestore() const {
        return settingsToRestore;
    }

    /**
     * @brief Returns the stored aircraft information.
     * @return Saved aircraft information.
     */
    VSP::Aircraft getAircrfatInfoToRestore() const {
        return aircrfatInfoToRestore;
    }



};
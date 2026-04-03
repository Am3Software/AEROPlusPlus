#pragma once
#include <string>

enum class AircraftEngineCategory
{
    SINGLE_ENGINE,
    TWIN_ENGINE,
    MULTI_PROPELLER_ENGINE,
    JET_ENGINE,
    UNKNOWN
};

// Funzione helper per convertire stringa in enum
AircraftEngineCategory stringToAircraftEngineCategory(const std::string& comp)
{
    if (comp == "single_engine" || comp == "Single_Engine"){
        return AircraftEngineCategory::SINGLE_ENGINE;
    }
    if (comp == "twin_engine" || comp == "Twin_Engine"){
        return AircraftEngineCategory::TWIN_ENGINE;
    }
    if (comp == "multi_propeller_engine" || comp == "Multi_Propeller_Engine"){
        return AircraftEngineCategory::MULTI_PROPELLER_ENGINE;
    }
    if (comp == "jet_engine" || comp == "Jet_Engine"){
        return AircraftEngineCategory::JET_ENGINE;
    }

    return AircraftEngineCategory::UNKNOWN;
}
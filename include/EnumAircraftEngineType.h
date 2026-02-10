#pragma once
#include <string>

enum class AircraftEngineType
{
    SINGLE_ENGINE,
    TWIN_ENGINE,
    MULTI_PROPELLER_ENGINE,
    JET,
    UNKNOWN
};

// Funzione helper per convertire stringa in enum
AircraftEngineType stringToAircraftEngineType(const std::string& comp)
{
    if (comp == "single_engine" || comp == "Single_Engine"){
        return AircraftEngineType::SINGLE_ENGINE;
    }
    if (comp == "twin_engine" || comp == "Twin_Engine"){
        return AircraftEngineType::TWIN_ENGINE;
    }
    if (comp == "multi_propeller_engine" || comp == "Multi_Propeller_Engine"){
        return AircraftEngineType::MULTI_PROPELLER_ENGINE;
    }
    if (comp == "jet" || comp == "Jet"){
        return AircraftEngineType::JET;
    }
    return AircraftEngineType::UNKNOWN;
}
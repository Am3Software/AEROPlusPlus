#pragma once
#include <string>

enum class AircraftEngineType
{
    SINGLE_ENGINE,
    TWIN_ENGINE,
    MULTI_PROPELLER_ENGINE,
    PISTON_PROPELLER,
    TURBOPROP,
    HYBRID,
    ELECTRIC,
    JET_TURBOFAN,
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
    if (comp == "jet_turbfan" || comp == "Jet_Turbofan"){
        return AircraftEngineType::JET_TURBOFAN;
    }
    if (comp == "piston_propeller" || comp == "Piston_Propeller"){
        return AircraftEngineType::PISTON_PROPELLER;
    }
    if (comp == "turboprop" || comp == "Turboprop"){
        return AircraftEngineType::TURBOPROP;
    }
    if (comp == "hybrid" || comp == "Hybrid"){
        return AircraftEngineType::HYBRID;
    }
    if (comp == "electric" || comp == "Electric"){
        return AircraftEngineType::ELECTRIC;
    }
    return AircraftEngineType::UNKNOWN;
}
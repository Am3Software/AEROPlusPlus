#pragma once
#include <string>

enum class AircraftEngineType
{
   
    PISTON_PROPELLER,
    TURBOPROP,
    TURBOCHARGED,
    HYBRID,
    ELECTRIC,
    JET_TURBOFAN,
    UNKNOWN
};

// Funzione helper per convertire stringa in enum
AircraftEngineType stringToAircraftEngineType(const std::string& comp)
{
    if (comp == "jet_turbfan" || comp == "Jet_Turbofan"){
        return AircraftEngineType::JET_TURBOFAN;
    }
    if (comp == "piston_propeller" || comp == "Piston_Propeller"){
        return AircraftEngineType::PISTON_PROPELLER;
    }
    if (comp == "turbocharged" || comp == "Turbocharged"){
        return AircraftEngineType::TURBOCHARGED;
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
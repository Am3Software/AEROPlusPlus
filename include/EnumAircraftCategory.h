#pragma once
#include <string>

enum class AircraftCategory
{
    FIGHTER,
    TRANSPORT_JET,
    GENERAL_AVIATION,
    UAV,
    UNKNOWN
};

// Funzione helper per convertire stringa in enum
AircraftCategory stringToAircraftCategory(const std::string& cat)
{
    if (cat == "Fighter" || cat == "fighter"){
        return AircraftCategory::FIGHTER;
    }
    if (cat == "Transport_Jet" || cat == "transport_jet") {
        return AircraftCategory::TRANSPORT_JET;
    }
    if (cat == "GA" || cat == "general aviation") {
        return AircraftCategory::GENERAL_AVIATION;
    }
    if (cat == "UAV" ) {
        return AircraftCategory::UAV;
    }
    return AircraftCategory::UNKNOWN;
}
#pragma once
#include <string>

enum class TypeOfInlet
{
    SCOOP_INLET,
    ANULAR_INLET,
    UNKNOWN
};

// Funzione helper per convertire stringa in enum
TypeOfInlet stringToTypeOfInlet(const std::string& inletType)
{
    if (inletType == "Scoop_Inlet" || inletType == "scoop_inlet") {
        return TypeOfInlet::SCOOP_INLET;
    }
    if (inletType == "Anular_Inlet" || inletType == "anular_inlet") {
        return TypeOfInlet::ANULAR_INLET;
    }
    return TypeOfInlet::UNKNOWN;
}
    
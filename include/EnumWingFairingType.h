#pragma once
#include <string>

enum class WingFairingType
{
    UNFILLETED,         // IF = 1.1 – 1.4
    FILLETED_FAIRING,   // IF = 1.0 – 1.1
    CAREFULLY_DESIGNED, // IF = 1.0
    UNKNOWN

};

// Funzione helper per convertire stringa in enum
WingFairingType stringToWingFairingType(const std::string &comp)
{
    if (comp == "unfilleted" || comp == "Unfilleted")
    {
        return WingFairingType::UNFILLETED;
    }
    if (comp == "filleted" || comp == "Filleted")
    {
        return WingFairingType::FILLETED_FAIRING;
    }
    if (comp == "carefully_designed" || comp == "Carefully_Designed")
    {
        return WingFairingType::CAREFULLY_DESIGNED;
    }

    return WingFairingType::UNKNOWN;
}
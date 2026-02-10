#pragma once
#include <string>

enum class WingPosition
{
    LOW_WING,
    MID_WING,
    HIGH_WING,
    UNKNOWN

};

// Funzione helper per convertire stringa in enum
WingPosition stringToWingPosition(const std::string &comp)
{
    if (comp == "low_wing" || comp == "Low_Wing")
    {
        return WingPosition::LOW_WING;
    }
    if (comp == "mid_wing" || comp == "Mid_Wing")
    {
        return WingPosition::MID_WING;
    }
    if (comp == "high_wing" || comp == "High_Wing")
    {
        return WingPosition::HIGH_WING;
    }

    return WingPosition::UNKNOWN;
}
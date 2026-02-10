
#pragma once

#include <string>
enum class UndercarriagePosition
{
    BODY_MOUNTED,
    WING_MOUNTED,
    UNKNOWN

};

// Funzione helper per convertire stringa in enum
UndercarriagePosition stringToUndercarriagePosition(const std::string &comp)
{
    if (comp == "body_mounted" || comp == "Body_Mounted")
    {
        return UndercarriagePosition::BODY_MOUNTED;
    }
    if (comp == "wing_mounted" || comp == "Wing_Mounted")
    {
        return UndercarriagePosition::WING_MOUNTED;
    }

    return UndercarriagePosition::UNKNOWN;
}

#pragma once

enum class EnginePosition
{
    BODY_MOUNTED,
    WING_MOUNTED,
    UNKNOWN
   
};

// Funzione helper per convertire stringa in enum
EnginePosition stringToEnginePosition(const std::string& comp)
{
    if (comp == "body_mounted" || comp == "Body_Mounted"){
        return EnginePosition::BODY_MOUNTED;
    }

    if (comp == "wing_mounted" || comp == "Wing_Mounted") {
        return EnginePosition::WING_MOUNTED;
    }
   
    return EnginePosition::UNKNOWN;
}
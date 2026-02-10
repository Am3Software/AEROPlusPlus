#pragma once

#include <string>

enum class WeightMethod
{
    WING_TORENBEEK,
    WING_SADRAEY,
    WING_CHIOZZOTTO,
    FUSELAGE_TORENBEEK,
    FUSELAGE_TORENBEEK_MATERIAL,
    UNKNOWN
};

// Funzione helper per convertire stringa in enum
WeightMethod stringToWeightMethod(const std::string& comp)
{
    if (comp == "wing_torenbeek" || comp == "Wing_Torenbeek"){
        
        return WeightMethod::WING_TORENBEEK;
    }
    if (comp == "wing_sadraey" || comp == "Wing_Sadraey") {

        return WeightMethod::WING_SADRAEY;
    }
    if (comp == "wing_chiozzotto" || comp == "Wing_Chiozzotto") {

        return WeightMethod::WING_CHIOZZOTTO;
    }
    if (comp == "fuselage_torenbeek" || comp == "Fuselage_Torenbeek") {

        return WeightMethod::FUSELAGE_TORENBEEK;
    }
    if (comp == "fuselage_torenbeek_material" || comp == "Fuselage_Torenbeek_Material") {
        
        return WeightMethod::FUSELAGE_TORENBEEK_MATERIAL;
    }
    
    return WeightMethod::UNKNOWN;
}
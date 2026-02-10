
#pragma once

enum class Material
{
    ALLUMINIUM,
    TITANIUM,
    CARBON_FIBER,
    GLASS_FIBER,
    KEVLAR,
    UNKNOWN

};

// Funzione helper per convertire stringa in enum
Material stringToMaterial(const std::string &comp)
{
    if (comp == "alluminium" || comp == "Alluminium")
    {
        return Material::ALLUMINIUM;
    }
    if (comp == "titanium" || comp == "Titanium")
    {
        return Material::TITANIUM;
    }
    if (comp == "carbon_fiber" || comp == "Carbon_Fiber")
    {
        return Material::CARBON_FIBER;
    }
    if (comp == "glass_fiber" || comp == "Glass_Fiber")
    {
        return Material::GLASS_FIBER;
    }
    if (comp == "kevlar" || comp == "Kevlar")
    {
        return Material::KEVLAR;
    }

    return Material::UNKNOWN;
}
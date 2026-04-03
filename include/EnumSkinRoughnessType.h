#pragma once
#include <string>

enum class SkinRoughnessType
{
   
    CAMOUFLAGE_OR_ALUMINIUM,
    SMOOTH_PAINT,
    PRODUCTION_SHEET_METAL,
    POLISHED_SHEET_METAL,
    SMOOTH_MOLDED_COMPOSITE,
    UNKNOWN
   
};

// Funzione helper per convertire stringa in enum
SkinRoughnessType stringToSkinRoughnessType(const std::string& comp)
{
    if (comp == "camouflage_or_aluminium" || comp == "Camouflage_or_Aluminium"){
        return SkinRoughnessType::CAMOUFLAGE_OR_ALUMINIUM;
    }
    if (comp == "smooth_paint" || comp == "Smooth_Paint"){
        return SkinRoughnessType::SMOOTH_PAINT;
    }
    if (comp == "production_sheet_metal" || comp == "Production_Sheet_Metal"){
        return SkinRoughnessType::PRODUCTION_SHEET_METAL;
    }
    if (comp == "polished_sheet_metal" || comp == "Polished_Sheet_Metal"){
        return SkinRoughnessType::POLISHED_SHEET_METAL;
    }
    if (comp == "smooth_molded_composite" || comp == "Smooth_Molded_Composite"){
        return SkinRoughnessType::SMOOTH_MOLDED_COMPOSITE;
    }
    return SkinRoughnessType::UNKNOWN;
}

#pragma once

#include <string>

enum class TypeOfStabilizer
{
    FIXED,
    UNFIXED,
    UNKNOWN
};

// Funzione helper per convertire stringa in enum
TypeOfStabilizer  stringToTypeOfStabilizer(const std::string& cat)
{
    if (cat == "Fixed" || cat == "fixed") {
        return TypeOfStabilizer::FIXED;
    }
    if (cat == "Unfixed" || cat == "unfixed") {
        return TypeOfStabilizer::UNFIXED;
    }
    return TypeOfStabilizer::UNKNOWN;
}
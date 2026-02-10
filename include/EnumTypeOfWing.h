#pragma once
#include <string>

enum class TypeOfWing
{
    STRAIGHT_TAPERED,
    CRANKED,
    UNKNOWN
};

// Funzione helper per convertire stringa in enum
TypeOfWing stringToTypeOfWing(const std::string& wing)
{
    if (wing == "straight_tapered" || wing == "Straight_Tapered") {
    return TypeOfWing::STRAIGHT_TAPERED;
    }
    if (wing == "cranked" || wing == "Cranked") {
    return TypeOfWing::CRANKED;
    }
    return TypeOfWing::UNKNOWN;
}
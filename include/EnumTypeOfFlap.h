#pragma once
#include <string>

enum class TypeOfFlap
{
    SLOTTED_FLAP,
    SPLIT_FLAP,
    PLAIN_FLAP,
    UNKNOWN

};

// Funzione helper per convertire stringa in enum
TypeOfFlap stringToTypeOfFlap(const std::string &comp)
{
    if (comp == "slotted" || comp == "Slo" || comp == "SLOTTED")
    {
        return TypeOfFlap::SLOTTED_FLAP;
    }
    if (comp == "split" || comp == "SPLIT")
    {
        return TypeOfFlap::SPLIT_FLAP;
    }
    if (comp == "plain" || comp == "PLAIN")
    {
        return TypeOfFlap::PLAIN_FLAP;
    }

    return TypeOfFlap::UNKNOWN;
}
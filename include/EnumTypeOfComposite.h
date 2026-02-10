
#pragma once

#include <string>

enum class TypeOfComposite
{
    FIBERGLASS,
    CARBONFIBER,
    KEVLAR,
    UNKNOWN
};

// Funzione helper per convertire stringa in enum
TypeOfComposite stringToComposite(const std::string& comp)
{
    if (comp == "fiberglass" || comp == "Fiberglass") {
        return TypeOfComposite::FIBERGLASS;
    }
    if (comp == "carbonfiber" || comp == "CarbonFiber") {
        return TypeOfComposite::CARBONFIBER;
    }
    if (comp == "kevlar" || comp == "Kevlar") {
        return TypeOfComposite::KEVLAR;
    }
    return TypeOfComposite::UNKNOWN;
}
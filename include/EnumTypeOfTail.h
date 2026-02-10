#pragma once

#include <string>

enum class TypeOfTail
{
    CONVENTIONAL_TAIL,
    T_TAIL,
    V_TAIL,
    V_REV_TAIL,
    H_TAIL,
    U_TAIL,
    UNKNOWN
};

// Funzione helper per convertire stringa in enum
TypeOfTail stringToTypeOfTail(const std::string& comp)
{
    if (comp == "conventional_tail" || comp == "Conventional_Tail"){
        
        return TypeOfTail::CONVENTIONAL_TAIL;
    }
    if (comp == "t_tail" || comp == "T_Tail") {
        return TypeOfTail::T_TAIL;
    }
    if (comp == "v_tail" || comp == "V_Tail") {
        return TypeOfTail::V_TAIL;
    }
    if (comp == "v_rev_tail" || comp == "V_Rev_Tail") {
        return TypeOfTail::V_REV_TAIL;
    }
    if (comp == "h_tail" || comp == "H_Tail") {
        return TypeOfTail::H_TAIL;
    }
    if (comp == "u_tail" || comp == "U_Tail") {
        return TypeOfTail::U_TAIL;
    }
    return TypeOfTail::UNKNOWN;
}
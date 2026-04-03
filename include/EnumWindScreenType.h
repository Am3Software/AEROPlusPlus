#pragma once
#include <string>

enum class WindScreenType
{

    FLAT_WINDSCREEN_WITH_PROTRUDING_FRAME,
    FLAT_WINDSCREEN_WITH_FLUSH_FRAME,
    CURVED_WINDSCREEN_WITH_SHARP_UPPER_EDGE,
    CURVED_WINDSCREEN_WITH_ROUNDED_UPPER_EDGE,
    UNKNOWN

};

// Funzione helper per convertire stringa in enum
WindScreenType stringToWindScreenType(const std::string &comp)
{
    if (comp == "flat_with_protruding_frame" || comp == "Flat_With_Protruding_Frame")
    {
        return WindScreenType::FLAT_WINDSCREEN_WITH_PROTRUDING_FRAME;
    }
    if (comp == "flat_with_flush_frame" || comp == "Flat_With_Flush_Frame")
    {
        return WindScreenType::FLAT_WINDSCREEN_WITH_FLUSH_FRAME;
    }
    if (comp == "curved_with_sharp_upper_edge" || comp == "Curved_With_Sharp_Upper_Edge")
    {
        return WindScreenType::CURVED_WINDSCREEN_WITH_SHARP_UPPER_EDGE;
    }
    if (comp == "curved_with_rounded_upper_edge" || comp == "Curved_With_Rounded_Upper_Edge")
    {
        return WindScreenType::CURVED_WINDSCREEN_WITH_ROUNDED_UPPER_EDGE;
    }

    return WindScreenType::UNKNOWN;
}
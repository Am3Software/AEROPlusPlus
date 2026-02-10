#pragma once

#include <string>
#include <algorithm>
#include <cctype>
#include <cstring>

// Aggiungi questa funzione helper nella sezione private della classe:
inline bool equalsIgnoreCase(const std::string& str1, const std::string& str2) {
    #ifdef _WIN32
        return _stricmp(str1.c_str(), str2.c_str()) == 0;
    #else
        return strcasecmp(str1.c_str(), str2.c_str()) == 0;
    #endif
}

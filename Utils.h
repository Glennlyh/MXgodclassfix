#pragma once
#include <string>
#include <cctype>
#include "Time.h"


inline std::string trim(const std::string& s) {
    size_t i = 0, j = s.size();
    while (i < j && isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && isspace(static_cast<unsigned char>(s[j - 1]))) --j;
    return s.substr(i, j - i);
}

inline int parseTimeToMinutes(std::string s) {
    return Time::parse(s).minutes();
}

inline std::string minutesToHHMM12(int mins) {
    return Time(mins).hhmm12(); 
}

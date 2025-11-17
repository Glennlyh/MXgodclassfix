#include "InputParser.h"
#include "Utils.h"
#include <cctype>
#include <algorithm>

using namespace std;

// ============================================================================
// Parse group size from string (1-10)
// ============================================================================

int InputParser::parseGroupSize(string s) {
    s = trim(s);
    if (s.empty())
        return 1;

    int g = 1;
    try { 
        g = stoi(s); 
    }
    catch (...) { 
        g = 1; 
    }
    
    if (g < 1)
        g = 1;
    if (g > 10)
        g = 10;

    return g;
}

// ============================================================================
// Parse freight type from string
// ============================================================================

FreightType InputParser::parseFreightType(string s) {
    s = trim(s);
    
    // Convert to lowercase
    for (char& ch : s)
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
    
    if (s == "12" || s == "megacarrier" || s == "mega")
        return FreightType::MegaCarrier;
    if (s == "6" || s == "cargocruiser" || s == "cruiser")
        return FreightType::CargoCruiser;
    if (s == "2" || s == "minimover" || s == "mini")
        return FreightType::MiniMover;

    // default 
    return FreightType::MiniMover;
}

// ============================================================================
// Create CSV filename from base name
// ============================================================================

string InputParser::createCSVFilename(string base) {
    base = trim(base);
    
    if (base.size() >= 4 && base.substr(base.size() - 4) == ".txt") {
        base = base.substr(0, base.size() - 4) + ".csv";
    }
    else if (base.size() >= 4 && base.substr(base.size() - 4) == ".csv") {
        // Already has .csv extension
    }
    else {
        base += ".csv";
    }
    
    return base;
}

#pragma once
#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <string>
#include "Freight.h"

/**
 * InputParser - Handles validation and parsing of user inputs
 * Responsibility: Parse and validate user input strings
 */
class InputParser {
public:
    // Parse group size from string (1-10)
    static int parseGroupSize(std::string s);
    
    // Parse freight type from string
    static FreightType parseFreightType(std::string s);
    
    // Create CSV filename from base name
    static std::string createCSVFilename(std::string base);
};

#endif

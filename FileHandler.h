#pragma once
#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include "FreightManager.h"
#include "CargoManager.h"

/**
 * FileHandler - Handles file loading and saving coordination
 * Responsibility: Coordinate file I/O operations with managers
 */
class FileHandler {
public:
    // Load freight and cargo files
    static void loadFiles(FreightManager& fm, CargoManager& cm);
    
    // Save schedule and CSV files
    static void saveSchedule(const FreightManager& fm, 
                            const CargoManager& cm, 
                            const std::string& txtFilename);
};

#endif

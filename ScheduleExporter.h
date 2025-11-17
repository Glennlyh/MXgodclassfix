#pragma once
#ifndef SCHEDULEEXPORTER_H
#define SCHEDULEEXPORTER_H

#include <vector>
#include <string>
#include "Freight.h"
#include "Cargo.h"

/**
 * ScheduleExporter - Handles all file I/O operations for schedules
 * Responsibility: Export schedules to text and CSV files
 */
class ScheduleExporter {
public:
    // Save schedule to text file
    static bool saveScheduleToFile(const std::vector<Freight>& freights,
                                   const std::vector<Cargo>& cargos,
                                   const std::string& outFile);

    // Save schedule to CSV file (legacy format)
    static bool saveScheduleToCSV(const std::vector<Freight>& freights,
                                  const std::vector<Cargo>& cargos,
                                  const std::string& outFile);

    // Save plan sorted by cargo time to CSV
    static bool savePlanByCargoTimeCSV(const std::vector<Freight>& freights,
                                       const std::vector<Cargo>& cargos,
                                       const std::string& outFile);

    // Save plan sorted by minimum freight to CSV
    static bool savePlanByMinimumFreightCSV(const std::vector<Freight>& freights,
                                            const std::vector<Cargo>& cargos,
                                            const std::string& outFile);
};

#endif

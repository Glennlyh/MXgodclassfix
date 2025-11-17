#pragma once
#ifndef MATCHINGENGINE_H
#define MATCHINGENGINE_H

#include <vector>
#include <string>
#include <utility>

#include "Freight.h"
#include "Cargo.h"
#include "Utils.h"
#include "MatchingAlgorithm.h"
#include "SchedulePrinter.h"
#include "ScheduleExporter.h"

/**
 * MatchingEngine - Backward compatibility wrapper
 * Delegates to specialized classes: MatchingAlgorithm, SchedulePrinter, ScheduleExporter
 * 
 * This class is maintained for backward compatibility.
 * New code should use the specialized classes directly.
 */
class MatchingEngine {
public:
    static constexpr int ARRIVAL_EARLY_MIN = 15;

    // Part 1 (legacy maximum matching) - delegates to MatchingAlgorithm
    static std::pair<std::vector<int>, std::vector<int>>
        computeMaximumMatching(const std::vector<Freight>& freights,
                               const std::vector<Cargo>& cargos);

    static void printSummary(const std::vector<Freight>& freights,
                             const std::vector<Cargo>& cargos);

    // File I/O - delegates to ScheduleExporter
    static bool saveScheduleToFile(const std::vector<Freight>& freights,
                                   const std::vector<Cargo>& cargos,
                                   const std::string& outFile);

    static bool saveScheduleToCSV(const std::vector<Freight>& freights,
                                  const std::vector<Cargo>& cargos,
                                  const std::string& outFile);

    // Printing - delegates to SchedulePrinter
    static void printArrivalPrioritySummary(const std::vector<Freight>& freights, 
                                            const std::vector<Cargo>& cargos);

    static void printCapacityFirstSummary(const std::vector<Freight>& freights,
                                          const std::vector<Cargo>& cargos);

    static void printPlanSortedByCargoTime(const std::vector<Freight>& freights,
                                           const std::vector<Cargo>& cargos);

    static void printPlanSortedByMinimumFreight(const std::vector<Freight>& freights, 
                                                const std::vector<Cargo>& cargos);

    static void printFreightsNotFull(const std::vector<Freight>& freights,
                                     const std::vector<Cargo>& cargos);

    static void printUnassignedCargos(const std::vector<Freight>& freights,
                                      const std::vector<Cargo>& cargos);

    // CSV Export - delegates to ScheduleExporter
    static bool savePlanByCargoTimeCSV(const std::vector<Freight>& freights,
                                       const std::vector<Cargo>& cargos,
                                       const std::string& outFile);

    static bool savePlanByMinimumFreightCSV(const std::vector<Freight>& freights,
                                            const std::vector<Cargo>& cargos,
                                            const std::string& outFile);
};

#endif

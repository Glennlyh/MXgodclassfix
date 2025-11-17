#pragma once
#ifndef MATCHINGENGINE_H
#define MATCHINGENGINE_H

#include <vector>
#include <string>
#include <utility>

#include "Freight.h"
#include "Cargo.h"
#include "Utils.h"

class MatchingEngine {
public:
    static constexpr int ARRIVAL_EARLY_MIN = 15;

    // Part 1 (legacy maximum matching)
    static std::pair<std::vector<int>, std::vector<int>>
        computeMaximumMatching(const std::vector<Freight>& freights,
                               const std::vector<Cargo>& cargos);

    static void printSummary(const std::vector<Freight>& freights,
                             const std::vector<Cargo>& cargos);

    static bool saveScheduleToFile(const std::vector<Freight>& freights,
                                   const std::vector<Cargo>& cargos,
                                   const std::string& outFile);

    static bool saveScheduleToCSV(const std::vector<Freight>& freights,
                                  const std::vector<Cargo>& cargos,
                                  const std::string& outFile);

    // Part 5 (arrival priority)
    static void printArrivalPrioritySummary(const std::vector<Freight>& freights, 
                                            const std::vector<Cargo>& cargos);

    // Part 6 (capacity-first packing overview)
    static void printCapacityFirstSummary(const std::vector<Freight>& freights,
                                          const std::vector<Cargo>& cargos);

    // Part 7 (plan view)
    static void printPlanSortedByCargoTime(const std::vector<Freight>& freights,
                                           const std::vector<Cargo>& cargos);

    static void printPlanSortedByMinimumFreight(const std::vector<Freight>& freights, 
                                                const std::vector<Cargo>& cargos);

    static void printFreightsNotFull(const std::vector<Freight>& freights,
                                     const std::vector<Cargo>& cargos);

    static void printUnassignedCargos(const std::vector<Freight>& freights,
                                      const std::vector<Cargo>& cargos);

    // Part 8 (export csv)
    static bool savePlanByCargoTimeCSV(const std::vector<Freight>& freights,
                                       const std::vector<Cargo>& cargos,
                                       const std::string& outFile);

    static bool savePlanByMinimumFreightCSV(const std::vector<Freight>& freights,
                                            const std::vector<Cargo>& cargos,
                                            const std::string& outFile);
};

#endif

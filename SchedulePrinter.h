#pragma once
#ifndef SCHEDULEPRINTER_H
#define SCHEDULEPRINTER_H

#include <vector>
#include "Freight.h"
#include "Cargo.h"

/**
 * SchedulePrinter - Handles all schedule display and formatting
 * Responsibility: Print schedules in various formats to console
 */
class SchedulePrinter {
public:
    // Print arrival-priority summary
    static void printArrivalPrioritySummary(const std::vector<Freight>& freights, 
                                            const std::vector<Cargo>& cargos);

    // Print capacity-first summary
    static void printCapacityFirstSummary(const std::vector<Freight>& freights,
                                          const std::vector<Cargo>& cargos);

    // Print plan sorted by cargo arrival time
    static void printPlanSortedByCargoTime(const std::vector<Freight>& freights,
                                           const std::vector<Cargo>& cargos);

    // Print plan sorted by minimum freight
    static void printPlanSortedByMinimumFreight(const std::vector<Freight>& freights, 
                                                const std::vector<Cargo>& cargos);

    // Print freights not at full capacity
    static void printFreightsNotFull(const std::vector<Freight>& freights,
                                     const std::vector<Cargo>& cargos);

    // Print unassigned cargos
    static void printUnassignedCargos(const std::vector<Freight>& freights,
                                      const std::vector<Cargo>& cargos);
};

#endif

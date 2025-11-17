#include "MatchingEngine.h"
#include "MatchingAlgorithm.h"
#include "SchedulePrinter.h"
#include "ScheduleExporter.h"
#include <iostream>

using namespace std;

// ============================================================================
// MatchingEngine - Backward compatibility wrapper
// All methods delegate to specialized classes
// ============================================================================

pair<vector<int>, vector<int>>
MatchingEngine::computeMaximumMatching(const vector<Freight>& freights, const vector<Cargo>& cargos) {
    return MatchingAlgorithm::computeMaximumMatching(freights, cargos);
}

void MatchingEngine::printSummary(const vector<Freight>& freights,
                                  const vector<Cargo>& cargos) {
    // Legacy method - prints basic matching summary
    pair<vector<int>, vector<int>> result = computeMaximumMatching(freights, cargos);
    const vector<int>& fAssigned = result.first;
    const vector<int>& cAssigned = result.second;

    cout << "=== Schedule Summary ===\n";
    for (size_t i = 0; i < freights.size(); ++i) {
        const auto& f = freights[i];
        cout << "Freight " << f.getId() << " (" << f.getDest() << ") -> ";
        if (fAssigned[i] == -1) {
            cout << "NONE\n";
        }
        else {
            const auto& c = cargos[fAssigned[i]];
            cout << "Cargo " << c.getId() << " (" << c.getDest() << ")\n";
        }
    }

    cout << "\nUnassigned Cargo:\n";
    for (size_t j = 0; j < cargos.size(); ++j) {
        if (cAssigned[j] == -1) {
            cout << "  - " << cargos[j] << "\n";
        }
    }
}

bool MatchingEngine::saveScheduleToFile(const vector<Freight>& freights,
                                       const vector<Cargo>& cargos,
                                       const string& outFile) {
    return ScheduleExporter::saveScheduleToFile(freights, cargos, outFile);
}

bool MatchingEngine::saveScheduleToCSV(const vector<Freight>& freights,
                                      const vector<Cargo>& cargos,
                                      const string& outFile) {
    return ScheduleExporter::saveScheduleToCSV(freights, cargos, outFile);
}

void MatchingEngine::printArrivalPrioritySummary(const vector<Freight>& freights, 
                                                 const vector<Cargo>& cargos) {
    SchedulePrinter::printArrivalPrioritySummary(freights, cargos);
}

void MatchingEngine::printCapacityFirstSummary(const vector<Freight>& freights,
                                               const vector<Cargo>& cargos) {
    SchedulePrinter::printCapacityFirstSummary(freights, cargos);
}

void MatchingEngine::printPlanSortedByCargoTime(const vector<Freight>& freights,
                                                const vector<Cargo>& cargos) {
    SchedulePrinter::printPlanSortedByCargoTime(freights, cargos);
}

void MatchingEngine::printPlanSortedByMinimumFreight(const vector<Freight>& freights, 
                                                     const vector<Cargo>& cargos) {
    SchedulePrinter::printPlanSortedByMinimumFreight(freights, cargos);
}

void MatchingEngine::printFreightsNotFull(const vector<Freight>& freights,
                                         const vector<Cargo>& cargos) {
    SchedulePrinter::printFreightsNotFull(freights, cargos);
}

void MatchingEngine::printUnassignedCargos(const vector<Freight>& freights,
                                          const vector<Cargo>& cargos) {
    SchedulePrinter::printUnassignedCargos(freights, cargos);
}

bool MatchingEngine::savePlanByCargoTimeCSV(const vector<Freight>& freights,
                                           const vector<Cargo>& cargos,
                                           const string& outFile) {
    return ScheduleExporter::savePlanByCargoTimeCSV(freights, cargos, outFile);
}

bool MatchingEngine::savePlanByMinimumFreightCSV(const vector<Freight>& freights,
                                                 const vector<Cargo>& cargos,
                                                 const string& outFile) {
    return ScheduleExporter::savePlanByMinimumFreightCSV(freights, cargos, outFile);
}

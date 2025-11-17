#include "ScheduleExporter.h"
#include "MatchingAlgorithm.h"
#include "Utils.h"
#include <fstream>
#include <algorithm>
#include <numeric>
#include <map>

using namespace std;

// ============================================================================
// Save schedule to text file
// ============================================================================

bool ScheduleExporter::saveScheduleToFile(const vector<Freight>& freights,
                                          const vector<Cargo>& cargos,
                                          const string& outFile) {
    pair<vector<int>, vector<int>> result = MatchingAlgorithm::computeMaximumMatching(freights, cargos);
    const vector<int>& fAssigned = result.first;
    const vector<int>& cAssigned = result.second;

    ofstream out(outFile.c_str());
    if (!out.is_open())
        return false;

    out << "Schedule Summary\n";

    // write freight assignments
    for (size_t i = 0; i < freights.size(); ++i) {
        const auto& f = freights[i];
        out << "Freight " << f.getId() << " (" << f.getDest()
            << " " << minutesToHHMM12(f.getTime()) << ") -> ";

        if (fAssigned[i] == -1) {
            out << "NONE\n";
        }
        else {
            const auto& c = cargos[fAssigned[i]];
            out << "Cargo " << c.getId() << " (" << c.getDest()
                << " deadline " << minutesToHHMM12(c.getDeadline()) << ")\n";
        }
    }

    // Write unassigned cargo
    out << "\nUnassigned Cargo:\n";
    for (size_t j = 0; j < cargos.size(); ++j) {
        if (cAssigned[j] == -1) {
            out << "  - " << cargos[j] << "\n";
        }
    }

    return true;
}

// ============================================================================
// Save schedule to CSV file (legacy format)
// ============================================================================

bool ScheduleExporter::saveScheduleToCSV(const vector<Freight>& freights,
                                         const vector<Cargo>& cargos,
                                         const string& outFile) {
    pair<vector<int>, vector<int>> result = MatchingAlgorithm::computeMaximumMatching(freights, cargos);
    const vector<int>& fAssigned = result.first;
    const vector<int>& cAssigned = result.second;

    ofstream out(outFile.c_str());
    if (!out.is_open()) 
        return false;

    // write csv header
    out << "freight_id,freight_dest,freight_departure,paired_cargo_id,cargo_dest,cargo_deadline\n";

    // write data rows
    for (size_t i = 0; i < freights.size(); ++i) {
        const auto& f = freights[i]; 
        out << f.getId() << "," << f.getDest() << ","
            << minutesToHHMM12(f.getTime()) << ",";

        if (fAssigned[i] == -1) {
            out << "NONE,,\n";
        }
        else {
            int cj = fAssigned[i];
            const auto& c = cargos[cj];
            out << c.getId() << "," << c.getDest() << ","
                << minutesToHHMM12(c.getDeadline()) << "\n";
        }
    }
    return true;
}

// ============================================================================
// Save plan sorted by cargo time to CSV
// ============================================================================

bool ScheduleExporter::savePlanByCargoTimeCSV(const vector<Freight>& freights,
                                              const vector<Cargo>& cargos,
                                              const string& outFile)
{
    // Build packed plan
    vector<MatchingAlgorithm::Row> plan;
    vector<int> capLeft, remain;
    MatchingAlgorithm::buildPackedPlan(freights, cargos, plan, capLeft, remain, true); // Use earliest first

    // sort by cargo deadline, then freight time
    vector<size_t> order(plan.size());
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](size_t a, size_t b) {
        int da = cargos[plan[a].ci].getDeadline();
        int db = cargos[plan[b].ci].getDeadline();
        if (da != db) 
            return da < db;

        int fa = freights[plan[a].fi].getTime();
        int fb = freights[plan[b].fi].getTime();
        return fa < fb;
        });

    // initialise simulation state
    vector<int> capLeftSim(freights.size(), 0);
    for (size_t i = 0; i < freights.size(); ++i) {
        int cap = freights[i].getMaxCapacity();
        capLeftSim[i] = (cap > 0 ? cap : 1);
    }
    vector<int> remainSim(cargos.size(), 0);
    for (size_t j = 0; j < cargos.size(); ++j) {
        int g = cargos[j].getGroupSize();
        remainSim[j] = (g > 0 ? g : 1);
    }

    ofstream out(outFile.c_str());
    if (!out.is_open())
        return false;

    out << "Schedule #,"
        << "Freight Name,"
        << "Freight Destination,"
        << "Freight Refuel Time,"
        << "Freight Capacity Name,"
        << "Freight: Original Capacity Values,"
        << "Freight: Outstanding Capacity or Unfulfilled Capacity,"
        << "Cargo Name,"
        << "Cargo Destination,"
        << "Cargo Time to Reach Destination,"
        << "Cargo: Original Number of Cargo per Group,"
        << "Cargo: Outstanding number of Cargo without freight\n";

    auto capacityName = [](int cap) -> string {
        if (cap >= 12) return "MegaCarrier";
        if (cap >= 6)  return "CargoCruiser";
        return "MiniMover";
        };

    for (size_t k = 0; k < order.size(); ++k) {
        const MatchingAlgorithm::Row& r = plan[order[k]];
        const Freight& f = freights[r.fi];
        const Cargo& c = cargos[r.ci];

        int capOrig = f.getMaxCapacity();
            capOrig = (capOrig > 0 ? capOrig : 1);
        int capBefore = capLeftSim[r.fi];
        int take = r.cnt;
        int capAfter = capBefore - take;
        if (capAfter < 0)
            capAfter = 0;

        int groupOrig = c.getGroupSize();
            groupOrig = (groupOrig > 0 ? groupOrig : 1);
        int cargoBefore = remainSim[r.ci];
        int cargoAfter = cargoBefore - take;
        if (cargoAfter < 0)
            cargoAfter = 0;

        out << (k + 1) << ","
            << f.getId() << ","
            << f.getDest() << ","
            << minutesToHHMM12(f.getTime()) << ","
            << capacityName(capOrig) << ","
            << capOrig << ","
            << capAfter << ","
            << c.getId() << ","
            << c.getDest() << ","
            << minutesToHHMM12(c.getDeadline()) << ","
            << groupOrig << ","
            << cargoAfter << "\n";

        capLeftSim[r.fi] = capAfter;
        remainSim[r.ci] = cargoAfter;
    }

    return true;
}

// ============================================================================
// Save plan sorted by minimum freight to CSV
// ============================================================================

bool ScheduleExporter::savePlanByMinimumFreightCSV(const vector<Freight>& freights,
                                                    const vector<Cargo>& cargos,
                                                    const string& outFile)
{
    // Build packed plan
    vector<MatchingAlgorithm::Row> plan;
    vector<int> capLeft, remain;
    MatchingAlgorithm::buildPackedPlan(freights, cargos, plan, capLeft, remain);

    // Group plan rows by freight index
    map<int, vector<size_t>> freightGroups;
    for (size_t k = 0; k < plan.size(); ++k) {
        freightGroups[plan[k].fi].push_back(k);
    }

    // Sort freight indices by freight ID (ascending order)
    vector<int> sortedFreightIndices;
    for (const auto& pair : freightGroups) {
        sortedFreightIndices.push_back(pair.first);
    }
    sort(sortedFreightIndices.begin(), sortedFreightIndices.end(), [&](int a, int b) {
        return freights[a].getId() < freights[b].getId();
    });

    // initialise simulation state
    vector<int> capLeftSim(freights.size(), 0);
    for (size_t i = 0; i < freights.size(); ++i) {
        int cap = freights[i].getMaxCapacity();
        capLeftSim[i] = (cap > 0 ? cap : 1);
    }
    vector<int> remainSim(cargos.size(), 0);
    for (size_t j = 0; j < cargos.size(); ++j) {
        int g = cargos[j].getGroupSize();
        remainSim[j] = (g > 0 ? g : 1);
    }

    ofstream out(outFile.c_str());
    if (!out.is_open())
        return false;

    out << "Schedule #,"
        << "Freight Name,"
        << "Freight Destination,"
        << "Freight Refuel Time,"
        << "Freight Capacity Name,"
        << "Freight: Original Capacity Values,"
        << "Freight: Outstanding Capacity or Unfulfilled Capacity,"
        << "Cargo Name,"
        << "Cargo Destination,"
        << "Cargo Time to Reach Destination,"
        << "Cargo: Original Number of Cargo per Group,"
        << "Cargo: Outstanding number of Cargo without freight\n";

    auto capacityName = [](int cap) -> string {
        if (cap >= 12) return "MegaCarrier";
        if (cap >= 6)  return "CargoCruiser";
        return "MiniMover";
        };

    int scheduleNum = 1;
    for (int fi : sortedFreightIndices) {
        const Freight& f = freights[fi];
        
        // Process all cargos assigned to this freight
        for (size_t k : freightGroups[fi]) {
            const MatchingAlgorithm::Row& r = plan[k];
            const Cargo& c = cargos[r.ci];

            int capOrig = f.getMaxCapacity();
                capOrig = (capOrig > 0 ? capOrig : 1);
            int capBefore = capLeftSim[r.fi];
            int take = r.cnt;
            int capAfter = capBefore - take;
            if (capAfter < 0)
                capAfter = 0;

            int groupOrig = c.getGroupSize();
                groupOrig = (groupOrig > 0 ? groupOrig : 1);
            int cargoBefore = remainSim[r.ci];
            int cargoAfter = cargoBefore - take;
            if (cargoAfter < 0)
                cargoAfter = 0;

            out << scheduleNum << ","
                << f.getId() << ","
                << f.getDest() << ","
                << minutesToHHMM12(f.getTime()) << ","
                << capacityName(capOrig) << ","
                << capOrig << ","
                << capAfter << ","
                << c.getId() << ","
                << c.getDest() << ","
                << minutesToHHMM12(c.getDeadline()) << ","
                << groupOrig << ","
                << cargoAfter << "\n";

            capLeftSim[r.fi] = capAfter;
            remainSim[r.ci] = cargoAfter;
            scheduleNum++;
        }
    }

    return true;
}

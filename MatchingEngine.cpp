#include "MatchingEngine.h"
#include "Time.h"

#include <functional>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <utility>
#include <numeric>
#include <map>

using namespace std;

// ============================================================================
// Part 1: Base matching (legacy DFS bipartite match)
// ============================================================================

pair<vector<int>, vector<int>>
MatchingEngine::computeMaximumMatching(const vector<Freight>& freights, const vector<Cargo>& cargos) {

    vector<vector<int>> adj(freights.size());
    for (size_t i = 0; i < freights.size(); ++i) {
        for (size_t j = 0; j < cargos.size(); ++j) {
            if (freights[i].getDest() == cargos[j].getDest() &&
                freights[i].getTime() <= cargos[j].getDeadline()) {
                adj[i].push_back(static_cast<int>(j));
            }
        }
    }

    vector<int> matchToCargo(freights.size(), -1);
    vector<int> matchToFreight(cargos.size(), -1);
    vector<int> visited(cargos.size(), 0);

    function<bool(int)> dfs = [&](int u) -> bool {
        for (int v : adj[u]) {
            if (visited[v])
                continue;
            visited[v] = 1;

            if (matchToFreight[v] == -1 || dfs(matchToFreight[v])) {
                matchToFreight[v] = u;
                matchToCargo[u] = v;
                return true;
            }
        }
        return false;
        };

    for (int u = 0; u < static_cast<int>(freights.size()); ++u) {
        fill(visited.begin(), visited.end(), 0);
        dfs(u);
    }
    return make_pair(matchToCargo, matchToFreight);
}

bool MatchingEngine::saveScheduleToFile(const vector<Freight>& freights,
    const vector<Cargo>& cargos,
    const string& outFile) {
    pair<vector<int>, vector<int>> result = computeMaximumMatching(freights, cargos);
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

bool MatchingEngine::saveScheduleToCSV(const vector<Freight>& freights,
                                       const vector<Cargo>& cargos,
                                       const string& outFile) {
    pair<vector<int>, vector<int>> result = computeMaximumMatching(freights, cargos);
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Option 5a - View Plans - Sorted by cargos deadline

void MatchingEngine::printArrivalPrioritySummary(const vector<Freight>& freights, const vector<Cargo>& cargos) {
    const int EARLY = ARRIVAL_EARLY_MIN;

    // sort cargos by deadline
    vector<size_t> order(cargos.size());
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](size_t a, size_t b) {
        int da = cargos[a].getDeadline();
        int db = cargos[b].getDeadline();
        if (da < 0 && db < 0)
            return a < b;
        if (da < 0)
            return false;
        if (db < 0)
            return true;
        return da < db;
        });

    vector<char> usedFreight(freights.size(), 0);
    vector<pair<int, int> > plan; 

	// assign cargos to freights 
    for (size_t ci : order) {
        const Cargo& c = cargos[ci];
        if (c.getDeadline() < 0)
            continue;

        vector<size_t> eligible;
        for (size_t fi = 0; fi < freights.size(); ++fi) {
            if (usedFreight[fi])
                continue;
            
            const Freight& f = freights[fi];
            if (f.getDest() != c.getDest())
                continue;
            if (!Time::inArrivalWindow(Time(f.getTime()), Time(c.getDeadline()), EARLY))
                continue;
            
            eligible.push_back(fi);
        }
        if (eligible.empty())
            continue;

		// choose best freight (latest departure time)
        size_t best = *max_element(eligible.begin(), eligible.end(),
                                   [&](size_t x, size_t y) {
                return freights[x].getTime() < freights[y].getTime();
            });

        usedFreight[best] = 1;
        plan.push_back(make_pair(static_cast<int>(best), static_cast<int>(ci)));
    }

    cout << "Arrival-Priority Schedule (<= 15 min early or on time):\n";
    for (size_t k = 0; k < plan.size(); ++k) {
        int fi = plan[k].first;
        int ci = plan[k].second;
        const Freight& f = freights[fi];
        const Cargo& c = cargos[ci];

        cout << "  Freight " << f.getId()
            << " -> Cargo " << c.getId()
            << " (dest=" << c.getDest()
            << ", depart=" << minutesToHHMM12(f.getTime())
            << ", deadline=" << minutesToHHMM12(c.getDeadline())
            << ")\n";
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Option 5b - View Plans - Sorted by freights capacity
namespace {
    struct Row {
        int fi;
        int ci;
        int cnt;
    };

    // IMPROVED: Multi-pass algorithm with better cargo assignment
    static void buildPackedPlan(const vector<Freight>& freights,
        const vector<Cargo>& cargos,
        vector<Row>& plan,
        vector<int>& capLeft,
        vector<int>& remain,
        bool useEarliestFirst = false) {
        const int EARLY = 15;

        // Initialize capacity tracking
        capLeft.assign(freights.size(), 0);
        for (size_t i = 0; i < freights.size(); ++i) {
            int cap = freights[i].getMaxCapacity();
            capLeft[i] = (cap > 0 ? cap : 1);
        }

        // Initialize cargo tracking
        remain.assign(cargos.size(), 0);
        for (size_t j = 0; j < cargos.size(); ++j) {
            int g = cargos[j].getGroupSize();
            remain[j] = (g > 0 ? g : 1);
        }

        plan.clear();

        // IMPROVED APPROACH: Process cargos by deadline (earliest first)
        // For each cargo, find the best available freight

        vector<size_t> cargoOrder(cargos.size());
        iota(cargoOrder.begin(), cargoOrder.end(), 0);
        sort(cargoOrder.begin(), cargoOrder.end(), [&](size_t a, size_t b) {
            int da = cargos[a].getDeadline();
            int db = cargos[b].getDeadline();
            if (da < 0 && db < 0) return a < b;
            if (da < 0) return false;
            if (db < 0) return true;
            return da < db;
            });

        // Process each cargo
        for (size_t ci : cargoOrder) {
            if (remain[ci] <= 0) continue;

            const Cargo& c = cargos[ci];

            // Find all compatible freights
            vector<size_t> compatible;
            for (size_t fi = 0; fi < freights.size(); ++fi) {
                if (capLeft[fi] <= 0) continue;

                const Freight& f = freights[fi];

                // Check destination
                if (f.getDest() != c.getDest()) continue;

                // Check time window
                if (!Time::inArrivalWindow(Time(f.getTime()),
                    Time(c.getDeadline()), EARLY)) {
                    continue;
                }

                compatible.push_back(fi);
            }

            if (compatible.empty()) continue;

            // Sort compatible freights by departure time
            if (useEarliestFirst) {
                // Earliest first - prioritize freights departing earlier
                sort(compatible.begin(), compatible.end(), [&](size_t a, size_t b) {
                    return freights[a].getTime() < freights[b].getTime();
                    });
            } else {
                // Latest first - prioritize freights closest to deadline
                sort(compatible.begin(), compatible.end(), [&](size_t a, size_t b) {
                    return freights[a].getTime() > freights[b].getTime();
                    });
            }

            // Assign to freights until cargo is fully allocated
            for (size_t fi : compatible) {
                if (remain[ci] <= 0) break;

                int take = min(capLeft[fi], remain[ci]);
                if (take <= 0) continue;

                plan.push_back(Row{ static_cast<int>(fi),
                                   static_cast<int>(ci), take });
                capLeft[fi] -= take;
                remain[ci] -= take;
            }
        }
    }
}

void MatchingEngine::printCapacityFirstSummary(const vector<Freight>& freights,
                                               const vector<Cargo>& cargos) {
    vector<Row> plan;
    vector<int> capLeft, remain;
    buildPackedPlan(freights, cargos, plan, capLeft, remain);

    // print assignments
    cout << "Capacity-First Schedule (fill each freight before using another):\n";
    for (size_t k = 0; k < plan.size(); ++k) {
        int fi = plan[k].fi;
        int ci = plan[k].ci;
        int cnt = plan[k].cnt;
        const Freight& f = freights[fi];
        const Cargo& c = cargos[ci];
        
        cout << "  Freight " << f.getId()
             << " <-- " << cnt << " unit(s) of Cargo " << c.getId()
             << " (dest=" << c.getDest()
             << ", depart=" << minutesToHHMM12(f.getTime())
             << ", deadline=" << minutesToHHMM12(c.getDeadline())
             << ")\n";
    }

	// print freights usage
    cout << "\nFreights used:\n";
    for (size_t i = 0; i < freights.size(); ++i) {
        int cap = freights[i].getMaxCapacity();
        cap = (cap > 0 ? cap : 1);
        int used = cap - capLeft[i];
        if (used > 0) {
            cout << "  - " << freights[i].getId()
                << " [" << used << "/" << cap << "] "
                << "(" << freights[i].getDest() << " " << minutesToHHMM12(freights[i].getTime()) << ")\n";
        }
    }

	// print unassigned cargos
    cout << "\nUnassigned Cargo (remaining units):\n";
    for (size_t j = 0; j < cargos.size(); ++j) {
        if (remain[j] > 0) {
            cout << "  - " << cargos[j].getId() << " x" << remain[j] << "\n";
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// option 4&5 - Schedle Plan Views

void MatchingEngine::printPlanSortedByCargoTime(const vector<Freight>& freights,
                                                const vector<Cargo>& cargos) {
    vector<Row> plan;
    vector<int> capLeft, remain;
    buildPackedPlan(freights, cargos, plan, capLeft, remain, true); // Use earliest first

    // Group plan rows by cargo index
    map<int, vector<size_t>> cargoGroups;
    for (size_t k = 0; k < plan.size(); ++k) {
        cargoGroups[plan[k].ci].push_back(k);
    }

    // Sort cargo indices by deadline (ascending order)
    vector<int> sortedCargoIndices;
    for (const auto& pair : cargoGroups) {
        sortedCargoIndices.push_back(pair.first);
    }
    sort(sortedCargoIndices.begin(), sortedCargoIndices.end(), [&](int a, int b) {
        int da = cargos[a].getDeadline();
        int db = cargos[b].getDeadline();
        if (da != db) return da < db;
        return cargos[a].getId() < cargos[b].getId();
    });

    cout << "Scheduling plan (sorted by cargo arrival time):\n";
    for (int ci : sortedCargoIndices) {
        const Cargo& c = cargos[ci];
        
        // Calculate total assigned and remaining for this cargo
        int groupSize = c.getGroupSize();
        groupSize = (groupSize > 0 ? groupSize : 1);
        int assigned = groupSize - remain[ci];
        
        cout << "  Cargo " << c.getId()
             << " [" << assigned << "/" << groupSize << "] "
             << "(" << c.getDest() << " deadline=" << minutesToHHMM12(c.getDeadline()) << ")\n";
        
        // List all freights assigned to this cargo
        for (size_t k : cargoGroups[ci]) {
            const Row& r = plan[k];
            const Freight& f = freights[r.fi];
            cout << "      <- " << r.cnt << " unit(s) via Freight " << f.getId()
                 << " (depart=" << minutesToHHMM12(f.getTime()) << ")\n";
        }
    }
}

void MatchingEngine::printPlanSortedByMinimumFreight(const vector<Freight>& freights,
                                                     const vector<Cargo>& cargos) {
    vector<Row> plan;
    vector<int> capLeft, remain;
    buildPackedPlan(freights, cargos, plan, capLeft, remain);

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

    cout << "Scheduling plan (sorted by minimum freight):\n";
    for (int fi : sortedFreightIndices) {
        const Freight& f = freights[fi];
        
        // Calculate capacity usage
        int cap = f.getMaxCapacity();
        cap = (cap > 0 ? cap : 1);
        int used = cap - capLeft[fi];
        
        cout << "  Freight " << f.getId()
             << " [" << used << "/" << cap << "] "
             << "(" << f.getDest() << " " << minutesToHHMM12(f.getTime()) << ")\n";
        
        // List all cargos assigned to this freight
        for (size_t k : freightGroups[fi]) {
            const Row& r = plan[k];
            const Cargo& c = cargos[r.ci];
            cout << "      <- " << r.cnt << " unit(s) of Cargo " << c.getId()
                 << " (deadline=" << minutesToHHMM12(c.getDeadline()) << ")\n";
        }
    }
}


void MatchingEngine::printFreightsNotFull(const vector<Freight>& freights,
                                          const vector<Cargo>& cargos) {
    vector<Row> plan;
    vector<int> capLeft, remain;
    buildPackedPlan(freights, cargos, plan, capLeft, remain);

    cout << "Freights not at full capacity:\n";
    bool any = false;

    for (size_t i = 0; i < freights.size(); ++i) {
        int cap = freights[i].getMaxCapacity();
        cap = (cap > 0 ? cap : 1);
        int used = cap - capLeft[i];

        if (used < cap) {
            any = true;
            cout << "  - " << freights[i].getId()
                << " [" << used << "/" << cap << "] "
                << "(" << freights[i].getDest() << " " << minutesToHHMM12(freights[i].getTime()) << ")\n";
        }
    }
    if (!any)
        cout << "  (none)\n";
}

void MatchingEngine::printUnassignedCargos(const vector<Freight>& freights,
                                           const vector<Cargo>& cargos) {
    vector<Row> plan;
    vector<int> capLeft, remain;
    buildPackedPlan(freights, cargos, plan, capLeft, remain);

    cout << "Cargos not yet fully assigned:\n";
    bool any = false;

    for (size_t j = 0; j < cargos.size(); ++j) {
        if (remain[j] > 0) {
            any = true;
            cout << "  - " << cargos[j].getId()
                 << " | dest=" << cargos[j].getDest()
                 << " | deadline=" << minutesToHHMM12(cargos[j].getDeadline())
                 << " | remaining=" << remain[j] << "\n";
        }
    }
    if (!any)
        cout << "  (none)\n";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Option 7 - Save & Export to CSV

bool MatchingEngine::savePlanByCargoTimeCSV(const vector<Freight>& freights,
                                            const vector<Cargo>& cargos,
                                            const string& outFile)
{
    // Build packed plan
    vector<Row> plan;
    vector<int> capLeft, remain;
    buildPackedPlan(freights, cargos, plan, capLeft, remain, true); // Use earliest first

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
        const Row& r = plan[order[k]];
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

bool MatchingEngine::savePlanByMinimumFreightCSV(const vector<Freight>& freights,
                                                  const vector<Cargo>& cargos,
                                                  const string& outFile)
{
    // Build packed plan
    vector<Row> plan;
    vector<int> capLeft, remain;
    buildPackedPlan(freights, cargos, plan, capLeft, remain);

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
            const Row& r = plan[k];
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

#include "SchedulePrinter.h"
#include "MatchingAlgorithm.h"
#include "Time.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <map>

using namespace std;

// ============================================================================
// Print arrival-priority summary
// ============================================================================

void SchedulePrinter::printArrivalPrioritySummary(const vector<Freight>& freights, const vector<Cargo>& cargos) {
    const int EARLY = MatchingAlgorithm::ARRIVAL_EARLY_MIN;

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

// ============================================================================
// Print capacity-first summary
// ============================================================================

void SchedulePrinter::printCapacityFirstSummary(const vector<Freight>& freights,
                                                const vector<Cargo>& cargos) {
    vector<MatchingAlgorithm::Row> plan;
    vector<int> capLeft, remain;
    MatchingAlgorithm::buildPackedPlan(freights, cargos, plan, capLeft, remain);

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

// ============================================================================
// Print plan sorted by cargo time
// ============================================================================

void SchedulePrinter::printPlanSortedByCargoTime(const vector<Freight>& freights,
                                                 const vector<Cargo>& cargos) {
    vector<MatchingAlgorithm::Row> plan;
    vector<int> capLeft, remain;
    MatchingAlgorithm::buildPackedPlan(freights, cargos, plan, capLeft, remain, true); // Use earliest first

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
            const MatchingAlgorithm::Row& r = plan[k];
            const Freight& f = freights[r.fi];
            cout << "      <- " << r.cnt << " unit(s) via Freight " << f.getId()
                 << " (depart=" << minutesToHHMM12(f.getTime()) << ")\n";
        }
    }
}

// ============================================================================
// Print plan sorted by minimum freight
// ============================================================================

void SchedulePrinter::printPlanSortedByMinimumFreight(const vector<Freight>& freights,
                                                      const vector<Cargo>& cargos) {
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
            const MatchingAlgorithm::Row& r = plan[k];
            const Cargo& c = cargos[r.ci];
            cout << "      <- " << r.cnt << " unit(s) of Cargo " << c.getId()
                 << " (deadline=" << minutesToHHMM12(c.getDeadline()) << ")\n";
        }
    }
}

// ============================================================================
// Print freights not at full capacity
// ============================================================================

void SchedulePrinter::printFreightsNotFull(const vector<Freight>& freights,
                                           const vector<Cargo>& cargos) {
    vector<MatchingAlgorithm::Row> plan;
    vector<int> capLeft, remain;
    MatchingAlgorithm::buildPackedPlan(freights, cargos, plan, capLeft, remain);

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

// ============================================================================
// Print unassigned cargos
// ============================================================================

void SchedulePrinter::printUnassignedCargos(const vector<Freight>& freights,
                                            const vector<Cargo>& cargos) {
    vector<MatchingAlgorithm::Row> plan;
    vector<int> capLeft, remain;
    MatchingAlgorithm::buildPackedPlan(freights, cargos, plan, capLeft, remain);

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

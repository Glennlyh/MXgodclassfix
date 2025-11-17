#include "MatchingAlgorithm.h"
#include "Time.h"
#include <functional>
#include <algorithm>
#include <numeric>

using namespace std;

// ============================================================================
// Core matching algorithm (bipartite matching via DFS)
// ============================================================================

pair<vector<int>, vector<int>>
MatchingAlgorithm::computeMaximumMatching(const vector<Freight>& freights, const vector<Cargo>& cargos) {

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

// ============================================================================
// Build packed plan for capacity-based scheduling
// ============================================================================

void MatchingAlgorithm::buildPackedPlan(const vector<Freight>& freights,
                                        const vector<Cargo>& cargos,
                                        vector<Row>& plan,
                                        vector<int>& capLeft,
                                        vector<int>& remain,
                                        bool useEarliestFirst) {
    const int EARLY = ARRIVAL_EARLY_MIN;

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

    // Process cargos by deadline (earliest first)
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

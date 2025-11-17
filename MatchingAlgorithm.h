#pragma once
#ifndef MATCHINGALGORITHM_H
#define MATCHINGALGORITHM_H

#include <vector>
#include <utility>
#include "Freight.h"
#include "Cargo.h"

/**
 * MatchingAlgorithm - Handles core matching algorithm logic
 * Responsibility: Compute optimal freight-cargo matchings
 */
class MatchingAlgorithm {
public:
    static constexpr int ARRIVAL_EARLY_MIN = 15;

    // Core matching algorithm (bipartite matching via DFS)
    static std::pair<std::vector<int>, std::vector<int>>
        computeMaximumMatching(const std::vector<Freight>& freights,
                               const std::vector<Cargo>& cargos);

    // Row structure for packed plan
    struct Row {
        int fi;  // freight index
        int ci;  // cargo index
        int cnt; // count
    };

    // Build packed plan for capacity-based scheduling
    static void buildPackedPlan(const std::vector<Freight>& freights,
                                const std::vector<Cargo>& cargos,
                                std::vector<Row>& plan,
                                std::vector<int>& capLeft,
                                std::vector<int>& remain,
                                bool useEarliestFirst = false);
};

#endif

#pragma once

#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"


// Uses the basic contraction-deletion recurrence with memoization.
class NaiveSolver {
public:
    NaiveSolver(const Multipole& g, bool use_memo) : g_(g), use_memo_(use_memo) {}

    FlowPoly get_flow_poly() {
        std::vector<edge_t> loops = g_.remove_loops();
        auto result = get_flow_poly_no_loops();
        std::cout << "memo size: " << memo_.size() << std::endl;
        return result * power(3, loops.size());
    }

private:
    FlowPoly get_flow_poly_no_loops() {
        if (use_memo_ && memo_.contains(g_)) {
            return memo_[g_];
        }

        if (!g_.has_inner_edge()) {
            auto result = FlowPoly({{g_.to_partition(), 1}});
            if (use_memo_) memo_[g_] = result;
            return result;
        }

        auto [u, v] = g_.get_largest_inner_edge();
        ContractionBackup backup = g_.contract_edge(u, v);
        int loops = backup.inner_neighbors_u.count(v) - 1;
        auto result_contracted = get_flow_poly_no_loops() * power(3, loops);
        g_.uncontract_edge(u, v, backup);

        g_.remove_edge({u, v});
        auto result_removed = get_flow_poly_no_loops();
        g_.add_edge({u, v});

        auto result = result_contracted - result_removed;
        if (use_memo_) memo_[g_] = result;
        return result;
    }

    Multipole g_;
    std::map<Multipole, FlowPoly> memo_;
    bool use_memo_;
};

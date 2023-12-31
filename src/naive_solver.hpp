#pragma once

#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"


// Uses the basic contraction-deletion recurrence with memoization.
class NaiveSolver {
public:
    NaiveSolver(const Multipole& g, bool use_memo) : g_(g), use_memo_(use_memo) {}

    FlowPoly get_flow_poly() {
        vec<Edge> loops = g_.remove_loops();
        FlowPoly result = get_flow_poly_no_loops();
        result.multiply(power(3, loops.size()));
        // std::cout << "memo size: " << memo_.size() << std::endl;
        return result;
    }

private:
    FlowPoly get_flow_poly_no_loops() {
        if (use_memo_ && memo_.contains(g_)) {
            return memo_[g_];
        }

        if (!g_.has_any_inner_edge()) {
            // (we don't need to memoize this, it will be computed quickly anyway)
            return prune_if_enabled(g_.to_partition(), 1);
        }

        auto [u, v] = g_.get_largest_inner_edge();

        auto [v_outer, v_inner] = g_.get_inner_vertex_edge_count(v);
        if (v_outer == 0 && v_inner == 1) {
            // contraction/deletion here yields empty flow poly, return early
            // (we don't need to memoize this, it will be computed quickly anyway)
            return {};
        }

        ContractionBackup backup = g_.contract_edge(u, v);
        ll loops = backup.u.inner_neighbors.count(v) - 1;
        FlowPoly result = get_flow_poly_no_loops();
        result.multiply(power(3, loops));
        g_.uncontract_edge(u, v, backup);

        g_.remove_edge({u, v});
        result.add(get_flow_poly_no_loops(), -1);
        g_.add_edge({u, v});

        if (use_memo_) memo_[g_] = result;
        return result;
    }

    Multipole g_;
    std::map<Multipole, FlowPoly> memo_;
    bool use_memo_;
};

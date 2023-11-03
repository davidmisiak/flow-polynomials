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
        // std::cout << "memo size: " << memo_.size() << std::endl;
        return result * power(3, loops.size());
    }

private:
    FlowPoly get_flow_poly_no_loops() {
        if (use_memo_ && memo_.contains(g_)) {
            return memo_[g_];
        }

        if (!g_.has_any_inner_edge()) {
            FlowPoly result = FlowPoly({{g_.to_partition(), 1}});
            if (use_memo_) memo_[g_] = result;
            return result;
        }

        auto [u, v] = g_.get_largest_inner_edge();

        // contraction/deletion here yields empty flow poly, return early
        auto [v_outer, v_inner] = g_.get_inner_vertex_edge_count(v);
        if (v_outer == 0 && v_inner == 1) {
            return FlowPoly();
        }

        ContractionBackup backup = g_.contract_edge(u, v);
        ll loops = backup.u.inner_neighbors.count(v) - 1;
        FlowPoly result = get_flow_poly_no_loops() * power(3, loops);
        g_.uncontract_edge(u, v, backup);

        g_.remove_edge({u, v});
        result -= get_flow_poly_no_loops();
        g_.add_edge({u, v});

        if (use_memo_) memo_[g_] = result;
        return result;
    }

    Multipole g_;
    naive_solver_memo_t<Multipole, FlowPoly> memo_;
    bool use_memo_;
};

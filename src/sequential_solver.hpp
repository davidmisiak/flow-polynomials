#pragma once

#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"


// In each step:
// - removes a vertex u which will contribute the least number of outer edge groups when removed
// - adds a new outer edge instead of each removed inner edge
// - solves the emerging subproblem
// - computes the flow polynomial from the subproblem's flow polynomial
class SequentialSolver {
public:
    SequentialSolver(const Multipole& g) : g_(g) {}

    FlowPoly get_flow_poly() {
        vec<Edge> loops = g_.remove_loops();

        next_outer_vertex_ = g_.get_first_unused_outer_vertex();

        FlowPoly result = get_flow_poly_no_loops();
        result.multiply(power(3, loops.size()));
        return result;
    }

private:
    FlowPoly get_flow_poly_no_loops() {
        if (!g_.has_any_inner_edge()) {
            return prune_if_enabled(g_.to_partition(), 1);
        }

        vertex_t u = get_inner_vertex_to_remove();
        VertexBackup backup = g_.remove_vertex(u);

        // contraction/deletion here yields empty flow poly, return early
        if (backup.outer_neighbors.empty() && backup.inner_neighbors.size() == 1) return {};

        vec<vertex_t> tmp_outer_vertices;
        for (vertex_t v : backup.inner_neighbors) {
            vertex_t w = next_outer_vertex_--;
            tmp_outer_vertices.push_back(w);
            g_.add_edge({v, w});
        }

        FlowPoly subresult = get_flow_poly_no_loops();
        vertex_set_t new_s_base = mset_to_set(backup.outer_neighbors);
        FlowPoly result;
        for (const auto& [p, coef] : subresult) {
            result.add(process_subresult_partition(p, new_s_base, tmp_outer_vertices), coef);
        }
        return result;
    }

    // Returns the vertex that will contribute the least number of outer edge groups when removed.
    // Note that this turns out to be very important - if you remove this and select e.g. always
    // the last vertex, the performance decreases massively.
    vertex_t get_inner_vertex_to_remove() {
        check(g_.has_any_inner_edge(), "no inner edges");
        vertex_t best_u = -1;
        ll best_val = INT64_MAX;
        for (vertex_t u : g_.get_inner_vertices()) {
            auto [outer, inner] = g_.get_inner_vertex_distinct_neighbor_count(u);
            ll val = inner - outer;
            if (val <= best_val) {
                best_u = u;
                best_val = val;
            }
        }
        return best_u;
    }

    // For each partition in the subresult, computes the corresponding partitions and their
    // coefficients in the result. Instead of contracting/deleting every inner edge corresponding
    // to a tmp outer edge, only contracting/deleting the whole groups of tmp edges that share
    // a set in the partition is sufficient (the coefficients are then computed using the geometric
    // sum formula).
    FlowPoly process_subresult_partition(const Partition& p, const vertex_set_t& new_s_base, const vec<vertex_t>& tmp_outer_vertices) {
        Partition new_p_base;  // sets with no tmp vertices
        vec<vertex_set_t> cleared_p_rest;  // sets that had tmp vertices, but now cleared of them
        vec<ll> tmp_counts;  // counts of tmp vertices cleared from cleared_p_rest
        for (vertex_set_t s : p) {
            ll tmp_count = 0;
            for (vertex_t v : tmp_outer_vertices) {
                if (s.contains(v)) {
                    s.erase(v);
                    tmp_count++;
                }
            }
            if (tmp_count == 0) {
                new_p_base.insert(s);
            } else if (tmp_count == 1 && s.empty()) {
                // contraction/deletion here yields empty flow poly, return early
                // (though this is never reached if PRUNE_CONTINUOUSLY is true)
                return {};
            } else {
                cleared_p_rest.push_back(s);
                tmp_counts.push_back(tmp_count);
            }
        }

        FlowPoly result;
        ll max_mask = (1 << cleared_p_rest.size());
        for (ll contraction_mask = 0; contraction_mask < max_mask; contraction_mask++) {
            Partition new_p = new_p_base;
            vertex_set_t new_s = new_s_base;
            ll coef = 1;
            ll contracted_count = 0;
            for (size_t i = 0; i < cleared_p_rest.size(); i++) {
                const vertex_set_t& s = cleared_p_rest[i];
                if (contraction_mask & (1 << i)) {
                    new_s.insert(s.begin(), s.end());
                    contracted_count++;
                    coef *= geometric_sum(-3, tmp_counts[i]);
                } else {
                    if (!s.empty()) new_p.insert(s);
                }
            }
            if (!new_s.empty()) new_p.insert(new_s);
            ll removed_parity = tmp_outer_vertices.size() - contracted_count;
            if (removed_parity % 2 == 1) coef *= -1;
            result.add(prune_if_enabled(new_p, coef));
        }
        return result;
    }

    Multipole g_;
    vertex_t next_outer_vertex_;
};

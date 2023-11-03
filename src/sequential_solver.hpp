#pragma once

#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"


// Provides a priority queue for vertices where lower priority is better. Each vertex can be
// present at most once. When popping, the vertex with the lowest priority is returned; if there
// are multiple such vertices, the one with the highest number is returned.
class PriorityQueue {
public:
    bool empty() const {
        return priorities_.empty();
    }

    void insert(vertex_t v, ll priority) {
        check(!priorities_.contains(v));
        priorities_[v] = priority;
        queue_[priority].insert(v);
    }

    void remove(vertex_t v) {
        check(priorities_.contains(v));
        ll priority = priorities_[v];
        priorities_.erase(v);
        queue_[priority].erase(v);
        if (queue_[priority].empty()) queue_.erase(priority);
    }

    void update_priority(vertex_t v, ll new_priority) {
        check(priorities_.contains(v));
        ll old_priority = priorities_[v];
        priorities_[v] = new_priority;
        queue_[old_priority].erase(v);
        queue_[new_priority].insert(v);
        if (queue_[old_priority].empty()) queue_.erase(old_priority);
    }

    void add_priority(vertex_t v, ll delta) {
        check(priorities_.contains(v));
        update_priority(v, priorities_[v] + delta);
    }

    vertex_t pop() {
        check(!empty());
        ll priority = queue_.rbegin()->first;
        vertex_t v = *queue_[priority].rbegin();
        remove(v);
        return v;
    }

private:
    priority_queue_t<vertex_t, ll> priorities_;
    priority_queue_t<ll, vertex_set_t, std::greater<ll>> queue_;
};


// In each step:
// - removes a vertex u with the lowest number of inner neighbors
// - adds a new outer edge instead of each removed inner edge
// - solves the emerging subproblem
// - computes the flow polynomial from the subproblem's flow polynomial
class SequentialSolver {
public:
    SequentialSolver(const Multipole& g) : g_(g) {}

    FlowPoly get_flow_poly() {
        vec<Edge> loops = g_.remove_loops();

        for (vertex_t u : g_.get_inner_vertices()) {
            ll priority = g_.get_distinct_inner_neighbor_count(u);
            queue_.insert(u, priority);
        }
        next_outer_vertex_ = g_.get_first_unused_outer_vertex();

        auto result = get_flow_poly_no_loops();
        return result * power(3, loops.size());
    }

private:
    FlowPoly get_flow_poly_no_loops() {
        if (!g_.has_any_inner_edge()) {
            return FlowPoly({{g_.to_partition(), 1}});
        }

        vertex_t u = queue_.pop();
        VertexBackup backup = g_.remove_vertex(u);

        // contraction/deletion here yields empty flow poly, return early
        if (backup.outer_neighbors.empty() && backup.inner_neighbors.size() == 1) return {};

        vec<vertex_t> tmp_outer_vertices;
        vertex_t prev = -1;
        for (vertex_t v : backup.inner_neighbors) {
            vertex_t w = next_outer_vertex_--;
            tmp_outer_vertices.push_back(w);
            g_.add_edge({v, w});
            if (v != prev) {
                queue_.add_priority(v, -1);
                prev = v;
            }
        }

        FlowPoly subresult = get_flow_poly_no_loops();
        vertex_set_t new_s_base = mset_to_set(backup.outer_neighbors);
        FlowPoly result;
        for (const auto& [p, coef] : subresult) {
            result += process_subresult_partition(p, new_s_base, tmp_outer_vertices) * coef;
        }
        return result;
    }

    // For each partition in the subresult, computes the corresponding partitions and their
    // coefficients in the result. Instead of contracting/deleting every inner edge corresponding
    // to a tmp outer edge, only contracting/deleting the whole groups of tmp edges that share
    // a set in the partition is sufficient (the coefficients are then computed using the geometric
    // sum formula).
    FlowPoly process_subresult_partition(const Partition& p, const vertex_set_t& new_s_base, const vec<vertex_t>& tmp_outer_vertices) {
        FlowPoly result;

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
            } else {
                cleared_p_rest.push_back(s);
                tmp_counts.push_back(tmp_count);
            }
        }

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
            result += {new_p, coef};
        }
        return result;
    }

    Multipole g_;
    PriorityQueue queue_; // TODO also take into account outer neighbors?
    vertex_t next_outer_vertex_;
};

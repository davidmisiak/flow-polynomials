#pragma once

#include <bits/stdc++.h>

#include "base.hpp"


class VertexBackup {
public:
    vertex_mset_t outer_neighbors;
    vertex_mset_t inner_neighbors;

    VertexBackup(const vertex_mset_t& outer_neighbors, const vertex_mset_t& inner_neighbors)
        : outer_neighbors(outer_neighbors), inner_neighbors(inner_neighbors) {}
};


class ContractionBackup {
public:
    VertexBackup u;
    VertexBackup v;

    ContractionBackup(const VertexBackup& u, const VertexBackup& v) : u(u), v(v) {}
};


// Represent a k-pole with n inner vertices. Outer vertices must be assigned numbers between
// -1 and -MAX_VERTEX and inner vertices numbers between 0 to MAX_VERTEX.
// Each edge should connect either two inner vertices or an inner and an outer vertex.
// The multipole is represented by a map from each vertex to its neighbors (both directions).
// If a vertex has no neighbors, it does not appear in the map.
class Multipole {
public:
    Multipole() {}

    Multipole(const std::vector<edge_t>& edges) {
        for (const auto& e : edges) {
            vertex_t u = *e.begin();
            vertex_t v = *e.rbegin();
            check(u >= -MAX_VERTEX);
            check(v >= 0);
            check(v <= MAX_VERTEX);
            add_edge(e);
        }
    }

    friend bool operator<(const Multipole& g1, const Multipole& g2) {
        return tie(g1.outer_edges_, g1.inner_edges_) < tie(g2.outer_edges_, g2.inner_edges_);
    }

    friend std::istream& operator>>(std::istream& is, Multipole& g) {
        int m;
        is >> m;
        std::vector<edge_t> edges;
        for (int i = 0; i < m; i++) {
            int u, v;
            is >> u >> v;
            edges.push_back({static_cast<vertex_t>(u), static_cast<vertex_t>(v)});
        }
        g = Multipole(edges);
        return is;
    }

    // --- edge operations ---

    bool has_edge(const edge_t& e) const {
        vertex_t u = *e.begin();
        vertex_t v = *e.rbegin();
        const adjacency_t& edges = u < 0 ? outer_edges_ : inner_edges_;
        return edges.contains(u) && edges.at(u).contains(v);
    }

    bool has_any_inner_edge() const {
        return !inner_edges_.empty();
    }

    // Returns the pair of vertices with the following property: The second vertex is the largest
    // vertex with an inner neighbor, and the first vertex is the largest of its inner neighbors.
    std::pair<vertex_t, vertex_t> get_largest_inner_edge() const {
        check(has_any_inner_edge());
        const auto& [v, neighbors] = *inner_edges_.rbegin();
        vertex_t u = *neighbors.rbegin();
        return {u, v};
    }

    void add_edge(const edge_t& e) {
        vertex_t u = *e.begin();
        vertex_t v = *e.rbegin();
        adjacency_t& edges = u < 0 ? outer_edges_ : inner_edges_;
        edges[u].insert(v);
        if (u != v) {
            edges[v].insert(u);
        }
    }

    void remove_edge(const edge_t& e) {
        check(has_edge(e));
        vertex_t u = *e.begin();
        vertex_t v = *e.rbegin();
        adjacency_t& edges = u < 0 ? outer_edges_ : inner_edges_;
        edges[u].erase(edges[u].find(v));
        if (edges[u].empty()) edges.erase(u);
        if (u != v) {
            edges[v].erase(edges[v].find(u));
            if (edges[v].empty()) edges.erase(v);
        }
    }

    std::vector<edge_t> remove_loops() {
        std::vector<edge_t> loops;
        for (auto& [u, neighbors] : inner_edges_) {
            size_t c = neighbors.count(u);
            for (size_t i = 0; i < c; i++) {
                loops.push_back({u});
                remove_edge({u});
            }
        }
        return loops;
    }

    // --- vertex operations ---

    vertex_set_t get_inner_vertices() const {
        vertex_set_t result;
        for (const auto& [u, _] : outer_edges_) {
            if (u < 0) continue;
            result.insert(u);
        }
        for (const auto& [u, _] : inner_edges_) {
            result.insert(u);
        }
        return result;
    }

    vertex_t get_fresh_outer_vertex() {
        if (outer_edges_.empty()) return -1;
        return outer_edges_.rbegin()->first - 1;
    }

    VertexBackup get_vertex_backup(vertex_t u) const {
        vertex_mset_t outer_neighbors = outer_edges_.contains(u) ? outer_edges_.at(u) : vertex_mset_t();
        vertex_mset_t inner_neighbors = inner_edges_.contains(u) ? inner_edges_.at(u) : vertex_mset_t();
        return {outer_neighbors, inner_neighbors};
    }

    // Returns the number of distinct inner neighbors of an inner vertex.
    ll get_distinct_inner_neighbor_count(vertex_t u) const {
        check(u >= 0);
        if (!inner_edges_.contains(u)) return 0;
        ll result = 0;
        vertex_t prev = -1;
        for (const auto& v : inner_edges_.at(u)) {
            if (v == prev) continue;
            result++;
            prev = v;
        }
        return result;
    }

    // Removes vertex and all its edges, and returns the removed edges.
    VertexBackup remove_vertex(vertex_t u) {
        VertexBackup backup = get_vertex_backup(u);
        for (const auto& v : backup.outer_neighbors) {
            remove_edge({u, v});
        }
        for (const auto& v : backup.inner_neighbors) {
            remove_edge({u, v});
        }
        return backup;
    }

    // Contracts a u-v inner edge, while merging v into u and dropping all emerging loops.
    ContractionBackup contract_edge(vertex_t u, vertex_t v) {
        check(has_edge({u, v}));
        ContractionBackup backup(get_vertex_backup(u), get_vertex_backup(v));
        for (const auto& w : backup.v.outer_neighbors) {
            remove_edge({v, w});
            add_edge({u, w});
        }
        for (const auto& w : backup.v.inner_neighbors) {
            remove_edge({v, w});
            // avoid adding loops
            if (w != u) add_edge({u, w});
        }
        return backup;
    }

    // Undoes a previous operation of contract_edge.
    void uncontract_edge(vertex_t u, vertex_t v, const ContractionBackup& backup) {
        while (outer_edges_.contains(u)) {
            vertex_t w = *outer_edges_[u].rbegin();
            remove_edge({u, w});
        }
        while (inner_edges_.contains(u)) {
            vertex_t w = *inner_edges_[u].rbegin();
            remove_edge({u, w});
        }
        for (const auto& w : backup.u.outer_neighbors) add_edge({u, w});
        for (const auto& w : backup.u.inner_neighbors) add_edge({u, w});
        for (const auto& w : backup.v.outer_neighbors) add_edge({v, w});
        // avoid adding a u-v edge again
        for (const auto& w : backup.v.inner_neighbors) if (w != u) add_edge({v, w});
    }

    Partition to_partition() const {
        check(!has_any_inner_edge());
        Partition result;
        for (const auto& [u, neighbors] : outer_edges_) {
            if (u < 0) continue;
            result.insert(mset_to_set(neighbors));
        }
        return result;
    }

private:
    adjacency_t outer_edges_;
    adjacency_t inner_edges_;
};

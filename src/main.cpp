#include <bits/stdc++.h>

using namespace std;

using ll = long long;

// type aliases for easy underlying data structure replacement
using vertex_t = int8_t;
using edge_t = set<vertex_t>;
using neighbors_t = multiset<vertex_t>;
using adjacency_t = map<vertex_t, neighbors_t>;
template <typename K> using partition_set_t = set<K>;
template <typename K, typename V> using flow_poly_map_t = map<K, V>;


void check(bool cond) {
    if (!cond) {
        throw runtime_error("Assertion failed");
    }
}

ll power(ll x, ll n) {
    ll result = 1;
    while (n > 0) {
        if (n % 2 == 1) {
            result *= x;
        }
        x *= x;
        n /= 2;
    }
    return result;
}


// Partition of vertex set.
class Partition : public partition_set_t<neighbors_t> {
public:
    friend ostream& operator<<(ostream& os, const Partition& p) {
        for (const auto& s : p) {
            os << "[ ";
            for (const auto& v : s) {
                os << static_cast<int>(v) << " ";
            }
            os << "]";
        }
        return os;
    }

    bool is_proper() const {
        for (const auto& s : *this) {
            if (s.size() == 1) return false;
        }
        return true;
    }
};


// Flow polynomial expressed as a linear combination of its multipole minors without inner edges
// (each represented by the corresponding partition of its vertex set).
class FlowPoly : public flow_poly_map_t<Partition, ll> {
public:
    FlowPoly operator-(const FlowPoly& other) const {
        FlowPoly result;
        for (const auto& [p, count] : *this) {
            result[p] = count;
        }
        for (const auto& [p, count] : other) {
            result[p] -= count;
            if (result[p] == 0) result.erase(p);
        }
        return result;
    }

    FlowPoly operator*(int k) const {
        if (k == 0) return FlowPoly();
        FlowPoly result;
        for (const auto& [p, count] : *this) {
            result[p] = count * k;
        }
        return result;
    }

    friend ostream& operator<<(ostream& os, const FlowPoly& fp) {
        os << "[" << endl;
        for (const auto& [p, count] : fp) {
            os << format("{:4d} * ", count) << p << endl;
        }
        os << "]";
        return os;
    }

    // Remove multipoles that can not be compatible with any boundary value.
    FlowPoly only_proper() const {
        FlowPoly result;
        for (const auto& [p, count] : *this) {
            if (p.is_proper()) result[p] = count;
        }
        return result;
    }
};


class ContractionBackup {
public:
    neighbors_t outer_neighbors_u;
    neighbors_t inner_neighbors_u;
    neighbors_t outer_neighbors_v;
    neighbors_t inner_neighbors_v;

    ContractionBackup(const neighbors_t& outer_neighbors_u, const neighbors_t& inner_neighbors_u,
                      const neighbors_t& outer_neighbors_v, const neighbors_t& inner_neighbors_v)
        : outer_neighbors_u(outer_neighbors_u), inner_neighbors_u(inner_neighbors_u),
          outer_neighbors_v(outer_neighbors_v), inner_neighbors_v(inner_neighbors_v) {}
};


// Represent a k-pole with n inner vertices. Outer vertices must be assigned numbers between
// -1 and -128 and inner vertices numbers between 0 to 127.
// Each edge should connect either two inner vertices or an inner and an outer vertex.
// The multipole is represented by a map from each vertex to its neighbors (both directions).
// If a vertex has no neighbors, it does not appear in the map.
class Multipole {
public:
    Multipole() {}

    Multipole(const vector<edge_t>& edges) {
        for (const auto& e : edges) {
            check(*e.rbegin() >= 0);
            add_edge(e);
        }
    }

    friend bool operator<(const Multipole& g1, const Multipole& g2) {
        return tie(g1.outer_edges_, g1.inner_edges_) < tie(g2.outer_edges_, g2.inner_edges_);
    }

    friend istream& operator>>(istream& is, Multipole& g) {
        int m;
        is >> m;
        vector<edge_t> edges;
        for (int i = 0; i < m; i++) {
            int u, v;
            is >> u >> v;
            check(-128 <= u && u <= 127 && -128 <= v && v <= 127);
            edges.push_back({static_cast<vertex_t>(u), static_cast<vertex_t>(v)});
        }
        g = Multipole(edges);
        return is;
    }

    bool has_edge(const edge_t& e) const {
        vertex_t u = *e.begin();
        vertex_t v = *e.rbegin();
        const adjacency_t& edges = u < 0 ? outer_edges_ : inner_edges_;
        return edges.contains(u) && edges.at(u).contains(v);
    }

    bool has_inner_edge() const {
        return !inner_edges_.empty();
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

    // Returns the pair of edges with the following property: The second vertex is the largest
    // vertex with an inner neighbor, and the first vertex is the largest of its inner neighbors.
    pair<vertex_t, vertex_t> get_largest_inner_edge() const {
        check(has_inner_edge());
        const auto& [v, neighbors] = *inner_edges_.rbegin();
        vertex_t u = *neighbors.rbegin();
        return {u, v};
    }

    vector<edge_t> remove_loops() {
        vector<edge_t> loops;
        for (auto& [u, neighbors] : inner_edges_) {
            size_t c = neighbors.count(u);
            for (size_t i = 0; i < c; i++) {
                loops.push_back({u});
                remove_edge({u});
            }
        }
        return loops;
    }

    // Contracts a u-v edge, while merging v into u and dropping all emerging loops.
    ContractionBackup contract_edge(vertex_t u, vertex_t v) {
        check(has_edge({u, v}));
        neighbors_t outer_neighbors_u = outer_edges_.contains(u) ? outer_edges_[u] : neighbors_t();
        neighbors_t inner_neighbors_u = inner_edges_.contains(u) ? inner_edges_[u] : neighbors_t();
        neighbors_t outer_neighbors_v = outer_edges_.contains(v) ? outer_edges_[v] : neighbors_t();
        neighbors_t inner_neighbors_v = inner_edges_.contains(v) ? inner_edges_[v] : neighbors_t();
        ContractionBackup backup(
            outer_neighbors_u, inner_neighbors_u,
            outer_neighbors_v, inner_neighbors_v
        );
        for (const auto& w : backup.outer_neighbors_v) {
            remove_edge({v, w});
            add_edge({u, w});
        }
        for (const auto& w : backup.inner_neighbors_v) {
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
        for (const auto& w : backup.outer_neighbors_u) add_edge({u, w});
        for (const auto& w : backup.inner_neighbors_u) add_edge({u, w});
        for (const auto& w : backup.outer_neighbors_v) add_edge({v, w});
        // avoid adding a u-v edge again
        for (const auto& w : backup.inner_neighbors_v) if (w != u) add_edge({v, w});
    }

    Partition to_partition() const {
        check(!has_inner_edge());
        Partition result;
        for (const auto& [u, neighbors] : outer_edges_) {
            if (u < 0) continue;
            result.insert(neighbors);
        }
        return result;
    }

private:
    adjacency_t outer_edges_;
    adjacency_t inner_edges_;
};


// Uses the basic contraction-deletion recurrence with memoization.
class NaiveSolver {
public:
    NaiveSolver(const Multipole& g, bool use_memo) : g_(g), use_memo_(use_memo) {}

    FlowPoly get_flow_poly() {
        vector<edge_t> loops = g_.remove_loops();
        auto result = get_flow_poly_no_loops();
        cout << "memo size: " << memo_.size() << endl;
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
    map<Multipole, FlowPoly> memo_;
    bool use_memo_;
};


int main() {
    Multipole g;
    cin >> g;
    FlowPoly fp = NaiveSolver(g, true).get_flow_poly();
    cout << fp << endl;
    cout << fp.only_proper() << endl;
}

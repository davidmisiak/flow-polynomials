#pragma once

#include <bits/stdc++.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "icecream.hpp"
#pragma GCC diagnostic pop

// shortcuts:
using ll = int64_t;
template <typename T> using vec = std::vector<T>;
template <typename T, typename U> using pair = std::pair<T, U>;

// type aliases for easy experimenting with underlying data structures
// TODO try other container types
using vertex_t = int16_t;
using vertex_set_t = std::set<vertex_t>;
using vertex_mset_t = std::multiset<vertex_t>;
using adjacency_t = std::map<vertex_t, vertex_mset_t>;
template <typename K> using partition_t = std::set<K>;
template <typename K, typename V> using flow_poly_t = std::map<K, V>;
template <typename K, typename V> using naive_solver_memo_t = std::map<K, V>;

// constants:
constexpr ll MAX_VERTEX = 60;
constexpr bool PRUNE_CONTINUOUSLY = true;


void check(bool cond) {
    if (!cond) {
        throw std::runtime_error("Assertion failed");
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

// Returns x^0 + x^1 + ... + x^(n-1).
ll geometric_sum(ll x, ll n) {
    if (x == 1) return n;
    return (power(x, n) - 1) / (x - 1);
}

vertex_set_t mset_to_set(const vertex_mset_t& s) {
    vertex_set_t result;
    for (vertex_t v : s) result.insert(v);
    return result;
}


// The vertices can be given in any order, but are always stored and returned in sorted order.
class Edge {
public:
    Edge(vertex_t u, vertex_t v) {
        vertices_ = std::minmax(u, v);
    }

    auto operator<=>(const Edge& other) const = default;

    vertex_t u() const { return vertices_.first; }
    vertex_t v() const { return vertices_.second; }
    pair<vertex_t, vertex_t> get() const { return vertices_; }

private:
    pair<vertex_t, vertex_t> vertices_;
};


// Partition of vertex set.
class Partition : public partition_t<vertex_set_t> {
public:
    friend std::ostream& operator<<(std::ostream& os, const Partition& p) {
        for (const auto& s : p) {
            os << "[ ";
            for (const auto& v : s) {
                os << static_cast<ll>(v) << " ";
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
class FlowPoly : public flow_poly_t<Partition, ll> {
public:
    FlowPoly& operator+=(const FlowPoly& other) {
        for (const auto& [p, count] : other) {
            (*this)[p] += count;
            if ((*this)[p] == 0) erase(p);
        }
        return *this;
    }

    FlowPoly& operator-=(const FlowPoly& other) {
        for (const auto& [p, count] : other) {
            (*this)[p] -= count;
            if ((*this)[p] == 0) erase(p);
        }
        return *this;
    }

    FlowPoly& operator*=(ll c) {
        if (c == 0) {
            clear();
        } else {
            for (auto& [p, count] : *this) {
                count *= c;
            }
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const FlowPoly& fp) {
        os << "[" << std::endl;
        for (const auto& [p, count] : fp) {
            os << std::format("{:4d} * ", count) << p << std::endl;
        }
        os << "]";
        return os;
    }

    // Remove multipoles that can not be compatible with any boundary value.
    FlowPoly prune() const {
        FlowPoly result;
        for (const auto& [p, count] : *this) {
            if (p.is_proper()) result[p] = count;
        }
        return result;
    }
};

FlowPoly prune_if_enabled(const Partition& p, ll count) {
    if (PRUNE_CONTINUOUSLY && !p.is_proper()) return {};
    return {{{p, count}}};
}

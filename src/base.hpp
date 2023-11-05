#pragma once

#include <bits/stdc++.h>

#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "icecream.hpp"
#pragma GCC diagnostic pop

// shortcuts:
using ll = int64_t;
template <typename T> using vec = std::vector<T>;
template <typename T, typename U> using pair = std::pair<T, U>;

// type aliases (for easy experimenting with the underlying data structures):
using vertex_t = int16_t;
using vertex_set_t = boost::container::flat_set<vertex_t>;
using vertex_mset_t = boost::container::flat_multiset<vertex_t>;
using adjacency_t = std::map<vertex_t, vertex_mset_t>;
using partition_t = boost::container::flat_set<vertex_set_t>;

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
class Partition : public partition_t {
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
class FlowPoly : public std::map<Partition, ll> {
public:
    // performs "this += c*other"
    void add(FlowPoly&& other, ll c = 1) {
        if (c == 0) return;
        other.multiply(c);
        // insert items not contained in `this`
        merge(other);
        // update items contained in both
        // (only those remained in `other`; though this isn't true for boost::container::flat_map!)
        for (const auto& [p, count] : other) {
            (*this)[p] += count;
            if ((*this)[p] == 0) erase(p);
        }
    }

    void multiply(ll c) {
        if (c == 1) {
            return;
        }
        if (c == 0) {
            clear();
            return;
        }
        for (auto& [p, count] : *this) {
            count *= c;
        }
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

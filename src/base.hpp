#pragma once

#include <bits/stdc++.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "icecream.hpp"
#pragma GCC diagnostic pop


using ll = int64_t;
template <typename T> using vec = std::vector<T>;

// type aliases for easy experimenting with underlying data structures
// TODO try other container types
using vertex_t = int16_t;
using edge_t = std::set<vertex_t>;
using vertex_set_t = std::set<vertex_t>;
using vertex_mset_t = std::multiset<vertex_t>;
using adjacency_t = std::map<vertex_t, vertex_mset_t>;
template <typename K> using partition_t = std::set<K>;
template <typename K, typename V> using flow_poly_t = std::map<K, V>;
template <typename K, typename V> using naive_solver_memo_t = std::map<K, V>;
template <typename K, typename V, typename C = std::less<K>> using priority_queue_t = std::map<K, V, C>;

constexpr ll MAX_VERTEX = 60;


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
// TODO prune improper partitions continuously? (somewhere)
class FlowPoly : public flow_poly_t<Partition, ll> {
public:
    FlowPoly& operator+=(const std::pair<Partition, ll>& p) {
        (*this)[p.first] += p.second;
        if ((*this)[p.first] == 0) erase(p.first);
        return *this;
    }

    FlowPoly& operator-=(const std::pair<Partition, ll>& p) {
        (*this)[p.first] -= p.second;
        if ((*this)[p.first] == 0) erase(p.first);
        return *this;
    }

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

    FlowPoly operator+(const FlowPoly& other) const {
        FlowPoly result = *this;
        result += other;
        return result;
    }

    FlowPoly operator-(const FlowPoly& other) const {
        FlowPoly result = *this;
        result -= other;
        return result;
    }

    FlowPoly operator*(ll c) const {
        FlowPoly result = *this;
        result *= c;
        return result;
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
    FlowPoly only_proper() const {
        FlowPoly result;
        for (const auto& [p, count] : *this) {
            if (p.is_proper()) result[p] = count;
        }
        return result;
    }
};

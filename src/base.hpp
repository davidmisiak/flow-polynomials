#pragma once

#include <bits/stdc++.h>


using ll = long long;

// type aliases for easy underlying data structure replacement
using vertex_t = int16_t;
using edge_t = std::set<vertex_t>;
using neighbors_t = std::multiset<vertex_t>;
using adjacency_t = std::map<vertex_t, neighbors_t>;
template <typename K> using partition_set_t = std::set<K>;
template <typename K, typename V> using flow_poly_map_t = std::map<K, V>;

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


ll geometric_sum(ll x, ll n) {
    if (x == 1) return n + 1;
    return (power(x, n + 1) - 1) / (x - 1);
}


// Partition of vertex set.
class Partition : public partition_set_t<neighbors_t> {
public:
    friend std::ostream& operator<<(std::ostream& os, const Partition& p) {
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

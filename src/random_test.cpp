#include <bits/stdc++.h>

#include "base.hpp"


int main() {
    for (ll instance = 0; instance < 3000; instance++) {
        ll k = rand() % 6;
        ll n = (rand() % 9) + 1;
        ll m = rand() % (n*n/2+1);
        std::cout << k+m << '\n';
        for (vertex_t i = 0; i < k; i++) {
            vertex_t u = -i-1;
            vertex_t v = std::min(rand() % k, n-1);
            std::cout << u << " " << v << '\n';
        }
        for (ll i = 0; i < m; i++) {
            vertex_t u = rand() % n;
            vertex_t v = rand() % n;
            std::cout << u << " " << v << '\n';
        }
        std::cout << std::flush;
    }
}

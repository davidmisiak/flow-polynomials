#include <bits/stdc++.h>

#include "base.hpp"


int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);

    check(argc == 2, "invalid number of arguments");
    ll instances = std::stoll(argv[1]);

    srand(0);

    for (ll instance = 0; instance < instances; instance++) {
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

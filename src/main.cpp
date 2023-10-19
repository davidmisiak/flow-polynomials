#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"
#include "naive_solver.hpp"


int main() {
    Multipole g;
    std::cin >> g;
    FlowPoly fp = NaiveSolver(g, true).get_flow_poly();
    std::cout << fp << std::endl;
    std::cout << fp.only_proper() << std::endl;
}

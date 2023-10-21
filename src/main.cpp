#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"
#include "naive_solver.hpp"
#include "sequential_solver.hpp"


int main() {
    Multipole g;
    std::cin >> g;

    FlowPoly fp_naive = NaiveSolver(g, true).get_flow_poly();
    // std::cout << fp_naive << std::endl;
    // std::cout << fp_naive.only_proper() << std::endl;

    FlowPoly fp_seq = SequentialSolver(g).get_flow_poly();
    std::cout << fp_seq << std::endl;
    std::cout << fp_seq.only_proper() << std::endl;

    check(fp_naive == fp_seq);
}

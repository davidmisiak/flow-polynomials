#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"
#include "naive_solver.hpp"
#include "sequential_solver.hpp"

enum InputType {
    NUMERIC,
    PLANTRI,
};

enum SolverType {
    NAIVE,
    SEQUENTIAL,
    ALL,
};

int main(int argc, char *argv[] ) {
    std::ios::sync_with_stdio(false);

    check(argc == 3);

    InputType input_type = NUMERIC;
    if (strcmp(argv[1], "numeric") == 0) {
        input_type = NUMERIC;
    } else if (strcmp(argv[1], "plantri") == 0) {
        input_type = PLANTRI;
    } else {
        check(false);
    }

    SolverType solver_type = NAIVE;
    if (strcmp(argv[2], "naive") == 0) {
        solver_type = NAIVE;
    } else if (strcmp(argv[2], "sequential") == 0) {
        solver_type = SEQUENTIAL;
    } else if (strcmp(argv[2], "all") == 0) {
        solver_type = ALL;
    } else {
        check(false);
    }

    ll count = 0;
    while (std::cin.peek() != EOF) {
        count++;

        Multipole g;
        if (input_type == NUMERIC) {
            g = Multipole::read_numeric();
        } else if (input_type == PLANTRI) {
            g = Multipole::read_plantri_disk_triangulation();
        } else {
            check(false);
        }

        if (input_type == NUMERIC) {
            std::cin >> std::ws;
        }

        FlowPoly fp1;
        FlowPoly fp2;
        if (solver_type == NAIVE) {
            fp1 = NaiveSolver(g, true).get_flow_poly();
        } else if (solver_type == SEQUENTIAL) {
            fp1 = SequentialSolver(g).get_flow_poly();
        } else if (solver_type == ALL) {
            fp1 = NaiveSolver(g, true).get_flow_poly();
            fp2 = SequentialSolver(g).get_flow_poly();
            check(fp1 == fp2);
        } else {
            check(false);
        }
        std::cout << fp1 << std::endl;
        std::cout << fp1.only_proper() << std::endl;
    }
    std::cerr << "Processed " << count << " graphs" << std::endl;
}

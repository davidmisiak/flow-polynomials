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
    } else if (strcmp(argv[2], "seq") == 0) {
        solver_type = SEQUENTIAL;
    } else if (strcmp(argv[2], "all") == 0) {
        solver_type = ALL;
    } else {
        check(false);
    }

    ll count = 0;
    ll max_coef_count = 0;
    ll max_coef_value = 0;
    ll max_star_coef_value = 0;
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
            if (PRUNE_CONTINUOUSLY) {
                check(fp1 == fp1.prune());
                check(fp2 == fp2.prune());
            }
            check(fp1 == fp2);
        } else {
            check(false);
        }
        // std::cout << fp1 << std::endl;
        std::cout << fp1.prune() << std::endl;

        max_coef_count = std::max(max_coef_count, static_cast<ll>(fp1.size()));
        for (const auto& [p, coef] : fp1) {
            max_coef_value = std::max(max_coef_value, coef);
        }
        Partition star({g.get_outer_vertices()});
        if (fp1.contains(star)) {
            max_star_coef_value = std::max(max_star_coef_value, fp1.at(star));
        }
    }
    std::cerr << "Processed " << count << " graphs" << std::endl;
    std::cerr << "max coef count = " << max_coef_count
        << ", max coef val = " << max_coef_value
        << ", max star coef val = " << max_star_coef_value << std::endl;
}

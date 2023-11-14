#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"
#include "naive_solver.hpp"
#include "sequential_solver.hpp"
#include "output.hpp"

enum InputType {
    NUMERIC,
    PLANTRI,
};

enum SolverType {
    NAIVE,
    SEQUENTIAL,
    ALL,
};

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input_type> <solver_type> <output_type>" << std::endl;
        check(false, "invalid number of arguments");
    }

    InputType input_type = NUMERIC;
    if (strcmp(argv[1], "num") == 0) {
        input_type = NUMERIC;
    } else if (strcmp(argv[1], "plantri") == 0) {
        input_type = PLANTRI;
    } else {
        check(false, "invalid input type");
    }

    SolverType solver_type = NAIVE;
    if (strcmp(argv[2], "naive") == 0) {
        solver_type = NAIVE;
    } else if (strcmp(argv[2], "seq") == 0) {
        solver_type = SEQUENTIAL;
    } else if (strcmp(argv[2], "all") == 0) {
        solver_type = ALL;
    } else {
        check(false, "invalid solver type");
    }

    OutputType output_type = FLOW_POLY;
    if (strcmp(argv[3], "none") == 0) {
        output_type = NONE;
    } else if (strcmp(argv[3], "graph") == 0) {
        output_type = GRAPH;
    } else if (strcmp(argv[3], "fp") == 0) {
        output_type = FLOW_POLY;
    } else if (strcmp(argv[3], "stats") == 0) {
        output_type = STATS;
    } else {
        check(false, "invalid output type");
    }

    Output output(output_type);
    while (std::cin.peek() != EOF) {
        Multipole g;
        if (input_type == NUMERIC) {
            g = Multipole::read_numeric(std::cin);
        } else if (input_type == PLANTRI) {
            g = Multipole::read_plantri_disk_triangulation(std::cin);
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
            // sanity checks:
            if (PRUNE_CONTINUOUSLY) {
                check(fp1 == fp1.prune(), "fp1 != fp1.prune()");
                check(fp2 == fp2.prune(), "fp2 != fp2.prune()");
            }
            check(fp1 == fp2, "fp1 != fp2");
            for (const auto& [_, coef] : fp1) check(coef != 0, "fp contains zero");
        } else {
            check(false);
        }
        output.process(g, fp1);
    }
    output.summary();
}

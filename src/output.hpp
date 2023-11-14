#pragma once

#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"

enum OutputType {
    NONE,
    GRAPH,
    FLOW_POLY,
    STATS,
};

class Output {
public:
    Output(OutputType output_type) : output_type_(output_type) {}

    void process(const Multipole& g, const FlowPoly& fp) {
        ll outer_vertex_count = static_cast<ll>(g.get_outer_vertices().size());
        ll inner_vertex_count = static_cast<ll>(g.get_inner_vertices().size());
        ll coef_count = static_cast<ll>(fp.size());
        ll min_coef_value = INT64_MAX;
        ll max_coef_value = INT64_MIN;
        for (const auto& [p, coef] : fp) {
            min_coef_value = std::min(min_coef_value, coef);
            max_coef_value = std::max(max_coef_value, coef);
        }
        ll star_coef_value = 0;
        Partition star({g.get_outer_vertices()});
        if (fp.contains(star)) {
            star_coef_value = fp.at(star);
        }

        bool print_cond = true; // you can use a custom condition

        // max star:
        // ll v = outer_vertex_count + inner_vertex_count;
        // ll k = outer_vertex_count;
        // print_cond =
        //     (v == 16 && k == 3 && star_coef_value == 21 ) ||
        //     (v == 16 && k == 4 && star_coef_value == 11 ) ||
        //     (v == 16 && k == 5 && star_coef_value == 5  ) ||
        //     (v == 18 && k == 3 && star_coef_value == 44 ) ||
        //     (v == 18 && k == 4 && star_coef_value == 21 ) ||
        //     (v == 18 && k == 5 && star_coef_value == 10 ) ||
        //     (v == 20 && k == 3 && star_coef_value == 85 ) ||
        //     (v == 20 && k == 4 && star_coef_value == 43 ) ||
        //     (v == 20 && k == 5 && star_coef_value == 21 ) ||
        //     (v == 22 && k == 3 && star_coef_value == 172) ||
        //     (v == 22 && k == 4 && star_coef_value == 85 ) ||
        //     (v == 22 && k == 5 && star_coef_value == 42 ) ||
        //     (v == 24 && k == 3 && star_coef_value == 341) ||
        //     (v == 24 && k == 4 && star_coef_value == 171) ||
        //     (v == 24 && k == 5 && star_coef_value == 85 ) ||
        //     (v == 26 && k == 3 && star_coef_value == 684) ||
        //     (v == 26 && k == 4 && star_coef_value == 341) ||
        //     (v == 26 && k == 5 && star_coef_value == 170);

        if (print_cond) {
            if (output_type_ == NONE) {
                // do nothing
            } else if (output_type_ == GRAPH) {
                g.write_numeric(std::cout);
            } else if (output_type_ == FLOW_POLY) {
                std::cout << fp << std::endl;
            } else if (output_type_ == STATS) {
                std::cout << outer_vertex_count
                    << " " << inner_vertex_count
                    << " " << coef_count
                    << " " << min_coef_value
                    << " " << max_coef_value
                    << " " << star_coef_value
                    << std::endl;
            } else {
                check(false);
            }
        }

        count_++;
        min_coef_count_ = std::min(min_coef_count_, coef_count);
        max_coef_count_ = std::max(max_coef_count_, coef_count);
        min_coef_value_ = std::min(min_coef_value_, min_coef_value);
        max_coef_value_ = std::max(max_coef_value_, max_coef_value);
        min_star_coef_value_ = std::min(min_star_coef_value_, star_coef_value);
        max_star_coef_value_ = std::max(max_star_coef_value_, star_coef_value);
    }

    void summary() {
        std::cerr << "Processed " << count_ << " graphs" << std::endl;
        std::cerr << "coef counts in [" << min_coef_count_ << ", " << max_coef_count_ << "]"
            << ", coef values in [" << min_coef_value_ << ", " << max_coef_value_ << "]"
            << ", star coef values in [" << min_star_coef_value_ << ", " << max_star_coef_value_ << "]"
            << std::endl;
    }

private:
    OutputType output_type_;
    ll count_ = 0;
    ll min_coef_count_ = INT64_MAX;
    ll max_coef_count_ = INT64_MIN;
    ll min_coef_value_ = INT64_MAX;
    ll max_coef_value_ = INT64_MIN;
    ll min_star_coef_value_ = INT64_MAX;
    ll max_star_coef_value_ = INT64_MIN;
};

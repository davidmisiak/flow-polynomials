#pragma once

#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"

enum OutputType {
    NONE,
    GRAPH,
    FLOW_POLY,
    STATS,
    K4,
};

class Output {
public:
    Output(OutputType output_type) : output_type_(output_type) {}

    void process(const Multipole& g, const FlowPoly& fp) {
        ll k = static_cast<ll>(g.get_outer_vertices().size());
        ll n = static_cast<ll>(g.get_inner_vertices().size());
        [[maybe_unused]] ll v = k + n;
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

        // max star (run only with PLANTRI_FLAGS=dP):
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

        // k4 max colorings (run only with PLANTRI_FLAGS=dP4):
        // check(k == 4);
        // ll colorings = 0;
        // for (const auto& [_, coef] : fp) colorings += coef;
        // colorings = 9*colorings + 12*star_coef_value; // = 21*star + 9*second + 9*third
        // print_cond =
        //     (v == 16 && k == 4 && colorings == 198 ) ||
        //     (v == 18 && k == 4 && colorings == 390 ) ||
        //     (v == 20 && k == 4 && colorings == 774 ) ||
        //     (v == 22 && k == 4 && colorings == 1542) ||
        //     (v == 24 && k == 4 && colorings == 3078) ||
        //     (v == 26 && k == 4 && colorings == 6150);

        if (print_cond) {
            if (output_type_ == NONE) {
                // do nothing
            } else if (output_type_ == GRAPH) {
                g.write_numeric(std::cout);
            } else if (output_type_ == FLOW_POLY) {
                std::cout << fp << std::endl;
            } else if (output_type_ == STATS) {
                std::cout << k
                    << " " << n
                    << " " << coef_count
                    << " " << min_coef_value
                    << " " << max_coef_value
                    << " " << star_coef_value
                    << std::endl;
            } else if (output_type_ == K4) {
                if (k == 4) {
                    ll second_coef_value = 0;
                    Partition second_coef({{-1, -2}, {-3, -4}});
                    if (fp.contains(second_coef)) second_coef_value = fp.at(second_coef);

                    ll third_coef_value = 0;
                    Partition third_coef({{-1, -4}, {-2, -3}});
                    if (fp.contains(third_coef)) third_coef_value = fp.at(third_coef);

                    check(!fp.contains({{{-1, -3}, {-2, -4}}}), "nonplanar coef");

                    std::cout << n
                        << " " << star_coef_value
                        << " " << second_coef_value
                        << " " << third_coef_value
                        << std::endl;
                }
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

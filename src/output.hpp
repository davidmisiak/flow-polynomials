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
    K5,
    K6,
    K7,
    K4_SYM_SUMS,
    K5_SYM_SUMS,
    K6_SYM_SUMS,
    K7_SYM_SUMS,
};

enum AggType {
    EACH,
    UNIQUE,
    COUNTS,
};

class Output {
public:
    Output(OutputType output_type, AggType agg_type) : output_type_(output_type), agg_type_(agg_type) {}

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

        // k5 far away on the line 5a+b+c+d+e+f=0 (run with PLANTRI_FLAGS=dP5):
        // check(k == 5);
        // vec<ll> fp_values = {};
        // for (const auto& [_, coef] : fp) fp_values.push_back(coef);
        // std::sort(fp_values.begin(), fp_values.end());
        // print_cond =
        //     (v == 16 && k == 5 && fp_values == vec<ll>{-4, 4, 4, 4, 4, 4}) ||
        //     (v == 18 && k == 5 && fp_values == vec<ll>{-5, 5, 5, 5, 5, 5}) ||
        //     (v == 20 && k == 5 && fp_values == vec<ll>{-12, 12, 12, 12, 12, 12}) ||
        //     (v == 22 && k == 5 && fp_values == vec<ll>{-21, 21, 21, 21, 21, 21}) ||
        //     (v == 24 && k == 5 && fp_values == vec<ll>{-44, 44, 44, 44, 44, 44}) ||
        //     (v == 26 && k == 5 && fp_values == vec<ll>{-85, 85, 85, 85, 85, 85});

        if (print_cond) {
            if (output_type_ == NONE) {
                // do nothing
            } else if (output_type_ == GRAPH) {
                g.write_numeric(std::cout);
            } else if (output_type_ == FLOW_POLY) {
                std::cout << fp << std::endl;
            } else if (output_type_ == STATS) {
                std::stringstream line;
                line << k
                    << " " << n
                    << " " << coef_count
                    << " " << min_coef_value
                    << " " << max_coef_value
                    << " " << star_coef_value;
                aggregate_line(line.str());
            } else if (output_type_ == K4) {
                if (k == 4) {
                    aggregate_coefs(n, fp, get_k4_partitions());
                }
            } else if (output_type_ == K5) {
                if (k == 5) {
                    aggregate_coefs(n, fp, get_k5_partitions());
                }
            } else if (output_type_ == K6) {
                if (k == 6) {
                    aggregate_coefs(n, fp, get_k6_partitions());
                }
            } else if (output_type_ == K7) {
                if (k == 7) {
                    aggregate_coefs(n, fp, get_k7_partitions());
                }
            } else if (output_type_ == K4_SYM_SUMS) {
                if (k == 4) {
                    aggregate_coef_sym_sums(n, fp, get_k4_partitions(), compute_k4_sym_sum);
                }
            } else if (output_type_ == K5_SYM_SUMS) {
                if (k == 5) {
                    aggregate_coef_sym_sums(n, fp, get_k5_partitions(), compute_k5_sym_sum);
                }
            } else if (output_type_ == K6_SYM_SUMS) {
                if (k == 6) {
                    aggregate_coef_sym_sums(n, fp, get_k6_partitions(), compute_k6_sym_sum);
                }
            } else if (output_type_ == K7_SYM_SUMS) {
                if (k == 7) {
                    aggregate_coef_sym_sums(n, fp, get_k7_partitions(), compute_k7_sym_sum);
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

    void finish() {
        for (const auto& [line, count] : lines_agg_) {
            if (agg_type_ == EACH) {
                // do nothing, already printed
            } else if (agg_type_ == UNIQUE) {
                std::cout << line << std::endl;
            } else if (agg_type_ == COUNTS) {
                std::cout << line << " " << count << std::endl;
            } else {
                check(false);
            }
        }

        std::cerr << "Processed " << count_ << " graphs" << std::endl;
        std::cerr << "coef counts in [" << min_coef_count_ << ", " << max_coef_count_ << "]"
            << ", coef values in [" << min_coef_value_ << ", " << max_coef_value_ << "]"
            << ", star coef values in [" << min_star_coef_value_ << ", " << max_star_coef_value_ << "]"
            << std::endl;
    }

private:
    void aggregate_line(const std::string& line) {
        if (agg_type_ == EACH) {
            std::cout << line << std::endl;
        } else if (agg_type_ == UNIQUE || agg_type_ == COUNTS) {
            lines_agg_[line]++;
        } else {
            check(false);
        }
    }

    static vec<Partition> get_k4_partitions() {
        return {
            {{{-1, -2, -3, -4}}},
            {{{-1, -2}, {-3, -4}}},
            {{{-2, -3}, {-4, -1}}},
        };
    }

    static vec<ll> compute_k4_sym_sum(const vec<ll>& coefs) {
        check(coefs.size() == 3);
        return {
            coefs[0],
            coefs[1] + coefs[2],
        };
    }

    static vec<Partition> get_k5_partitions() {
        return {
            {{{-1, -2, -3, -4, -5}}},
            {{{-1, -2}, {-3, -4, -5}}},
            {{{-2, -3}, {-4, -5, -1}}},
            {{{-3, -4}, {-5, -1, -2}}},
            {{{-4, -5}, {-1, -2, -3}}},
            {{{-5, -1}, {-2, -3, -4}}},
        };
    }

    static vec<ll> compute_k5_sym_sum(const vec<ll>& coefs) {
        check(coefs.size() == 6);
        return {
            coefs[0],
            coefs[1] + coefs[2] + coefs[3] + coefs[4] + coefs[5],
        };
    }

    static vec<Partition> get_k6_partitions() {
        return {
            {{{-1, -2, -3, -4, -5, -6}}},     // a
            {{{-1, -2}, {-3, -4, -5, -6}}},   // b1
            {{{-2, -3}, {-4, -5, -6, -1}}},   // b2
            {{{-3, -4}, {-5, -6, -1, -2}}},   // b3
            {{{-4, -5}, {-6, -1, -2, -3}}},   // b4
            {{{-5, -6}, {-1, -2, -3, -4}}},   // b5
            {{{-6, -1}, {-2, -3, -4, -5}}},   // b6
            {{{-1, -2, -3}, {-4, -5, -6}}},   // c1
            {{{-2, -3, -4}, {-5, -6, -1}}},   // c2
            {{{-3, -4, -5}, {-6, -1, -2}}},   // c3
            {{{-1, -2}, {-3, -6}, {-4, -5}}}, // d1
            {{{-2, -3}, {-4, -1}, {-5, -6}}}, // d2
            {{{-3, -4}, {-5, -2}, {-6, -1}}}, // d3
            {{{-1, -2}, {-3, -4}, {-5, -6}}}, // e1
            {{{-2, -3}, {-4, -5}, {-6, -1}}}, // e2
        };
    }

    static vec<ll> compute_k6_sym_sum(const vec<ll>& coefs) {
        check(coefs.size() == 15);
        return {
            coefs[0],
            coefs[1] + coefs[2] + coefs[3] + coefs[4] + coefs[5] + coefs[6],
            coefs[7] + coefs[8] + coefs[9],
            coefs[10] + coefs[11] + coefs[12],
            coefs[13] + coefs[14],
        };
    }

    static vec<Partition> get_k7_partitions() {
        return {
            {{{-1, -2, -3, -4, -5, -6, -7}}},     // a
            {{{-1, -2}, {-3, -4, -5, -6, -7}}},   // b1
            {{{-2, -3}, {-4, -5, -6, -7, -1}}},   // b2
            {{{-3, -4}, {-5, -6, -7, -1, -2}}},   // b3
            {{{-4, -5}, {-6, -7, -1, -2, -3}}},   // b4
            {{{-5, -6}, {-7, -1, -2, -3, -4}}},   // b5
            {{{-6, -7}, {-1, -2, -3, -4, -5}}},   // b6
            {{{-7, -1}, {-2, -3, -4, -5, -6}}},   // b7
            {{{-1, -2, -3}, {-4, -5, -6, -7}}},   // c1
            {{{-2, -3, -4}, {-5, -6, -7, -1}}},   // c2
            {{{-3, -4, -5}, {-6, -7, -1, -2}}},   // c3
            {{{-4, -5, -6}, {-7, -1, -2, -3}}},   // c4
            {{{-5, -6, -7}, {-1, -2, -3, -4}}},   // c5
            {{{-6, -7, -1}, {-2, -3, -4, -5}}},   // c6
            {{{-7, -1, -2}, {-3, -4, -5, -6}}},   // c7
            {{{-1, -2}, {-3, -4}, {-5, -6, -7}}}, // d1
            {{{-2, -3}, {-4, -5}, {-6, -7, -1}}}, // d2
            {{{-3, -4}, {-5, -6}, {-7, -1, -2}}}, // d3
            {{{-4, -5}, {-6, -7}, {-1, -2, -3}}}, // d4
            {{{-5, -6}, {-7, -1}, {-2, -3, -4}}}, // d5
            {{{-6, -7}, {-1, -2}, {-3, -4, -5}}}, // d6
            {{{-7, -1}, {-2, -3}, {-4, -5, -6}}}, // d7
            {{{-1, -2}, {-3, -7}, {-4, -5, -6}}}, // e1
            {{{-2, -3}, {-4, -1}, {-5, -6, -7}}}, // e2
            {{{-3, -4}, {-5, -2}, {-6, -7, -1}}}, // e3
            {{{-4, -5}, {-6, -3}, {-7, -1, -2}}}, // e4
            {{{-5, -6}, {-7, -4}, {-1, -2, -3}}}, // e5
            {{{-6, -7}, {-1, -5}, {-2, -3, -4}}}, // e6
            {{{-7, -1}, {-2, -6}, {-3, -4, -5}}}, // e7
            {{{-1, -2}, {-4, -5}, {-3, -6, -7}}}, // f1
            {{{-2, -3}, {-5, -6}, {-4, -7, -1}}}, // f2
            {{{-3, -4}, {-6, -7}, {-5, -1, -2}}}, // f3
            {{{-4, -5}, {-7, -1}, {-6, -2, -3}}}, // f4
            {{{-5, -6}, {-1, -2}, {-7, -3, -4}}}, // f5
            {{{-6, -7}, {-2, -3}, {-1, -4, -5}}}, // f6
            {{{-7, -1}, {-3, -4}, {-2, -5, -6}}}, // f7
        };
    }

    static vec<ll> compute_k7_sym_sum(const vec<ll>& coefs) {
        check(coefs.size() == 36);
        return {
            coefs[0],
            coefs[1] + coefs[2] + coefs[3] + coefs[4] + coefs[5] + coefs[6] + coefs[7],
            coefs[8] + coefs[9] + coefs[10] + coefs[11] + coefs[12] + coefs[13] + coefs[14],
            coefs[15] + coefs[16] + coefs[17] + coefs[18] + coefs[19] + coefs[20] + coefs[21],
            coefs[22] + coefs[23] + coefs[24] + coefs[25] + coefs[26] + coefs[27] + coefs[28],
            coefs[29] + coefs[30] + coefs[31] + coefs[32] + coefs[33] + coefs[34] + coefs[35],
        };
    }

    // return coefficient values for the given k
    vec<ll> get_coefs(const FlowPoly& fp, const vec<Partition>& ps) {
        for (const auto& [p, _] : fp) {
            check(std::find(ps.begin(), ps.end(), p) != ps.end(), "nonplanar coef");
        }
        vec<ll> coefs;
        for (const auto& p : ps) {
            ll coef_value = fp.contains(p) ? fp.at(p) : 0;
            coefs.push_back(coef_value);
        }
        return coefs;
    }

    void aggregate_coefs(ll n, const FlowPoly& fp, const vec<Partition>& ps) {
        vec<ll> coefs = get_coefs(fp, ps);
        std::stringstream line;
        line << n;
        for (ll coef : coefs) {
            line << " " << coef;
        }
        aggregate_line(line.str());
    }

    void aggregate_coef_sym_sums(ll n, const FlowPoly& fp, const vec<Partition>& ps, vec<ll> (*compute_sym_sum)(const vec<ll>&)) {
        vec<ll> coefs = get_coefs(fp, ps);
        vec<ll> sym_sums = compute_sym_sum(coefs);
        std::stringstream line;
        line << n;
        for (ll sym_sum : sym_sums) {
            line << " " << sym_sum;
        }
        aggregate_line(line.str());
    }

    OutputType output_type_;
    AggType agg_type_;
    ll count_ = 0;
    ll min_coef_count_ = INT64_MAX;
    ll max_coef_count_ = INT64_MIN;
    ll min_coef_value_ = INT64_MAX;
    ll max_coef_value_ = INT64_MIN;
    ll min_star_coef_value_ = INT64_MAX;
    ll max_star_coef_value_ = INT64_MIN;
    std::map<std::string, ll> lines_agg_;
};

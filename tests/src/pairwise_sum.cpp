#include <gtest/gtest.h>

#include "quickstats/pairwise_sum.hpp"

#include "utils.h"

class PairwiseSumTest : public ::testing::TestWithParam<int> {};

TEST_P(PairwiseSumTest, Basic) {
    auto n = GetParam();
    std::mt19937_64 rng(n * 59);

    auto sim = simulate_vector<double>(n, -10.0, 10.0, rng);
    quickstats::PairwiseSumWorkspace<double> wrk;
    almost_equal_floats(
        std::accumulate(sim.begin(), sim.end(), 0.0),
        quickstats::pairwise_sum(n, sim.data(), wrk)
    );

    // Trying with a smaller limit.
    almost_equal_floats(
        std::accumulate(sim.begin(), sim.end(), 0.0),
        quickstats::pairwise_sum<5>(n, sim.data(), wrk)
    );
}

TEST_P(PairwiseSumTest, Modified) {
    const std::size_t n = GetParam();
    std::mt19937_64 rng(n * 59);

    auto sim = simulate_vector<double>(n, -10.0, 10.0, rng);

    auto mod = sim;
    for (std::size_t i = 0; i < n; ++i) {
        mod[i] = mod[i] * 2 + i;
    }

    quickstats::PairwiseSumWorkspace<double> wrk;
    auto modsum = quickstats::pairwise_sum(
        n,
        sim.data(),
        [&](std::size_t i, double val) -> double {
            return val * 2 + i;
        },
        wrk
    );

    almost_equal_floats(std::accumulate(mod.begin(), mod.end(), 0.0), modsum);
}

INSTANTIATE_TEST_SUITE_P(
    PairwiseSum,
    PairwiseSumTest,
    ::testing::Values(0, 1, 50, 100, 200, 500, 1000, 2000, 5000, 10000)
);

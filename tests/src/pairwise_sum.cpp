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
        quickstats::pairwise_sum<128, 1>(n, sim.data(), wrk)
    );

    // Trying with a smaller limit.
    almost_equal_floats(
        std::accumulate(sim.begin(), sim.end(), 0.0),
        quickstats::pairwise_sum<5, 1>(n, sim.data(), wrk)
    );

    // Trying with more accumulators.
    almost_equal_floats(
        std::accumulate(sim.begin(), sim.end(), 0.0),
        quickstats::pairwise_sum<128, 4>(n, sim.data(), wrk)
    );

    almost_equal_floats(
        std::accumulate(sim.begin(), sim.end(), 0.0),
        quickstats::pairwise_sum<5, 2>(n, sim.data(), wrk)
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
    auto modsum1 = quickstats::pairwise_sum<128, 1>(
        n,
        sim.data(),
        [&](std::size_t i, double val) -> double {
            return val * 2 + i;
        },
        wrk
    );
    almost_equal_floats(std::accumulate(mod.begin(), mod.end(), 0.0), modsum1);

    auto modsum4 = quickstats::pairwise_sum<128, 4>(
        n,
        sim.data(),
        [&](std::size_t i, double val) -> double {
            return val * 2 + i;
        },
        wrk
    );
    almost_equal_floats(std::accumulate(mod.begin(), mod.end(), 0.0), modsum4);
}

INSTANTIATE_TEST_SUITE_P(
    PairwiseSum,
    PairwiseSumTest,
    ::testing::Values(0, 1, 50, 100, 200, 500, 1000, 2000, 5000, 10000)
);

TEST(RecursiveSum, Basic) {
    // Non-powers of 2.
    {
        std::array<double, 10> sums{1,2,3,4,5,6,7,8,9,10};
        EXPECT_EQ(quickstats::recursive_sum(sums), std::accumulate(sums.begin(), sums.end(), 0.0));
    }

    {
        std::array<double, 3> sums{11,12,13};
        EXPECT_EQ(quickstats::recursive_sum(sums), std::accumulate(sums.begin(), sums.end(), 0.0));
    }

    // Powers of 2.
    {
        std::array<double, 2> sums{11,12};
        EXPECT_EQ(quickstats::recursive_sum(sums), std::accumulate(sums.begin(), sums.end(), 0.0));
    }

    {
        std::array<double, 4> sums{5,6,7,8};
        EXPECT_EQ(quickstats::recursive_sum(sums), std::accumulate(sums.begin(), sums.end(), 0.0));
    }

    {
        std::array<double, 8> sums{5,6,7,8,9,10,11,12};
        EXPECT_EQ(quickstats::recursive_sum(sums), std::accumulate(sums.begin(), sums.end(), 0.0));
    }
}

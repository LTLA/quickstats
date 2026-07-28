#include <gtest/gtest.h>

#include "quickstats/pairwise_sum.hpp"

#include "utils.h"

class PairwiseSumTest : public ::testing::TestWithParam<std::tuple<std::size_t, std::size_t> > {};

TEST_P(PairwiseSumTest, Basic) {
    const auto params = GetParam();
    const auto n = std::get<0>(params);
    const auto maxsum = std::get<1>(params);

    std::mt19937_64 rng(n * 59 + maxsum);
    auto sim = simulate_vector<double>(n, -10.0, 10.0, rng);

    quickstats::PairwiseSumWorkspace<double> wrk;
    quickstats::PairwiseSumOptions opt;
    opt.max_sum_length = maxsum;

    almost_equal_floats(
        std::accumulate(sim.begin(), sim.end(), 0.0),
        quickstats::pairwise_sum<1>(n, sim.data(), wrk, opt)
    );

    // Trying with more accumulators.
    almost_equal_floats(
        std::accumulate(sim.begin(), sim.end(), 0.0),
        quickstats::pairwise_sum<2>(n, sim.data(), wrk, opt)
    );

    almost_equal_floats(
        std::accumulate(sim.begin(), sim.end(), 0.0),
        quickstats::pairwise_sum<4>(n, sim.data(), wrk, opt)
    );
}

TEST_P(PairwiseSumTest, Modified) {
    const auto params = GetParam();
    const auto n = std::get<0>(params);
    const auto maxsum = std::get<1>(params);

    std::mt19937_64 rng(n * 59 + maxsum);
    auto sim = simulate_vector<double>(n, -10.0, 10.0, rng);
    auto mod = sim;
    for (std::size_t i = 0; i < n; ++i) {
        mod[i] = mod[i] * 2 + i;
    }

    quickstats::PairwiseSumWorkspace<double> wrk;
    quickstats::PairwiseSumOptions opt;
    opt.max_sum_length = maxsum;

    auto modsum1 = quickstats::pairwise_sum_abstract<1>(
        n,
        [&](const std::size_t i) -> double {
            return sim[i] * 2 + i;
        },
        wrk,
        opt
    );
    almost_equal_floats(std::accumulate(mod.begin(), mod.end(), 0.0), modsum1);

    auto modsum4 = quickstats::pairwise_sum_abstract<4>(
        n,
        [&](const std::size_t i) -> double {
            return sim[i] * 2 + i;
        },
        wrk,
        opt
    );
    almost_equal_floats(std::accumulate(mod.begin(), mod.end(), 0.0), modsum4);
}

INSTANTIATE_TEST_SUITE_P(
    PairwiseSum,
    PairwiseSumTest,
    ::testing::Combine(
        ::testing::Values(0, 1, 50, 100, 200, 500, 1000, 2000, 5000, 10000), // vector length
        ::testing::Values(10, 25, 100, 200) // max sum length
    )
);

TEST(PairwiseSum, Error) {
    quickstats::PairwiseSumWorkspace<double> wrk;
    quickstats::PairwiseSumOptions opt;
    opt.max_sum_length = 7;

    std::vector<double> foo(100);
    std::string msg;
    try {
        quickstats::pairwise_sum<4>(foo.size(), foo.data(), wrk, opt);
    } catch (std::exception& e) {
        msg = e.what();
    }

    EXPECT_TRUE(msg.find("must be greater") != std::string::npos);
}

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

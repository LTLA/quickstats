#include <gtest/gtest.h>

#include <vector>
#include <cstddef>
#include <random>
#include <algorithm>

#include "quickstats/MultipleQuantiles.hpp"

#include "utils.h"

class MultipleQuantilesRealisticTest : public ::testing::TestWithParam<std::tuple<int, std::pair<double, double>, std::vector<double> > > {};

TEST_P(MultipleQuantilesRealisticTest, Dense) {
    auto params = GetParam();
    const auto num = std::get<0>(params);
    const auto range = std::get<1>(params);
    const auto& probs = std::get<2>(params);

    const std::size_t nprobs = probs.size();
    std::mt19937_64 rng(range.first + range.second * 2 + num * 4 + nprobs * 8);
    auto sim = simulate_vector<double>(num, range.first, range.second, rng);

    quickstats::MultipleQuantilesFixedNumber<double, int> mult(num, probs);
    std::vector<double> res(nprobs);
    auto copy = sim;
    mult(copy.data(), [&](std::size_t i, double val) -> void {
        res[i] = val;
    });

    for (std::size_t p = 0; p < nprobs; ++p) {
        quickstats::SingleQuantileFixedNumber<double, int> quant(num, probs[p]);
        std::copy(sim.begin(), sim.end(), copy.begin());
        EXPECT_EQ(quant(copy.data()), res[p]);
    }
}

TEST_P(MultipleQuantilesRealisticTest, Sparse) {
    auto params = GetParam();
    const auto num = std::get<0>(params);
    const auto range = std::get<1>(params);
    const auto& probs = std::get<2>(params);

    const std::size_t nprobs = probs.size();
    std::mt19937_64 rng(range.first + range.second * 2 + num * 4 + nprobs * 8);
    auto sim = simulate_vector<double>(num, range.first, range.second, rng);

    quickstats::MultipleQuantilesFixedNumber<double, int> mult(num, probs);

    for (int z = 0; z <= num; ++z) {
        std::vector<double> res(nprobs);
        auto copy = sim;
        mult(z, copy.data(), [&](std::size_t i, double val) -> void {
            res[i] = val;
        });

        for (std::size_t p = 0; p < nprobs; ++p) {
            quickstats::SingleQuantileFixedNumber<double, int> quant(num, probs[p]);
            std::copy_n(sim.begin(), z, copy.begin());
            EXPECT_EQ(quant(z, copy.data()), res[p]);
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    MultipleQuantiles,
    MultipleQuantilesRealisticTest,
    ::testing::Combine(
        ::testing::Values(5, 6, 20, 21, 100, 101), // give a chance for '(n - 1) * prob' to be an exact integer.
        ::testing::Values(
            std::pair<double, double>(-10, 10),
            std::pair<double, double>(-10, -1),
            std::pair<double, double>(1, 10)
        ),
        ::testing::Values(
            std::vector<double>{ 0, 0.5, 1.0 },
            std::vector<double>{ 0.1, 0.2, 0.3, 0.5, 0.8 },
            std::vector<double>{ 0.05, 0.07, 0.1, 0.1, 0.15, 0.15, 0.18 }, // high density at the lower end, with ties.
            std::vector<double>{ 0.9, 0.95, 0.95, 0.97, 0.99, 0.99, 0.99, 0.995 } // high density at the other end, with ties.
        )
    )
);

/****************************************/

TEST(MultipleQuantiles, EdgeCases) {
    std::string msg;
    try {
        quickstats::MultipleQuantilesFixedNumber<double, int>(0, std::vector<double>());
    } catch (std::exception& e) {
        msg = e.what();
    }
    EXPECT_TRUE(msg.find("positive") != std::string::npos);

    msg.clear();
    try {
        quickstats::MultipleQuantilesFixedNumber<double, int>(10, std::vector<double>{ 1., 0. });
    } catch (std::exception& e) {
        msg = e.what();
    }
    EXPECT_TRUE(msg.find("should be sorted") != std::string::npos);

    // Check that the methods function correctly with no quantiles.
    {
        quickstats::MultipleQuantilesFixedNumber<double, int> foo(10, std::vector<double>());
        std::vector<double> input { 1, -1, 2, -2, 3, -3, 4, -4, 5, -5 };

        std::vector<double> res;
        foo(input.data(), [&](int, double val) -> void {
            res.push_back(val);
        });
        EXPECT_TRUE(res.empty());

        foo(5, input.data(), [&](int, double val) -> void {
            res.push_back(val);
        });
        EXPECT_TRUE(res.empty());
    }
}

TEST(MultipleQuantilesVariable, Dense) {
    std::vector<double> original { 5, 4, 3, 2, 1 };
    std::vector<double> probs { 0., 0.5, 1. };
    quickstats::MultipleQuantilesVariableNumber<double, int, std::vector<double>*> calc(original.size(), &probs);

    {
        auto values = original;
        calc(0, values.data(), [&](int, double val) -> void {
            EXPECT_TRUE(std::isnan(val));
        });
    }

    {
        auto values = original;
        calc(1, values.data(), [&](int, double val) -> void {
            EXPECT_EQ(val, 5);
        });
    }

    {
        auto values = original;
        std::vector<double> results(probs.size());
        calc(2, values.data(), [&](int q, double val) -> void {
            results[q] = val;
        });
        EXPECT_EQ(results[0], 4);
        EXPECT_EQ(results[1], 4.5);
        EXPECT_EQ(results[2], 5);
    }

    {
        auto values = original;
        std::vector<double> results(probs.size());
        calc(values.size(), values.data(), [&](int q, double val) -> void {
            results[q] = val;
        });
        EXPECT_EQ(results[0], 1);
        EXPECT_EQ(results[1], 3);
        EXPECT_EQ(results[2], 5);
    }

    // Re-using the cached calculator.
    {
        auto values = original;
        std::vector<double> results(probs.size());
        calc(2, values.data() + 2, [&](int q, double val) -> void {
            results[q] = val;
        });
        EXPECT_EQ(results[0], 2);
        EXPECT_EQ(results[1], 2.5);
        EXPECT_EQ(results[2], 3);
    }
}

TEST(MultipleQuantilesVariable, Sparse) {
    std::vector<double> original { 10, 8, 6, 4, 2, 0, -2, -4, -6, -8, -10 };
    std::vector<double> probs { 0., 0.5, 1. };
    quickstats::MultipleQuantilesVariableNumber<double, int, std::vector<double>*> calc(11, &probs);

    {
        auto values = original;
        calc(0, 0, values.data(), [&](int, double quan) -> void {
            EXPECT_TRUE(std::isnan(quan));
        });
    }

    {
        auto values = original;
        calc(1, 0, values.data(), [&](int, double quan) -> void {
            EXPECT_EQ(quan, 0);
        });
        calc(1, 1, values.data(), [&](int, double quan) -> void {
            EXPECT_EQ(quan, 10);
        });
    }

    {
        auto values = original;
        calc(2, 0, values.data(), [&](int, double quan) -> void {
            EXPECT_EQ(quan, 0);
        });

        std::vector<double> res(probs.size());
        calc(2, 1, values.data(), [&](int i, double quan) -> void {
            res[i] = quan;
        });
        EXPECT_EQ(res[0], 0);
        EXPECT_EQ(res[1], 5);
        EXPECT_EQ(res[2], 10);

        calc(2, 2, values.data(), [&](int i, double quan) -> void {
            res[i] = quan;
        });
        EXPECT_EQ(res[0], 8);
        EXPECT_EQ(res[1], 9);
        EXPECT_EQ(res[2], 10);
    }

    {
        auto values = original;
        calc(values.size(), 0, values.data(), [&](int, double quan) -> void {
            EXPECT_EQ(quan, 0);
        });

        std::vector<double> res(probs.size());
        calc(values.size(), 5, values.data(), [&](int i, double quan) -> void {
            res[i] = quan;
        });
        EXPECT_EQ(res[0], 0);
        EXPECT_EQ(res[1], 0);
        EXPECT_EQ(res[2], 10);

        calc(values.size(), values.size(), values.data(), [&](int i, double quan) -> void {
            res[i] = quan;
        });
        EXPECT_EQ(res[0], -10);
        EXPECT_EQ(res[1], 0);
        EXPECT_EQ(res[2], 10);
    }
}

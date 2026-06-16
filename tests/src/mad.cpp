#include <gtest/gtest.h>

#include <vector>
#include <cstddef>
#include <random>
#include <algorithm>

#include "quickstats/mad.hpp"

#include "utils.h"

TEST(Mad, DenseBasic) {
    std::vector<double> vec { 2, 1, 4, 6, 5, 3 };
    const auto med = quickstats::median(vec.size(), vec.data());
    auto mad = quickstats::mad(vec.size(), vec.data(), med);
    EXPECT_FLOAT_EQ(quickstats::scale_mad_to_sd(mad), 2.2239); // i.e., mad(1:6) in R.
}

/****************************************/

static double sparse_mad(std::vector<double> non_zeros, std::size_t num_total) {
    const auto med = quickstats::median(num_total, non_zeros.size(), non_zeros.data());
    return quickstats::mad(num_total, non_zeros.size(), non_zeros.data(), med);
}

static double dense_mad(std::vector<double> non_zeros, std::size_t num_total) {
    non_zeros.resize(num_total);
    const auto med = quickstats::median(num_total, non_zeros.data());
    return quickstats::mad(num_total, non_zeros.data(), med);
}

TEST(Mad, SparseAllPositive) {
    std::vector<double> vec { 2, 1, 4, 5, 3 };
    for (int total = vec.size(); total < 11; ++total) {
        EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
    }

    // Make sure we get identical results when the midpoints are tied floating-point values.
    std::vector<double> frac_vec { 1.0/3, 2.0/9, 3.0/7, 1.0/3, 2.0/9, 1.0/3 };
    for (int total = frac_vec.size(); total < 15; ++total) {
        EXPECT_EQ(sparse_mad(frac_vec, total), dense_mad(frac_vec, total));
    }
}

TEST(Mad, SparseAllNegative) {
    std::vector<double> vec { -2, -1, -4, -5, -3 };
    for (int total = vec.size(); total < 11; ++total) {
        EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
    }

    std::vector<double> frac_vec { -0.2, -0.3, -0.4, -0.2, -0.4, -0.1 };
    for (int total = frac_vec.size(); total < 15; ++total) {
        EXPECT_EQ(sparse_mad(frac_vec, total), dense_mad(frac_vec, total));
    }
}

TEST(Mad, SparseMixed) {
    // Mostly positive.
    {
        std::vector<double> vec { 2.5, -1, 4, -5, 3 };
        for (int total = vec.size(); total < 11; ++total) {
            EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
        }
    }

    // Mostly negative.
    {
        std::vector<double> vec { -2.5, 1, -4, 5, -3 };
        for (int total = vec.size(); total < 11; ++total) {
            EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
        }
    }

    // Equal numbers of positive and negative.
    {
        std::vector<double> vec { -2.5, 1, -4, 5, -3, 6 };
        for (int total = vec.size(); total < 13; ++total) {
            EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
        }
    }
}

/****************************************/

class RealisticMadTest : public ::testing::TestWithParam<std::pair<int, int> > {};

TEST_P(RealisticMadTest, Sparse) {
    auto param = GetParam();
    std::mt19937_64 rng(123 + param.first * 2 + param.second);
    for (int n = 10; n < 100; n += 5) { // alternate between even and odd.
        auto contents = simulate_vector<double>(n, param.first, param.second, rng);

        std::vector<double> copy;
        copy.reserve(n);
        for (int i = 0; i < n; ++i) {
            copy.resize(i);
            std::copy_n(contents.data(), i, copy.data());
            EXPECT_EQ(sparse_mad(copy, n), dense_mad(copy, n));
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    Mad,
    RealisticMadTest,
    ::testing::Values(
        std::pair<double, double>(-10, 10),
        std::pair<double, double>(1, 10),
        std::pair<double, double>(-10, -1)
    )
);

/****************************************/

TEST(Mad, Infinite) {
    EXPECT_TRUE(std::isnan(quickstats::mad<double, double>(0, NULL, std::numeric_limits<double>::infinity())));
    EXPECT_TRUE(std::isnan(quickstats::mad<double, double>(0, 0, NULL, std::numeric_limits<double>::infinity())));
}

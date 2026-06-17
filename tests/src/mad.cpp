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

static std::size_t get_max_test_length(std::size_t num_non_zeros) {
    return num_non_zeros * 2 + 1; // at or beyond this, the median and MAD will always be zero.
}

TEST(Mad, SparseAllPositive) {
    {
        std::vector<double> vec { 2, 1, 4, 5, 3 };
        const auto limit = get_max_test_length(vec.size());
        for (std::size_t total = vec.size(); total <= limit; ++total) {
            EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
        }
    }

    // Make sure we get identical results when the midpoints are tied floating-point values.
    {
        std::vector<double> vec { 1.0/3, 2.0/9, 3.0/7, 1.0/3, 2.0/9, 1.0/3 };
        const auto limit = get_max_test_length(vec.size());
        for (std::size_t total = vec.size(); total <= limit; ++total) {
            EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
        }
    }
}

TEST(Mad, SparseAllNegative) {
    {
        std::vector<double> vec { -2, -1, -4, -5, -3 };
        const auto limit = get_max_test_length(vec.size());
        for (std::size_t total = vec.size(); total <= limit; ++total) {
            EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
        }
    }

    // Trying with some ties and fractions, for some variety.
    {
        std::vector<double> vec { -0.2, -0.3, -0.4, -0.2, -0.4, -0.1 };
        const auto limit = get_max_test_length(vec.size());
        for (std::size_t total = vec.size(); total <= limit; ++total) {
            EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
        }
    }
}

TEST(Mad, SparseMixed) {
    // Mostly positive.
    {
        std::vector<double> vec { 2.5, -1, 4, -5, 3 };
        const auto limit = get_max_test_length(vec.size());
        for (std::size_t total = vec.size(); total <= limit; ++total) {
            EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
        }
    }

    // Mostly negative.
    {
        std::vector<double> vec { -2.5, 1, -4, 5, -3 };
        const auto limit = get_max_test_length(vec.size());
        for (std::size_t total = vec.size(); total <= limit; ++total) {
            EXPECT_EQ(sparse_mad(vec, total), dense_mad(vec, total));
        }
    }

    // Equal numbers of positive and negative.
    {
        std::vector<double> vec { -2.5, 1, -4, 5, -3, 6 };
        const auto limit = get_max_test_length(vec.size());
        for (std::size_t total = vec.size(); total <= limit; ++total) {
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

TEST(Mad, RegularInfinite) {
    std::vector<double> dummy{ 100 };
    EXPECT_TRUE(std::isnan(quickstats::mad<double, double>(1, dummy.data(), std::numeric_limits<double>::infinity())));
    EXPECT_TRUE(std::isnan(quickstats::mad<double, double>(1, 1, dummy.data(), std::numeric_limits<double>::infinity())));

    // Also NaN if the input is empty.
    EXPECT_TRUE(std::isnan(quickstats::mad<double, double>(0, NULL, 1)));
    EXPECT_TRUE(std::isnan(quickstats::mad<double, double>(0, 0, NULL, 1)));
}

TEST(Mad, DenseInfinite) {
    const auto inf = std::numeric_limits<double>::infinity();
    auto dense_mad_with_infinities = [](std::vector<double> values, double median) {
        return quickstats::mad_with_infinities<double>(values.size(), values.data(), median);
    };

    {
        std::vector<double> dummy{ inf, inf, -inf, -inf, inf };
        EXPECT_EQ(dense_mad_with_infinities(dummy, inf), 0);
        EXPECT_EQ(dense_mad_with_infinities(dummy, 0.0), inf);
        EXPECT_EQ(dense_mad_with_infinities(dummy, -inf), inf);
    }

    {
        std::vector<double> dummy{ inf, inf, -inf, -inf, inf, 100.0 };
        EXPECT_EQ(dense_mad_with_infinities(dummy, inf), inf);
        EXPECT_EQ(dense_mad_with_infinities(dummy, 0.0), inf);
        EXPECT_EQ(dense_mad_with_infinities(dummy, -inf), inf);
    }

    {
        std::vector<double> dummy{ inf, inf, -inf, -inf, inf, -inf };
        EXPECT_EQ(dense_mad_with_infinities(dummy, inf), inf);
        EXPECT_EQ(dense_mad_with_infinities(dummy, 0.0), inf);
        EXPECT_EQ(dense_mad_with_infinities(dummy, -inf), inf);
    }

    {
        std::vector<double> dummy{ inf, inf, -inf, -inf, inf, -inf, -inf };
        EXPECT_EQ(dense_mad_with_infinities(dummy, inf), inf);
        EXPECT_EQ(dense_mad_with_infinities(dummy, 0.0), inf);
        EXPECT_EQ(dense_mad_with_infinities(dummy, -inf), 0);
    }
}

static double sparse_mad_with_infinities(std::vector<double> non_zeros, std::size_t num_total, double median) {
    return quickstats::mad_with_infinities(num_total, non_zeros.size(), non_zeros.data(), median);
}

static double dense_mad_with_infinities(std::vector<double> non_zeros, std::size_t num_total, double median) {
    non_zeros.resize(num_total);
    return quickstats::mad_with_infinities(num_total, non_zeros.data(), median);
}

TEST(Mad, SparseInfinite) {
    const auto inf = std::numeric_limits<double>::infinity();

    {
        std::vector<double> dummy{ inf, inf, -inf, -inf, inf };
        const auto limit = get_max_test_length(dummy.size());
        for (std::size_t total = dummy.size(); total <= limit; ++total) {
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, inf), dense_mad_with_infinities(dummy, total, inf));
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, 0.0), dense_mad_with_infinities(dummy, total, 0.0));
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, -inf), dense_mad_with_infinities(dummy, total, -inf));
        }
    }

    {
        std::vector<double> dummy{ inf, inf, -inf, -inf, inf, 100.0 };
        const auto limit = get_max_test_length(dummy.size());
        for (std::size_t total = dummy.size(); total <= limit; ++total) {
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, inf), dense_mad_with_infinities(dummy, total, inf));
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, 0.0), dense_mad_with_infinities(dummy, total, 0.0));
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, -inf), dense_mad_with_infinities(dummy, total, -inf));
        }
    }

    {
        std::vector<double> dummy{ inf, inf, -inf, -inf, inf, -inf };
        const auto limit = get_max_test_length(dummy.size());
        for (std::size_t total = dummy.size(); total <= limit; ++total) {
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, inf), dense_mad_with_infinities(dummy, total, inf));
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, 0.0), dense_mad_with_infinities(dummy, total, 0.0));
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, -inf), dense_mad_with_infinities(dummy, total, -inf));
        }
    }

    {
        std::vector<double> dummy{ inf, inf, -inf, -inf, inf, -inf, -inf };
        const auto limit = get_max_test_length(dummy.size());
        for (std::size_t total = dummy.size(); total <= limit; ++total) {
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, inf), dense_mad_with_infinities(dummy, total, inf));
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, 0.0), dense_mad_with_infinities(dummy, total, 0.0));
            EXPECT_EQ(sparse_mad_with_infinities(dummy, total, -inf), dense_mad_with_infinities(dummy, total, -inf));
        }
    }
}

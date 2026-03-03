#include <gtest/gtest.h>

#include <vector>
#include <cstddef>
#include <random>
#include <algorithm>

#include "quickstats/median.hpp"

#include "utils.h"

template<typename Value_>
static double direct_medians(const Value_* vec, std::size_t n) {
    std::vector<Value_> copy(vec, vec + n);
    return quickstats::median<double>(n, copy.data());
}

template<typename Value_>
static double direct_medians(const Value_* vec, std::size_t num_nonzero, std::size_t num_all) {
    std::vector<Value_> copy(vec, vec + num_nonzero);
    return quickstats::median<double>(num_all, num_nonzero, copy.data());
}

TEST(Median, DenseBasic) {
    std::vector<int> vec { 2, 1, 4, 5, 3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize), 3);
    EXPECT_EQ(direct_medians(vec.data() + 1, vsize - 1), 3.5);
    EXPECT_EQ(direct_medians(vec.data(), vsize - 1), 3);

    EXPECT_TRUE(std::isnan(quickstats::median<double>(0, static_cast<double*>(NULL))));
}

TEST(Median, DenseTies) {
    std::vector<int> vec { 1, 2, 3, 1, 2, 1 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize), 1.5);
    EXPECT_EQ(direct_medians(vec.data() + 1, vsize - 1), 2);

    // Make sure we get identical results when the midpoints are tied floating-point values.
    std::vector<double> frac_vec { 1.0/3, 2.0/10, 3.0/7, 1.0/3, 2.0/10, 1.0/3 };
    int fvsize = frac_vec.size();
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize), 1.0/3);
    EXPECT_EQ(direct_medians(frac_vec.data() + 1, fvsize - 1), 1.0/3);
}

TEST(Median, DenseInf) {
    auto inf = std::numeric_limits<double>::infinity();
    std::vector<double> vec { inf, inf, -inf, -inf };
    EXPECT_EQ(direct_medians(vec.data(), 2), inf);
    EXPECT_EQ(direct_medians(vec.data() + 2, 2), -inf);
    EXPECT_EQ(direct_medians(vec.data(), 3), inf);
    EXPECT_TRUE(std::isnan(direct_medians(vec.data(), 4)));
}

TEST(Median, SparseAllPositive) {
    std::vector<int> vec { 2, 1, 4, 5, 3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize, 5), 3);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 11), 0);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 10), 0.5);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 9), 1);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 8), 1.5);

    EXPECT_TRUE(std::isnan(quickstats::median<double>(0, 0, static_cast<double*>(NULL))));

    // Make sure we get identical results when the midpoints are tied floating-point values.
    std::vector<double> frac_vec { 1.0/3, 2.0/9, 3.0/7, 1.0/3, 2.0/9, 1.0/3 };
    int fvsize = frac_vec.size();
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, 6), 1.0/3);
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, 7), 1.0/3);
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, 8), (1.0/3 + 2.0/9) / 2);
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, 9), 2.0/9);
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, 10), 2.0/9);
}

TEST(Median, SparseAllNegative) {
    std::vector<int> vec { -2, -1, -4, -5, -3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize, 5), -3);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 11), 0);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 10), -0.5);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 9), -1);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 8), -1.5);
}

TEST(Median, SparseMixed) {
    // Mostly positive.
    {
        std::vector<double> vec { 2.5, -1, 4, -5, 3 };
        int vsize = vec.size();
        EXPECT_EQ(direct_medians(vec.data(), vsize, 5), 2.5);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 11), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 10), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 6), 1.25);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 7), 0);
    }

    // Mostly negative.
    {
        std::vector<double> vec { -2.5, 1, -4, 5, -3 };
        int vsize = vec.size();
        EXPECT_EQ(direct_medians(vec.data(), vsize, 5), -2.5);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 11), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 10), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 6), -1.25);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 7), 0);
    }

    // Equal numbers of positive and negative.
    {
        std::vector<double> vec { -2.5, 1, -4, 5, -3, 6 };
        int vsize = vec.size();
        EXPECT_FLOAT_EQ(direct_medians(vec.data(), vsize, 6), -0.75);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 13), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 12), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 7), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 8), 0);
    }
}

TEST(Median, SparseInf) {
    auto inf = std::numeric_limits<double>::infinity();
    std::vector<double> vec { inf, inf, inf, -inf, -inf, -inf };
    EXPECT_EQ(direct_medians(vec.data(), 3, 3), inf);
    EXPECT_EQ(direct_medians(vec.data(), 3, 4), inf);
    EXPECT_EQ(direct_medians(vec.data() + 3, 3), -inf);
    EXPECT_EQ(direct_medians(vec.data() + 3, 4), -inf);
    EXPECT_EQ(direct_medians(vec.data(), 4, 5), inf);
    EXPECT_TRUE(std::isnan(direct_medians(vec.data(), 6, 6)));
    EXPECT_EQ(direct_medians(vec.data(), 6, 8), 0);
}

/****************************************/

class RealisticMedianTest : public ::testing::TestWithParam<std::pair<int, int> > {};

TEST_P(RealisticMedianTest, Dense) {
    auto param = GetParam();
    std::mt19937_64 rng(69 + param.first * 2 + param.second);
    for (size_t n = 10; n < 100; n += 10) {
        auto contents = simulate_vector<double>(n, param.first, param.second, rng);

        // Even
        {
            auto copy = contents;
            std::sort(copy.begin(), copy.end());
            EXPECT_FLOAT_EQ(direct_medians(contents.data(), contents.size()), (copy[copy.size() / 2] + copy[copy.size() / 2 - 1]) / 2);
        }

        // Odd
        {
            auto copy = contents;
            copy.pop_back();
            std::sort(copy.begin(), copy.end());
            EXPECT_EQ(direct_medians(contents.data(), contents.size() - 1), copy[copy.size() / 2]);
        }
    }
}

TEST_P(RealisticMedianTest, Sparse) {
    auto param = GetParam();
    std::mt19937_64 rng(123 + param.first * 2 + param.second);
    for (int n = 10; n < 100; n += 5) { // alternate between even and odd.
        auto contents = simulate_vector<double>(n, param.first, param.second, rng);

        std::vector<double> copy(n), expected(n); 
        for (int i = 0; i < n; ++i) {
            std::copy_n(contents.data(), i, copy.data());
            std::copy_n(contents.data(), i, expected.data());
            std::fill(expected.begin() + i, expected.end(), 0);

            EXPECT_EQ(
                quickstats::median<double>(n, i, copy.data()),
                quickstats::median<double>(n, expected.data())
            );
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    Median,
    RealisticMedianTest,
    ::testing::Values(
        std::pair<double, double>(-10, 10),
        std::pair<double, double>(1, 10),
        std::pair<double, double>(-10, -1)
    )
);

/****************************************/

// Check that we behave correctly with integers.
TEST(Median, IntegerInput) {
    {
        std::vector<int> test{ 1, 2, 3, 4, 5, 6 };
        auto q = quickstats::median<double>(test.size(), test.data()); // even
        EXPECT_EQ(q, 3.5);

        q = quickstats::median<double>(test.size() - 1, test.data()); // odd
        EXPECT_EQ(q, 3);

        test = std::vector<int>{ 1, 2, 4, 4, 5, 6 }; // ties.
        q = quickstats::median<double>(test.size(), test.data());
        EXPECT_EQ(q, 4);
    }

    {
        std::vector<int> test{ 1, 2, 3, 4, 5, 6 };
        std::vector<double> dtest(test.size() * 2 + 1);
        for (int i = 6; i < 13; ++i) {
            dtest.clear();
            dtest.insert(dtest.end(), test.begin(), test.end());
            dtest.resize(i);
            auto q = quickstats::median<double, int>(i, test.size(), test.data());
            auto ref = quickstats::median<double, int>(dtest.size(), dtest.data());
            EXPECT_EQ(q, ref);
        }
    }

    {
        std::vector<int> test{ -1, -2, -3, -4, -5, -6 };
        std::vector<double> dtest(test.size() * 2 + 1);
        for (int i = 6; i < 13; ++i) {
            dtest.clear();
            dtest.insert(dtest.end(), test.begin(), test.end());
            dtest.resize(i);
            auto q = quickstats::median<double, int>(i, test.size(), test.data());
            auto ref = quickstats::median<double, int>(dtest.size(), dtest.data());
            EXPECT_EQ(q, ref);
        }
    }
}

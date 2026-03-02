#include <gtest/gtest.h>

#include <vector>
#include <cstddef>
#include <random>
#include <algorithm>

#include "quickstats/median.hpp"

#include "utils.h"

class ComputeMediansTest : public ::testing::Test {
protected:
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
};

TEST_F(ComputeMediansTest, DenseBasic) {
    std::vector<int> vec { 2, 1, 4, 5, 3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize), 3);
    EXPECT_EQ(direct_medians(vec.data() + 1, vsize - 1), 3.5);
    EXPECT_EQ(direct_medians(vec.data(), vsize - 1), 3);

    EXPECT_TRUE(std::isnan(quickstats::median<double>(0, static_cast<double*>(NULL))));
}

TEST_F(ComputeMediansTest, DenseTies) {
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

TEST_F(ComputeMediansTest, DenseRealistic) {
    for (size_t n = 10; n < 100; n += 10) {
        auto contents = simulate_vector<double>(n, -10, 10, /* seed = */ n + 1);

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

TEST_F(ComputeMediansTest, DenseInf) {
    auto inf = std::numeric_limits<double>::infinity();
    std::vector<double> vec { inf, inf, -inf, -inf };
    EXPECT_EQ(direct_medians(vec.data(), 2), inf);
    EXPECT_EQ(direct_medians(vec.data() + 2, 2), -inf);
    EXPECT_EQ(direct_medians(vec.data(), 3), inf);
    EXPECT_TRUE(std::isnan(direct_medians(vec.data(), 4)));
}

TEST_F(ComputeMediansTest, SparseAllPositive) {
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

TEST_F(ComputeMediansTest, SparseAllNegative) {
    std::vector<int> vec { -2, -1, -4, -5, -3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize, 5), -3);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 11), 0);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 10), -0.5);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 9), -1);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 8), -1.5);
}

TEST_F(ComputeMediansTest, SparseMixed) {
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

TEST_F(ComputeMediansTest, SparseInf) {
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

TEST_F(ComputeMediansTest, SparseRealistic) {
    for (int n = 10; n < 100; n += 5) {
        auto contents = simulate_vector<double>(n, -10, 10, /* seed = */ n + 1);

        {
            auto ref = direct_medians(contents.data(), n, n);
            EXPECT_EQ(ref, direct_medians(contents.data(), n));
        }

        // Replacing the back with a zero.
        {
            auto ref = direct_medians(contents.data(), n - 1, n);
            auto copy = contents;
            copy.back() = 0;
            EXPECT_EQ(ref, direct_medians(copy.data(), n));
        }

        // Adding an extra zero.
        {
            auto ref = direct_medians(contents.data(), n, n + 1);
            auto copy = contents;
            copy.push_back(0);
            EXPECT_EQ(ref, direct_medians(copy.data(), n + 1));
        }

        // Adding two extra zeros.
        {
            auto ref = direct_medians(contents.data(), n, n + 2);
            auto copy = contents;
            copy.push_back(0);
            copy.push_back(0);
            EXPECT_EQ(ref, direct_medians(copy.data(), n + 2));
        }
    }
}


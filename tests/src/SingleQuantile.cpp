#include <gtest/gtest.h>

#include <vector>
#include <cstddef>
#include <random>
#include <algorithm>

#include "quickstats/SingleQuantile.hpp"

#include "utils.h"

TEST(SingleQuantile, Dense) {
    std::vector<double> original { 0.7, 8.2, 2.3, 1.5, 8.8, 0.5 };
    const std::size_t n = static_cast<std::size_t>(original.size());
    ASSERT_EQ(n, 6); // make sure that num - 1 = 5 for exact quantile calculations. 

    // In general, we make a copy to preserve the original unordered state, otherwise the vector ends up sorted and the test is too easy. 

    {
        auto values = original;
        double val = quickstats::SingleQuantileFixedNumber<double, int>(n, 0)(values.data());
        EXPECT_FLOAT_EQ(val, 0.5);
    }

    {
        auto values = original;
        double val = quickstats::SingleQuantileFixedNumber<double, int>(n, 0.1)(values.data());
        EXPECT_FLOAT_EQ(val, 0.6);
    }

    {
        auto values = original;
        double val = quickstats::SingleQuantileFixedNumber<double, int>(n, 0.25)(values.data());
        EXPECT_FLOAT_EQ(val, 0.9);
    }

    {
        auto values = original;
        double val = quickstats::SingleQuantileFixedNumber<double, int>(n, 0.5)(values.data());
        EXPECT_FLOAT_EQ(val, 1.9);
    }

    {
        auto values = original;
        double val = quickstats::SingleQuantileFixedNumber<double, int>(n, 0.75)(values.data());
        EXPECT_FLOAT_EQ(val, 6.725);
    }

    {
        auto values = original;
        double val = quickstats::SingleQuantileFixedNumber<double, int>(n, 0.8)(values.data());
        EXPECT_FLOAT_EQ(val, 8.2);
    }

    {
        auto values = original;
        double val = quickstats::SingleQuantileFixedNumber<double, int>(n, 1)(values.data());
        EXPECT_FLOAT_EQ(val, 8.8);
    }

    // Works with some lower precisions too.
    {
        auto values = original;
        auto fval = quickstats::SingleQuantileFixedNumber<float, int>(n, 1)(values.data());
        EXPECT_FLOAT_EQ(static_cast<double>(fval), 8.8);
    }

    {
        std::vector<float> fvalues(original.begin(), original.end());
        auto val = quickstats::SingleQuantileFixedNumber<double, int>(n, 1)(fvalues.data());
        EXPECT_FLOAT_EQ(val, 8.8);
    }
}

/****************************************/

// Checking all possibilities of zero and non-zero values for a variety of quantiles.

class SingleQuantileSparseTest : public ::testing::TestWithParam<double> {};

TEST_P(SingleQuantileSparseTest, Mixed) {
    std::vector<double> original { 0.70, -0.87, 0.55, 1.77, -0.76, -0.98, 0.58, -0.34, -1.00, -1.29, 0.13 };
    ASSERT_EQ(original.size(), 11); // make sure that num - 1 = 10 for exact quantile calculations. 

    quickstats::SingleQuantileFixedNumber<double, int> qcalcs(original.size(), GetParam());

    std::vector<double> expected(original.size());
    std::vector<double> copy(original.size());

    for (std::size_t num_nonzero = 0; num_nonzero <= original.size(); ++num_nonzero) {
        // In general, we make a copy to preserve the original unordered state, otherwise the vector ends up sorted and the test is too easy. 
        std::copy_n(original.data(), num_nonzero, copy.data());
        std::copy_n(original.data(), num_nonzero, expected.data());
        std::fill(expected.begin() + num_nonzero, expected.end(), 0);
        EXPECT_FLOAT_EQ(qcalcs(num_nonzero, copy.data()), qcalcs(expected.data()));
    }
}

TEST_P(SingleQuantileSparseTest, Positive) {
    std::vector<double> original { 1.5, 7.7, 8.3, 3.9, 2.3, 8.6, 9.4, 9.2, 0.1, 4.1, 5.4 };
    ASSERT_EQ(original.size(), 11); // make sure that num - 1 = 10 for exact quantile calculations. 

    quickstats::SingleQuantileFixedNumber<double, int> qcalcs(original.size(), GetParam());

    std::vector<double> expected(original.size());
    std::vector<double> copy(original.size());
    for (std::size_t num_nonzero = 0; num_nonzero <= original.size(); ++num_nonzero) {
        // In general, we make a copy to preserve the original unordered state, otherwise the vector ends up sorted and the test is too easy. 
        std::copy_n(original.data(), num_nonzero, copy.data());
        std::copy_n(original.data(), num_nonzero, expected.data());
        std::fill(expected.begin() + num_nonzero, expected.end(), 0);
        EXPECT_FLOAT_EQ(qcalcs(num_nonzero, copy.data()), qcalcs(expected.data()));
    }
}

TEST_P(SingleQuantileSparseTest, Negative) {
    std::vector<double> original { -6.1, -6.5, -5.9, -2.2, -2.2, -7.1, -5.2, -9.8, -2.1, -2.2, -7.8 };
    ASSERT_EQ(original.size(), 11); // make sure that num - 1 = 10 for exact quantile calculations. 

    quickstats::SingleQuantileFixedNumber<double, int> qcalcs(original.size(), GetParam());

    std::vector<double> expected(original.size());
    std::vector<double> copy(original.size());
    for (std::size_t num_nonzero = 0; num_nonzero <= original.size(); ++num_nonzero) {
        // In general, we make a copy to preserve the original unordered state, otherwise the vector ends up sorted and the test is too easy. 
        std::copy_n(original.data(), num_nonzero, copy.data());
        std::copy_n(original.data(), num_nonzero, expected.data());
        std::fill(expected.begin() + num_nonzero, expected.end(), 0);
        EXPECT_FLOAT_EQ(qcalcs(num_nonzero, copy.data()), qcalcs(expected.data()));
    }
}

INSTANTIATE_TEST_SUITE_P(
    SingleQuantile,
    SingleQuantileSparseTest,
    ::testing::Values(0, 0.12, 0.2, 0.27, 0.4, 0.49, 0.5, 0.51, 0.8, 0.99, 1.0)
);

/****************************************/

class ComplexSingleQuantileTest : public ::testing::TestWithParam<std::pair<int, int> > {};

TEST_P(ComplexSingleQuantileTest, Dense) {
    const std::size_t n = 54; // n - 1 should be prime so that we can use EXPECT_LT, GT, without worrying about float equality.
    auto param = GetParam();
    std::mt19937_64 rng(69 + param.first * 2 + param.second);
    auto original = simulate_vector<double>(n, param.first, param.second, rng);

    for (int val = 1; val < 100; ++val) {
        const double prop = static_cast<double>(val)/100.0;
        auto values = original;
        const double Q = quickstats::SingleQuantileFixedNumber<double, int>(n, prop)(values.data());

        double sum = 0;
        for (auto x : original) {
            sum += (x <= Q);
        }

        EXPECT_GT(prop, (sum-1)/(n-1));
        EXPECT_LT(prop, sum/(n-1));
    }

    {
        auto values = original;
        const double Q = quickstats::SingleQuantileFixedNumber<double, int>(n, 0)(values.data());
        EXPECT_EQ(Q, *std::min_element(original.begin(), original.end()));
    }

    {
        auto values = original;
        const double Q = quickstats::SingleQuantileFixedNumber<double, int>(n, 1)(values.data());
        EXPECT_EQ(Q, *std::max_element(original.begin(), original.end()));
    }
}

TEST_P(ComplexSingleQuantileTest, Sparse) {
    const std::size_t n = 62; // n - 1 should be prime so that we can use EXPECT_LT, GT, without worrying about float equality.
    auto param = GetParam();
    std::mt19937_64 rng(42 + param.first + param.second * 2);
    auto original = simulate_vector<double>(n, param.first, param.second, rng);

    for (int val = 0; val < 100; ++val) {
        const double prop = static_cast<double>(val)/100.0;
        quickstats::SingleQuantileFixedNumber<double, int> Q(n, prop);

        std::vector<double> copy(n), expected(n); 
        for (std::size_t i = 0; i <= n; ++i) {
            std::copy_n(original.data(), i, copy.data());
            std::copy_n(original.data(), i, expected.data());
            std::fill(expected.begin() + i, expected.end(), 0);
            EXPECT_FLOAT_EQ(Q(i, copy.data()), Q(expected.data()));
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    SingleQuantile,
    ComplexSingleQuantileTest,
    ::testing::Values(
        std::pair<double, double>(-10, 10),
        std::pair<double, double>(1, 10),
        std::pair<double, double>(-10, -1)
    )
);

/****************************************/

TEST(SingleQuantile, InvalidArguments) {
    std::string msg;
    try {
        quickstats::SingleQuantileFixedNumber<double, int>(3, -1);
    } catch (std::exception& e) {
        msg = e.what();
    }
    EXPECT_TRUE(msg.find("[0, 1]") != std::string::npos);

    msg.clear();
    try {
        quickstats::SingleQuantileFixedNumber<double, int>(3, 10);
    } catch (std::exception& e) {
        msg = e.what();
    }
    EXPECT_TRUE(msg.find("[0, 1]") != std::string::npos);

    msg.clear();
    try {
        quickstats::SingleQuantileFixedNumber<double, int>(0, 1);
    } catch (std::exception& e) {
        msg = e.what();
    }
    EXPECT_TRUE(msg.find("positive") != std::string::npos);
}

TEST(SingleQuantile, FloatConversion) {
    // Check that we behave correctly if the conversion of 'len - 1' to a single-precision float is successful but inexact,
    // such that it rounds up, and then when we convert back, the final value exceeds 'len - 1'.
    int inaccurate = 0;
    std::vector<float> non_zero_values(1000, 123);

    for (int i = 0; i < 100; ++i) {
        std::int32_t x = i + static_cast<std::int32_t>(1e8);

        // Check that it's actually inexact.
        auto x_m1 = x - 1;
        float y = x_m1;
        std::int32_t x_back = y;
        inaccurate += (x_m1 < x_back);

        quickstats::SingleQuantileFixedNumber<float, std::int32_t> test(x, 1);

        // We want to check that we hard-cap the upper_index correctly to avoid out-of-bounds access.
        EXPECT_EQ(test(non_zero_values.size(), non_zero_values.data()), 123);
    }

    EXPECT_GT(inaccurate, 0);
}

TEST(SingleQuantileVariable, Dense) {
    std::vector<double> original { 0, 1, 2, 3, 4, 5 };
    quickstats::SingleQuantileVariableNumber<double, int> calc(original.size(), 0.5);

    {
        auto values = original;
        auto Q = calc(0, values.data());
        EXPECT_TRUE(std::isnan(Q));
    }

    {
        auto values = original;
        auto Q = calc(1, values.data());
        EXPECT_EQ(Q, 0);
    }

    {
        auto values = original;
        auto Q = calc(2, values.data());
        EXPECT_FLOAT_EQ(Q, 0.5);
    }

    {
        auto values = original;
        auto Q = calc(values.size(), values.data());
        EXPECT_FLOAT_EQ(Q, 2.5);
    }

    // Re-using the cached calculator.
    {
        auto values = original;
        auto Q = calc(2, values.data() + 2);
        EXPECT_FLOAT_EQ(Q, 2.5);
    }
}

TEST(SingleQuantileVariable, Sparse) {
    std::vector<double> original { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    quickstats::SingleQuantileVariableNumber<double, int> calc(11, 0.5);

    {
        auto values = original;
        auto Q = calc(0, 0, values.data());
        EXPECT_TRUE(std::isnan(Q));
    }

    {
        auto values = original;
        EXPECT_EQ(calc(1, 0, values.data() + 1), 0);
        EXPECT_EQ(calc(1, 1, values.data() + 1), 1);
    }

    {
        auto values = original;
        EXPECT_EQ(calc(2, 0, values.data() + 1), 0);
        EXPECT_EQ(calc(2, 1, values.data() + 1), 0.5);
        EXPECT_EQ(calc(2, 2, values.data() + 1), 1.5);
    }

    {
        auto values = original;
        EXPECT_EQ(calc(11, 0, values.data()), 0);
        EXPECT_EQ(calc(11, 7, values.data()), 1);
        EXPECT_EQ(calc(11, 11, values.data()), 5);
    }
}

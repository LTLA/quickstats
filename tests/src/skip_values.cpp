#include <gtest/gtest.h>
#include "quickstats/skip_values.hpp"

TEST(SkipValues, Basic) {
    {
        std::vector<double> input{ 1, 2, 3, 4, 5 };
        auto kept = quickstats::skip_values(input.size(), input.data(), [&](std::size_t, double val) -> bool { return val < 3; });
        EXPECT_EQ(kept, 3);
        input.resize(kept);
        std::vector<double> after{ 3, 4, 5 };
        EXPECT_EQ(input, after);
    }

    {
        std::vector<double> input{ 1, 2, 3, 4, 5 };
        auto kept = quickstats::skip_values(input.size(), input.data(), [&](std::size_t i, double) -> bool { return i % 2 == 1; });
        EXPECT_EQ(kept, 3);
        input.resize(kept);
        std::vector<double> after{ 1, 3, 5 };
        EXPECT_EQ(input, after);
    }

    {
        std::vector<double> input{ 5, 4, 3, 2, 1 };
        auto kept = quickstats::skip_values(input.size(), input.data(), [&](std::size_t, double) -> bool { return false; });
        EXPECT_EQ(kept, 5);
        std::vector<double> after{ 5, 4, 3, 2, 1 };
        EXPECT_EQ(input, after);
    }

    // Behaves on an empty object.
    {
        std::vector<double> input;
        auto kept = quickstats::skip_values(input.size(), input.data(), [&](std::size_t, double) -> bool { return false; });
        EXPECT_EQ(kept, 0);
    }
}

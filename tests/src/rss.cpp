#include <gtest/gtest.h>

#include <vector>
#include <cstddef>
#include <random>
#include <algorithm>

#include "quickstats/rss.hpp"

#include "utils.h"

TEST(Rss, Dense) {
    std::vector<double> vec { 2.2, 1.1, 4.4, 5.5, 3.3 };

    {
        const auto rout = quickstats::rss(vec.size(), vec.data());
        EXPECT_FLOAT_EQ(rout.mean, 3.3);
        EXPECT_FLOAT_EQ(rout.rss, 12.1);
    }

    {
        const auto rout = quickstats::rss(vec.size() - 1, vec.data() + 1);
        EXPECT_FLOAT_EQ(rout.mean, 3.575);
        EXPECT_FLOAT_EQ(rout.rss, 10.5875);
    }

    {
        const auto rout = quickstats::rss(vec.size() - 1, vec.data());
        EXPECT_FLOAT_EQ(rout.mean, 3.3);
        EXPECT_FLOAT_EQ(rout.rss, 12.1); 
    }

    {
        const auto rout = quickstats::rss(0, static_cast<double*>(NULL));
        EXPECT_TRUE(std::isnan(rout.mean));
        EXPECT_EQ(rout.rss, 0);
    }
}

TEST(Rss, Sparse) {
    std::vector<double> vec { 2.2, 1.1, 4.4, 5.5, 3.3 };

    {
        const auto rout = quickstats::rss(10, static_cast<int>(vec.size()), vec.data());
        EXPECT_FLOAT_EQ(rout.mean, 1.65);
        EXPECT_FLOAT_EQ(rout.rss, 39.325);
    }

    {
        const auto rout = quickstats::rss(8, static_cast<int>(vec.size()), vec.data());
        EXPECT_FLOAT_EQ(rout.mean, 2.0625);
        EXPECT_FLOAT_EQ(rout.rss, 32.51875);
    }
}

TEST(Rss, Integer) {
    std::vector<int> vec { 2, 7, 4, 5, 8 };

    {
        const auto rout = quickstats::rss(vec.size(), vec.data());
        EXPECT_FLOAT_EQ(rout.mean, 5.2);
        EXPECT_FLOAT_EQ(rout.rss, 22.8);
    }

    {
        const auto rout = quickstats::rss(10, static_cast<int>(vec.size()), vec.data());
        EXPECT_FLOAT_EQ(rout.mean, 2.6);
        EXPECT_FLOAT_EQ(rout.rss, 90.4);
    }
}

/***************************************/

class RssRunningDenseTest : public ::testing::TestWithParam<std::pair<int, int> > {};

TEST_P(RssRunningDenseTest, Simple) {
    const auto params = GetParam();
    const std::size_t num_obj = params.first; 
    const std::size_t num_obs = params.second;

    std::vector<std::vector<double> > collected(num_obj);
    std::mt19937_64 rng(/* seed = */ num_obs + num_obj);

    auto mean = sanisizer::create<std::vector<double> >(num_obj);
    auto rss = sanisizer::create<std::vector<double> >(num_obj);
    quickstats::RssRunningDense<double> running(num_obj, mean.data(), rss.data());

    for (std::size_t i = 0; i < num_obs; ++i) {
        auto sim = simulate_vector<double>(num_obj, -10.0, 10.0, rng);
        running.add(sim.data());
        for (std::size_t j = 0; j < num_obj; ++j) {
            collected[j].push_back(sim[j]);
        }
    }

    running.finish();

    for (std::size_t j = 0; j < num_obj; ++j) {
        const auto stats = quickstats::rss(collected[j].size(), collected[j].data());
        almost_equal_floats(stats.mean, mean[j]);
        almost_equal_floats(stats.rss, rss[j]);
    }
}

TEST_P(RssRunningDenseTest, SimpleInteger) {
    const auto params = GetParam();
    const std::size_t num_obj = params.first; 
    const std::size_t num_obs = params.second;

    std::vector<std::vector<int> > collected(num_obj);
    std::mt19937_64 rng(/* seed = */ num_obs + num_obj);

    auto mean = sanisizer::create<std::vector<double> >(num_obj);
    auto rss = sanisizer::create<std::vector<double> >(num_obj);
    quickstats::RssRunningDense<int> running(num_obj, mean.data(), rss.data());

    for (std::size_t i = 0; i < num_obs; ++i) {
        auto sim = simulate_vector<int>(num_obj, -10, 10, rng);
        running.add(sim.data());
        for (std::size_t j = 0; j < num_obj; ++j) {
            collected[j].push_back(sim[j]);
        }
    }

    running.finish();

    for (std::size_t j = 0; j < num_obj; ++j) {
        const auto stats = quickstats::rss(collected[j].size(), collected[j].data());
        almost_equal_floats(stats.mean, mean[j]);
        almost_equal_floats(stats.rss, rss[j]);
    }
}

TEST_P(RssRunningDenseTest, Skip) {
    const auto params = GetParam();
    const std::size_t num_obj = params.first; 
    const std::size_t num_obs = params.second;

    std::vector<std::vector<double> > collected(num_obj);
    std::mt19937_64 rng(/* seed = */ num_obs + num_obj);

    auto mean = sanisizer::create<std::vector<double> >(num_obj);
    auto rss = sanisizer::create<std::vector<double> >(num_obj);
    auto used = sanisizer::create<std::vector<int> >(num_obj);
    quickstats::RssRunningDenseSkip<int, double> running(num_obj, mean.data(), rss.data(), used.data());

    std::uniform_real_distribution<double> unit(0, 1);
    for (std::size_t i = 0; i < num_obs; ++i) {
        auto sim = simulate_vector<double>(num_obj, -10.0, 10.0, rng);
        for (std::size_t j = 0; j < num_obj; ++j) {
            if (unit(rng) < 0.2) {
                sim[j] = std::numeric_limits<double>::quiet_NaN();
            } else {
                collected[j].push_back(sim[j]);
            }
        }

        running.add(
            sim.data(),
            [&](std::size_t, double val) -> bool {
                return std::isnan(val);
            }
        );
    }

    running.finish();

    for (std::size_t j = 0; j < num_obj; ++j) {
        const auto stats = quickstats::rss(collected[j].size(), collected[j].data());
        almost_equal_floats(stats.mean, mean[j]);
        almost_equal_floats(stats.rss, rss[j]);
        EXPECT_EQ(used[j], collected[j].size());
    }
}

TEST_P(RssRunningDenseTest, SkipInteger) {
    const auto params = GetParam();
    const std::size_t num_obj = params.first; 
    const std::size_t num_obs = params.second;

    std::vector<std::vector<int> > collected(num_obj);
    std::mt19937_64 rng(/* seed = */ num_obs + num_obj);

    auto mean = sanisizer::create<std::vector<double> >(num_obj);
    auto rss = sanisizer::create<std::vector<double> >(num_obj);
    auto used = sanisizer::create<std::vector<int> >(num_obj);
    quickstats::RssRunningDenseSkip<int, int> running(num_obj, mean.data(), rss.data(), used.data());

    std::uniform_real_distribution<double> unit(0, 1);
    for (std::size_t i = 0; i < num_obs; ++i) {
        auto sim = simulate_vector<int>(num_obj, -10, 10, rng);
        for (std::size_t j = 0; j < num_obj; ++j) {
            if (unit(rng) < 0.2) {
                sim[j] = -1000;
            } else {
                collected[j].push_back(sim[j]);
            }
        }

        running.add(
            sim.data(),
            [&](std::size_t, int val) -> bool {
                return val < -10;
            }
        );
    }

    running.finish();

    for (std::size_t j = 0; j < num_obj; ++j) {
        const auto stats = quickstats::rss(collected[j].size(), collected[j].data());
        almost_equal_floats(stats.mean, mean[j]);
        almost_equal_floats(stats.rss, rss[j]);
        EXPECT_EQ(used[j], collected[j].size());
    }
}

INSTANTIATE_TEST_SUITE_P(
    Rss,
    RssRunningDenseTest,
    ::testing::Values(
        std::make_pair(299, 21),
        std::make_pair(43, 195)
    )
);

/***************************************/

class RssRunningSparseTest : public ::testing::TestWithParam<std::pair<int, int> > {};

TEST_P(RssRunningSparseTest, Simple) {
    const auto params = GetParam();
    const std::size_t num_obj = params.first; 
    const std::size_t num_obs = params.second;

    std::vector<std::vector<double> > collected(num_obj);
    std::mt19937_64 rng(/* seed = */ num_obs + num_obj);

    auto mean = sanisizer::create<std::vector<double> >(num_obj);
    auto rss = sanisizer::create<std::vector<double> >(num_obj);
    auto nnz = sanisizer::create<std::vector<int> >(num_obj);
    quickstats::RssRunningSparse<int, double> running(num_obj, mean.data(), rss.data(), nnz.data());

    std::uniform_real_distribution<double> unit(0, 1);
    for (std::size_t i = 0; i < num_obs; ++i) {
        std::vector<int> indices;
        for (std::size_t j = 0; j < num_obj; ++j) {
            if (unit(rng) <= 0.2) {
                indices.push_back(j);
            }
        }

        const std::size_t num_nz = indices.size();
        auto values = simulate_vector<double>(num_nz, -10.0, 10.0, rng);
        running.add(num_nz, values.data(), indices.data());

        for (std::size_t z = 0; z < num_nz; ++z) {
            collected[indices[z]].push_back(values[z]);
        }
    }

    running.finish();

    for (std::size_t j = 0; j < num_obj; ++j) {
        const auto stats = quickstats::rss(num_obs, collected[j].size(), collected[j].data());
        almost_equal_floats(stats.mean, mean[j]);
        almost_equal_floats(stats.rss, rss[j]);
        EXPECT_EQ(collected[j].size(), nnz[j]);
    }
}

TEST_P(RssRunningSparseTest, SimpleInteger) {
    const auto params = GetParam();
    const std::size_t num_obj = params.first; 
    const std::size_t num_obs = params.second;

    std::vector<std::vector<int> > collected(num_obj);
    std::mt19937_64 rng(/* seed = */ num_obs + num_obj);

    auto mean = sanisizer::create<std::vector<double> >(num_obj);
    auto rss = sanisizer::create<std::vector<double> >(num_obj);
    auto nnz = sanisizer::create<std::vector<int> >(num_obj);
    quickstats::RssRunningSparse<int, int> running(num_obj, mean.data(), rss.data(), nnz.data());

    std::uniform_real_distribution<double> unit(0, 1);

    for (std::size_t i = 0; i < num_obs; ++i) {
        std::vector<int> indices;
        for (std::size_t j = 0; j < num_obj; ++j) {
            if (unit(rng) < 0.2) {
                indices.push_back(j);
            }
        }

        const std::size_t num_nz = indices.size();
        auto values = simulate_vector<int>(num_nz, -10, 10, rng);
        running.add(num_nz, values.data(), indices.data());

        for (std::size_t z = 0; z < num_nz; ++z) {
            collected[indices[z]].push_back(values[z]);
        }
    }

    running.finish();

    for (std::size_t j = 0; j < num_obj; ++j) {
        const auto stats = quickstats::rss(num_obs, collected[j].size(), collected[j].data());
        almost_equal_floats(stats.mean, mean[j]);
        almost_equal_floats(stats.rss, rss[j]);
        EXPECT_EQ(collected[j].size(), nnz[j]);
    }
}

TEST_P(RssRunningSparseTest, Skip) {
    const auto params = GetParam();
    const std::size_t num_obj = params.first; 
    const std::size_t num_obs = params.second;

    std::vector<std::vector<double> > collected(num_obj);
    std::vector<int> lost(num_obj);
    std::mt19937_64 rng(/* seed = */ num_obs + num_obj);

    auto mean = sanisizer::create<std::vector<double> >(num_obj);
    auto rss = sanisizer::create<std::vector<double> >(num_obj);
    auto nnz = sanisizer::create<std::vector<int> >(num_obj);
    auto used = sanisizer::create<std::vector<int> >(num_obj);
    quickstats::RssRunningSparseSkip<int, double> running(num_obj, mean.data(), rss.data(), nnz.data(), used.data());

    std::uniform_real_distribution<double> unit(0, 1);
    std::uniform_real_distribution<double> dist(-10.0, 10.0);
    for (std::size_t i = 0; i < num_obs; ++i) {
        std::vector<double> values;
        std::vector<int> indices;

        for (std::size_t j = 0; j < num_obj; ++j) {
            const double prob = unit(rng);
            if (prob < 0.2) {
                values.push_back(std::numeric_limits<double>::quiet_NaN());
                indices.push_back(j);
                lost[j] += 1;
            } else if (prob < 0.5) {
                values.push_back(dist(rng));
                indices.push_back(j);
                collected[j].push_back(values.back());
            }
        }

        running.add(
            values.size(),
            values.data(),
            indices.data(),
            [&](std::size_t, double val) -> bool {
                return std::isnan(val);
            }
        );
    }

    running.finish();

    for (std::size_t j = 0; j < num_obj; ++j) {
        const auto stats = quickstats::rss(num_obs - lost[j], collected[j].size(), collected[j].data());
        almost_equal_floats(stats.mean, mean[j]);
        almost_equal_floats(stats.rss, rss[j]);
        EXPECT_EQ(nnz[j], collected[j].size());
        EXPECT_EQ(used[j], num_obs - lost[j]);
    }
}

TEST_P(RssRunningSparseTest, SkipInteger) {
    const auto params = GetParam();
    const std::size_t num_obj = params.first; 
    const std::size_t num_obs = params.second;

    std::vector<std::vector<int> > collected(num_obj);
    std::vector<int> lost(num_obj);
    std::mt19937_64 rng(/* seed = */ num_obs + num_obj);

    auto mean = sanisizer::create<std::vector<double> >(num_obj);
    auto rss = sanisizer::create<std::vector<double> >(num_obj);
    auto nnz = sanisizer::create<std::vector<int> >(num_obj);
    auto used = sanisizer::create<std::vector<int> >(num_obj);
    quickstats::RssRunningSparseSkip<int, int> running(num_obj, mean.data(), rss.data(), nnz.data(), used.data());

    std::uniform_real_distribution<double> unit(0, 1);
    std::uniform_int_distribution<int> dist(-10, 10);
    for (std::size_t i = 0; i < num_obs; ++i) {
        std::vector<int> values;
        std::vector<int> indices;

        for (std::size_t j = 0; j < num_obj; ++j) {
            const double prob = unit(rng);
            if (prob < 0.2) {
                values.push_back(-1000);
                indices.push_back(j);
                lost[j] += 1;
            } else if (prob < 0.5) {
                values.push_back(dist(rng));
                indices.push_back(j);
                collected[j].push_back(values.back());
            }
        }

        running.add(
            values.size(),
            values.data(),
            indices.data(),
            [&](std::size_t, double val) -> bool {
                return val < -10;
            }
        );
    }

    running.finish();

    for (std::size_t j = 0; j < num_obj; ++j) {
        const auto stats = quickstats::rss(num_obs - lost[j], collected[j].size(), collected[j].data());
        almost_equal_floats(stats.mean, mean[j]);
        almost_equal_floats(stats.rss, rss[j]);
        EXPECT_EQ(nnz[j], collected[j].size());
        EXPECT_EQ(used[j], num_obs - lost[j]);
    }
}

INSTANTIATE_TEST_SUITE_P(
    Rss,
    RssRunningSparseTest,
    ::testing::Values(
        std::make_pair(259, 31),
        std::make_pair(73, 125)
    )
);

/***************************************/

TEST(Rss, RunningNone) {
    const std::size_t num_obj = 50;

    // Dense.
    {
        auto mean = sanisizer::create<std::vector<double> >(num_obj);
        auto rss = sanisizer::create<std::vector<double> >(num_obj);
        quickstats::RssRunningDense<int, double> running(num_obj, mean.data(), rss.data());
        running.finish();

        for (std::size_t i = 0; i < num_obj; ++i) {
            EXPECT_TRUE(std::isnan(mean[i]));
            EXPECT_EQ(rss[i], 0);
        }
    }

    // Sparse.
    {
        auto mean = sanisizer::create<std::vector<double> >(num_obj);
        auto rss = sanisizer::create<std::vector<double> >(num_obj);
        auto nnz = sanisizer::create<std::vector<int> >(num_obj);
        quickstats::RssRunningSparse<int, double> running(num_obj, mean.data(), rss.data(), nnz.data());
        running.finish();

        for (std::size_t i = 0; i < num_obj; ++i) {
            EXPECT_TRUE(std::isnan(mean[i]));
            EXPECT_EQ(rss[i], 0);
            EXPECT_EQ(nnz[i], 0);
        }
    }
}

TEST(Rss, RunningSkipAll) {
    const std::size_t num_obj = 50;
    const std::size_t num_obs = 20;

    // Dense.
    {
        auto mean = sanisizer::create<std::vector<double> >(num_obj);
        auto rss = sanisizer::create<std::vector<double> >(num_obj);
        auto unskipped = sanisizer::create<std::vector<int> >(num_obj);
        quickstats::RssRunningDenseSkip<int, double> running(num_obj, mean.data(), rss.data(), unskipped.data());

        for (std::size_t o = 0; o < num_obs; ++o) {
            std::vector<double> values(num_obj);
            running.add(
                values.data(),
                [&](int i, double) -> bool {
                    return i % 2 == 0;
                }
            );
        }

        running.finish();

        for (std::size_t i = 0; i < num_obj; ++i) {
            if (i % 2 == 0) {
                EXPECT_TRUE(std::isnan(mean[i]));
                EXPECT_EQ(unskipped[i], 0);
            } else {
                EXPECT_EQ(mean[i], 0);
                EXPECT_EQ(unskipped[i], num_obs);
            }
            EXPECT_EQ(rss[i], 0);
        }
    }

    // Sparse.
    {
        auto mean = sanisizer::create<std::vector<double> >(num_obj);
        auto rss = sanisizer::create<std::vector<double> >(num_obj);
        auto nnz = sanisizer::create<std::vector<int> >(num_obj);
        auto unskipped = sanisizer::create<std::vector<int> >(num_obj);
        quickstats::RssRunningSparseSkip<int, double> running(num_obj, mean.data(), rss.data(), nnz.data(), unskipped.data());

        for (std::size_t o = 0; o < num_obs; ++o) {
            std::vector<double> values;
            std::vector<int> indices;

            for (std::size_t i = 0; i < num_obj; i += 2) {
                values.push_back(std::numeric_limits<double>::quiet_NaN());
                indices.push_back(i);
            }

            running.add(
                values.size(), 
                values.data(),
                indices.data(),
                [&](int, double) -> bool {
                    return true;
                }
            );
        }

        running.finish();

        for (std::size_t i = 0; i < num_obj; ++i) {
            if (i % 2 == 0) {
                EXPECT_TRUE(std::isnan(mean[i]));
                EXPECT_EQ(unskipped[i], 0);
            } else {
                EXPECT_EQ(mean[i], 0);
                EXPECT_EQ(unskipped[i], num_obs);
            }
            EXPECT_EQ(rss[i], 0);
            EXPECT_EQ(nnz[i], 0);
        }
    }
}

/***************************************/

TEST(Rss, Recenter) {
    std::mt19937_64 rng(1234);

    // Into 2.
    {
        auto values = simulate_vector<double>(19, -10.0, 10.0, rng);
        auto first = quickstats::rss(9, values.data());
        auto second = quickstats::rss(10, values.data() + 9);

        auto all = quickstats::rss(19, values.data());
        auto combined = quickstats::recenter_rss(9, first.rss, first.mean, all.mean)
            + quickstats::recenter_rss(10, second.rss, second.mean, all.mean);

        almost_equal_floats(combined, all.rss);
    }

    // Into 5.
    {
        auto values = simulate_vector<double>(31, -10.0, 10.0, rng);
        auto res1 = quickstats::rss(2, values.data());
        auto res2 = quickstats::rss(4, values.data() + 2);
        auto res3 = quickstats::rss(8, values.data() + 6);
        auto res4 = quickstats::rss(16, values.data() + 14);
        auto res5 = quickstats::rss(1, values.data() + 30);

        auto all = quickstats::rss(31, values.data());
        auto combined = quickstats::recenter_rss(2, res1.rss, res1.mean, all.mean)
            + quickstats::recenter_rss(4, res2.rss, res2.mean, all.mean)
            + quickstats::recenter_rss(8, res3.rss, res3.mean, all.mean)
            + quickstats::recenter_rss(16, res4.rss, res4.mean, all.mean)
            + quickstats::recenter_rss(1, res5.rss, res5.mean, all.mean);

        almost_equal_floats(combined, all.rss);
    }

    // Ignores an NaN mean.
    {
        auto res = quickstats::rss(0, static_cast<double*>(NULL));
        EXPECT_EQ(quickstats::recenter_rss(0, res.rss, res.mean, 50.0), 0);
    }
}


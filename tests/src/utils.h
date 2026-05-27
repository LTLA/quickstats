#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <cstddef>
#include <random>

#include <gtest/gtest.h>

#include "sanisizer/sanisizer.hpp"

template<typename Value_, class Rng_>
std::vector<Value_> simulate_vector(std::size_t n, Value_ lower, Value_ upper, Rng_& rng) {
    auto output = sanisizer::create<std::vector<Value_> >(n);
    std::uniform_real_distribution<> unif(lower, upper);
    for (auto& v : output) {
        v = unif(rng);
    }
    return output;
}

template<typename Value_>
void almost_equal_floats(Value_ left, Value_ right) {
    constexpr double tol = 1e-8;
    if (std::abs(left) <= tol) {
        EXPECT_LT(std::abs(right), tol) << "discrepancy between " << left <<  " and " << right;
    } else {
        EXPECT_LT(std::abs(left - right), tol * (std::abs(left) + std::abs(right)) / 2) << "discrepancy between " << left <<  " and " << right;
    }
}

#endif

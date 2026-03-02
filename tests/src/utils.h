#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <cstddef>
#include <random>

#include "sanisizer/sanisizer.hpp"

template<typename Value_>
std::vector<Value_> simulate_vector(std::size_t n, Value_ lower, Value_ upper, unsigned long long seed) {
    auto output = sanisizer::create<std::vector<Value_> >(n);
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<> unif(lower, upper);
    for (auto& v : output) {
        v = unif(rng);
    }
    return output;
}

#endif

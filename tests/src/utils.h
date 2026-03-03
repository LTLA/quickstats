#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <cstddef>
#include <random>

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

#endif

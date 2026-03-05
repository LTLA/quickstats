#ifndef QUICKSTATS_UTILS_HPP
#define QUICKSTATS_UTILS_HPP

#include <type_traits>
#include <cmath>

namespace quickstats {

template<typename Value_>
using I = std::remove_cv_t<std::remove_reference_t<Value_> >;

template<typename Value_>
Value_ interpolate(const Value_ left, const Value_ right, const Value_ right_frac) {
    if (std::isinf(left)) {
        // If only left is +/-Inf, the sum will also be the same Inf.
        // If both are Inf of the same sign, the sum will be the same Inf.
        // If they are Inf of different signs, the sum will be a NaN.
        return left + right;
    } else {
        // If only right is +/-Inf, this will also yield the same Inf.
        // This assumes that right_frac > 0 otherwise there wouldn't be any need for interpolation.
        return left + (right - left) * right_frac; 
    }
}

}

#endif

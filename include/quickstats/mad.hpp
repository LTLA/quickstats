#ifndef QUICKSTATS_MAD_HPP
#define QUICKSTATS_MAD_HPP

#include <cstddef>
#include <cmath>

#include "median.hpp"

/**
 * @file mad.hpp
 * @brief Compute the median absolute deviation.
 */

namespace quickstats {

/**
 * Compute the median absolute deviation (MAD) of an array of elements, given its median.
 *
 * No consideration is given to special values like NaNs in the array.
 * If these are to be skipped, consider using `skip_values()` before calling this function.
 *
 * See also `scale_mad_to_sd()` if the MAD is to be used as an estimate of the standard deviation.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of representing NaNs.
 * @tparam Input_ Numeric type of the input values and median.
 * This is generally expected to be floating-point, though it is also possible to use signed integers as their differences do not overflow.
 *
 * @param num_total Total number of elements from which to compute a MAD.
 * @param[in] ptr Pointer to an array of length `num_total`.
 * On output, the contents will contain the (possibly reordered) absolute deviations from the median.
 * @param median Median of the array at `ptr`, typically computed with `median()`.
 * For medians that might be infinite, consider using `mad_with_infinities()` instead.
 *
 * @return MAD of the array.
 * If `median` is not finite or if `num_total == 0`, NaN is returned.
 */
template<typename Output_ = double, typename Input_>
Output_ mad(const std::size_t num_total, Input_* const ptr, const Input_ median) {
    if (!std::isfinite(median)) {
        // Pre-emptively avoid shenanigans from trying to order NaN deviations.
        return std::numeric_limits<Output_>::quiet_NaN();
    }
    for (std::size_t i = 0; i < num_total; ++i) {
        ptr[i] = std::abs(ptr[i] - median);
    }
    return ::quickstats::median<Output_>(num_total, ptr);
}

/**
 * Compute the median absolute deviation (MAD) of a sparse vector, given its median.
 * This vector is assumed to have `num_non_zero` structural non-zeros and `num_total - num_non_zero` zeros.
 *
 * No consideration is given to special values like NaNs in the array.
 * If these are to be skipped, consider using `skip_values()` before calling this function.
 *
 * See also `scale_mad_to_sd()` if the MAD is to be used as an estimate of the standard deviation.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of representing NaNs.
 * @tparam Input_ Numeric type of the input values and median.
 * This is generally expected to be floating-point, though it is also possible to use signed integers as long as their differences do not overflow.
 *
 * @param num_total Total number of elements in the sparse vector.
 * @param num_non_zero Number of structural non-zeros in the sparse vector.
 * This should be no greater than `num_total`.
 * `num_total - num_non_zero` is the number of structural zeros.
 * @param[in] values Pointer to the start of an array of length `num_non_zero`, containing the values of the structural non-zeros of the sparse vector.
 * On output, the contents will contain the (possibly reordered) absolute deviations from the median.
 * @param median Median of the sparse vector at `values`, typically computed with `median()`.
 * For medians that might be infinite, consider using `mad_with_infinities()` instead.
 *
 * @return MAD of the sparse vector.
 * If `median` is not finite or if `num_total == 0`, NaN is returned.
 */
template<typename Output_ = double, typename Input_>
Output_ mad(const std::size_t num_total, const std::size_t num_non_zero, Input_* const values, const Input_ median) {
    if (!std::isfinite(median)) {
        // Pre-emptively avoid shenanigans from trying to order NaN deviations.
        return std::numeric_limits<Output_>::quiet_NaN();
    }

    // It is also possible to implement the sparse MAD by subtracting 'abs(median)' from the absolute deviations,
    // computing the sparse median of the difference, and then adding 'abs(median)' back to the result.
    // We don't do this as the subtraction and addition introduces some numerical error,
    // which isn't that consequential in practice but interferes with exact comparisons to the dense results in our tests.

    for (std::size_t i = 0; i < num_non_zero; ++i) {
        values[i] = std::abs(values[i] - median);
    }
    return median_internal<Output_>(num_total, num_non_zero, values, std::abs(median));
}

/**
 * Scale the median absolute deviation (MAD) so that its expected value for a normal distribution is equal to the standard deviation.
 *
 * @tparam Float_ Floating-point type of the MAD.
 *
 * @param x MAD, typically computed from `mad()`.
 *
 * @return Scaled value of `x`.
 */
template<typename Float_>
Float_ scale_mad_to_sd(const Float_ x) {
    return x * 1.4826;
}

/**
 * Variant of `mad()` to compute the median absolute deviation from an array of elements with an infinite median.
 * Specifically, we assume that all occurrences of infinity of the same sign refer to the same arbitrarily large number,
 * such that the deviation of infinity from a median of infinity is defined as zero (instead of NaN via usual IEEE arithmetic).
 * This ensures that the MAD will be well-defined for a non-empty array, i.e., either 0 or infinity.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of representing NaNs.
 * @tparam Input_ Floating-point type of the input values and median.
 *
 * @param num_total Total number of elements from which to compute a MAD.
 * @param[in] ptr Pointer to an array of length `num_total`.
 * On output, the contents will contain the (possibly reordered) absolute deviations from the median.
 * @param median Median of the array at `ptr`, typically computed with `median()`.
 * This may be infinite.
 *
 * @return MAD of the array, assuming that the difference between infinities of the same sign is zero.
 * If `median` is finite, this will be the same as `mad()`. 
 */
template<typename Output_ = double, typename Input_>
Output_ mad_with_infinities(const std::size_t num_total, Input_* const ptr, const Input_ median) {
    if (std::isinf(median)) {
        for (std::size_t i = 0; i < num_total; ++i) {
            ptr[i] = (median == ptr[i] ? 0 : std::numeric_limits<Input_>::infinity());
        }
        return ::quickstats::median<Output_>(num_total, ptr);
    } else {
        return mad(num_total, ptr, median);
    }
}

/**
 * Variant of `mad()` to compute the median absolute deviation from a sparse vector with an infinite median.
 * Specifically, we assume that all occurrences of infinity of the same sign refer to the same arbitrarily large number,
 * such that the deviation of infinity from a median of infinity is defined as zero (instead of NaN via usual IEEE arithmetic).
 * This ensures that the MAD will be well-defined for a non-empty array, i.e., either 0 or infinity.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of representing NaNs.
 * @tparam Input_ Floating-point type of the input values and median.
 *
 * @param num_total Total number of elements in the sparse vector.
 * @param num_non_zero Number of structural non-zeros in the sparse vector.
 * This should be no greater than `num_total`.
 * `num_total - num_non_zero` is the number of structural zeros.
 * @param[in] values Pointer to the start of an array of length `num_non_zero`, containing the values of the structural non-zeros of the sparse vector.
 * On output, the contents will contain the (possibly reordered) absolute deviations from the median.
 * @param median Median of the sparse vector at `values`, typically computed with `median()`.
 * This may be infinite.
 *
 * @return MAD of the sparse vector, assuming that the difference between infinities of the same sign is zero. 
 * If `median` is finite, this will be the same as `mad()`. 
 */
template<typename Output_ = double, typename Input_>
Output_ mad_with_infinities(const std::size_t num_total, const std::size_t num_non_zero, Input_* const values, const Input_ median) {
    if (std::isinf(median)) {
        for (std::size_t i = 0; i < num_non_zero; ++i) {
            values[i] = (median == values[i] ? 0 : std::numeric_limits<Input_>::infinity());
        }
        return median_internal<Output_>(num_total, num_non_zero, values, std::numeric_limits<Input_>::infinity());
    } else {
        return mad(num_total, num_non_zero, values, median);
    }
}

}

#endif

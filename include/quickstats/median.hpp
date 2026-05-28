#ifndef QUICKSTATS_MEDIAN_HPP
#define QUICKSTATS_MEDIAN_HPP

#include <algorithm>
#include <limits>
#include <type_traits>
#include <cassert>
#include <cstddef>

#include "utils.hpp"

/**
 * @file median.hpp
 * @brief Compute a median.
 */

namespace quickstats {

/**
 * Compute the median of an array of elements.
 *
 * No consideration is given to special values like NaNs in the array.
 * If these are to be skipped, consider using `skip_values()` before calling this function.
 *
 * @param num_total Total number of elements from which to compute a median.
 * @param[in] ptr Pointer to an array of length `num_total`.
 * On output, the contents may be reordered.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of representing NaNs.
 * @tparam Input_ Numeric type of the input values.
 *
 * @return Median of values in `[ptr, ptr + num_total)`, or NaN if `num_total == 0`.
 */
template<typename Output_, typename Input_>
Output_ median(const std::size_t num_total, Input_* const ptr) {
    static_assert(std::is_floating_point<Output_>::value);

    if (num_total == 0) {
        return std::numeric_limits<Output_>::quiet_NaN();
    }

    const std::size_t halfway = num_total / 2;
    const bool is_even = (num_total % 2 == 0);

    std::nth_element(ptr, ptr + halfway, ptr + num_total);
    const Output_ medtmp = *(ptr + halfway);
    if (!is_even) {
        return medtmp;
    }

    // 'nth_element()' reorganizes 'ptr' so that everything below 'halfway' is
    // less than or equal to 'ptr[halfway]', while everything above 'halfway'
    // is greater than or equal to 'ptr[halfway]'. Thus, to get the element
    // immediately before 'halfway' in the sort order, we just need to find the
    // maximum from '[0, halfway)'.
    const Output_ other = *std::max_element(ptr, ptr + halfway);

    return interpolate<Output_>(medtmp, other, 0.5);
}

/**
 * Compute the median of a sparse vector.
 * This vector is assumed to have `num_non_zero` structural non-zeros and `num_total - num_non_zero` zeros.
 *
 * No consideration is given to special values like NaNs in the array.
 * If these are to be skipped, consider using `skip_values()` before calling this function.
 *
 * @param num_total Total number of elements in the sparse vector.
 * @param num_non_zero Number of structural non-zeros in the sparse vector.
 * This should be no greater than `num_total`.
 * `num_total - num_non_zero` is the number of structural zeros.
 * @param[in] values Pointer to the start of an array of length `num_non_zero`, containing the values of the structural non-zeros of the sparse vector.
 * On output, the contents may be reordered.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 * @tparam Input_ Numeric type of the input values.
 *
 * @return Median of the sparse vector.
 */
template<typename Output_ = double, typename Input_>
Output_ median(const std::size_t num_total, const std::size_t num_non_zero, Input_* const values) {
    assert(num_total >= num_non_zero);

    // Fallback to the dense code if there are no structural zeros. This is not
    // just for efficiency as the downstream averaging code assumes that there
    // is at least one structural zero when considering its scenarios.
    if (num_non_zero == num_total) {
        return median<Output_>(num_total, values);
    }

    // Is the number of non-zeros less than the number of zeros?
    // If so, the median must be zero. Note that we calculate it
    // in this way to avoid overflow from 'num_non_zero * 2'.
    if (num_non_zero < num_total - num_non_zero) {
        return 0;
    } 

    const std::size_t halfway = num_total / 2;
    const bool is_even = (num_total % 2 == 0);

    const std::size_t num_zero = num_total - num_non_zero;
    std::size_t num_negative = 0;
    for (std::size_t i = 0; i < num_non_zero; ++i) {
        num_negative += (values[i] < 0);
    }

    if (!is_even) {
        if (num_negative > halfway) {
            std::nth_element(values, values + halfway, values + num_non_zero);
            return values[halfway];

        } else if (halfway >= num_negative + num_zero) {
            const std::size_t skip_zeros = halfway - num_zero;
            std::nth_element(values, values + skip_zeros, values + num_non_zero);
            return values[skip_zeros];

        } else {
            return 0;
        }
    }

    Output_ baseline = 0, other = 0;
    if (num_negative > halfway) { // both halves of the median are negative.
        std::nth_element(values, values + halfway, values + num_non_zero);
        baseline = values[halfway];
        other = *(std::max_element(values, values + halfway)); // max_element gets the sorted value at halfway - 1, see explanation for the dense case.

    } else if (num_negative == halfway) { // the upper half is guaranteed to be zero.
        const std::size_t below_halfway = halfway - 1;
        std::nth_element(values, values + below_halfway, values + num_non_zero);
        other = values[below_halfway]; // set to other so that addition/subtraction of a zero baseline has no effect on precision. 

    } else if (num_negative < halfway && num_negative + num_zero > halfway) { // both halves are zero, so zero is the median.
        ;

    } else if (num_negative + num_zero == halfway) { // the lower half is guaranteed to be zero.
        const std::size_t skip_zeros = halfway - num_zero;
        std::nth_element(values, values + skip_zeros, values + num_non_zero);
        other = values[skip_zeros]; // set to other so that addition/subtraction of a zero baseline has no effect on precision. 

    } else { // both halves of the median are non-negative.
        const std::size_t skip_zeros = halfway - num_zero;
        std::nth_element(values, values + skip_zeros, values + num_non_zero);
        baseline = values[skip_zeros];
        other = *(std::max_element(values, values + skip_zeros)); // max_element gets the sorted value at skip_zeros - 1, see explanation for the dense case.
    }

    return interpolate<Output_>(baseline, other, 0.5);
}

}

#endif

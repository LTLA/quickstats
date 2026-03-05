#ifndef QUICKSTATS_MEDIAN_HPP
#define QUICKSTATS_MEDIAN_HPP

#include <algorithm>
#include <limits>
#include <type_traits>
#include <cassert>

#include "utils.hpp"

/**
 * @brief median.hpp
 * @param Compute a median.
 */

namespace quickstats {

/**
 * Compute the median of an array of elements.
 *
 * @param num Number of elements from which to compute a median.
 * @param[in] ptr Pointer to an array of length `num`.
 * This should not contain any NaN values.
 * On output, the contents may be reordered. 
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of representing NaNs.
 * @tparam Input_ Numeric type of the input values.
 * @tparam Number_ Integer type of the number of elements.
 *
 * @return Median of values in `[ptr, ptr + num)`, or NaN if `num == 0`.
 */
template<typename Output_, typename Number_, typename Input_>
Output_ median(const Number_ num, Input_* const ptr) {
    static_assert(std::is_integral<Number_>::value);
    static_assert(std::is_floating_point<Output_>::value);

    if (num == 0) {
        return std::numeric_limits<Output_>::quiet_NaN();
    }

    const Number_ halfway = num / 2;
    const bool is_even = (num % 2 == 0);

    std::nth_element(ptr, ptr + halfway, ptr + num);
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
 * This vector is assumed to have `nnz` structural non-zeros and `num - nnz` zeros.
 *
 * @param num Total number of elements in the sparse vector.
 * @param nnz Number of structural non-zeros in the sparse vector.
 * This should be no greater than `num`.
 * `num - nnz` is the number of zeros.
 * @param[in] ptr Pointer to the start of an array of length `nnz`, containing the values of the structural non-zeros of the sparse vector.
 * It should not contain any NaN values.
 * On output, the contents may be reordered.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 * @tparam Input_ Numeric type of the input values.
 * @tparam Size_ Integer type of the row/column indices.
 *
 * @return Median of the sparse vector.
 */
template<typename Output_ = double, typename Number_, typename Input_>
Output_ median(const Number_ num, const Number_ nnz, Input_* const ptr) {
    assert(num >= nnz);

    // Fallback to the dense code if there are no structural zeros. This is not
    // just for efficiency as the downstream averaging code assumes that there
    // is at least one structural zero when considering its scenarios.
    if (nnz == num) {
        return median<Output_>(num, ptr);
    }

    // Is the number of non-zeros less than the number of zeros?
    // If so, the median must be zero. Note that we calculate it
    // in this way to avoid overflow from 'nnz * 2'.
    if (nnz < num - nnz) {
        return 0;
    } 

    const Number_ halfway = num / 2;
    const bool is_even = (num % 2 == 0);

    const Number_ num_zero = num - nnz;
    Number_ num_negative = 0;
    for (Number_ i = 0; i < nnz; ++i) {
        num_negative += (ptr[i] < 0);
    }

    if (!is_even) {
        if (num_negative > halfway) {
            std::nth_element(ptr, ptr + halfway, ptr + nnz);
            return ptr[halfway];

        } else if (halfway >= num_negative + num_zero) {
            const Number_ skip_zeros = halfway - num_zero;
            std::nth_element(ptr, ptr + skip_zeros, ptr + nnz);
            return ptr[skip_zeros];

        } else {
            return 0;
        }
    }

    Output_ baseline = 0, other = 0;
    if (num_negative > halfway) { // both halves of the median are negative.
        std::nth_element(ptr, ptr + halfway, ptr + nnz);
        baseline = ptr[halfway];
        other = *(std::max_element(ptr, ptr + halfway)); // max_element gets the sorted value at halfway - 1, see explanation for the dense case.

    } else if (num_negative == halfway) { // the upper half is guaranteed to be zero.
        const Number_ below_halfway = halfway - 1;
        std::nth_element(ptr, ptr + below_halfway, ptr + nnz);
        other = ptr[below_halfway]; // set to other so that addition/subtraction of a zero baseline has no effect on precision. 

    } else if (num_negative < halfway && num_negative + num_zero > halfway) { // both halves are zero, so zero is the median.
        ;

    } else if (num_negative + num_zero == halfway) { // the lower half is guaranteed to be zero.
        const Number_ skip_zeros = halfway - num_zero;
        std::nth_element(ptr, ptr + skip_zeros, ptr + nnz);
        other = ptr[skip_zeros]; // set to other so that addition/subtraction of a zero baseline has no effect on precision. 

    } else { // both halves of the median are non-negative.
        const Number_ skip_zeros = halfway - num_zero;
        std::nth_element(ptr, ptr + skip_zeros, ptr + nnz);
        baseline = ptr[skip_zeros];
        other = *(std::max_element(ptr, ptr + skip_zeros)); // max_element gets the sorted value at skip_zeros - 1, see explanation for the dense case.
    }

    return interpolate<Output_>(baseline, other, 0.5);
}

}

#endif

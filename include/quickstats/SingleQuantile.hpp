#ifndef QUICKSTATS_SINGLE_QUANTILE_HPP
#define QUICKSTATS_SINGLE_QUANTILE_HPP

#include <cmath>
#include <cassert>
#include <type_traits>
#include <stdexcept>
#include <optional>
#include <limits>

#include "sanisizer/sanisizer.hpp"

/**
 * @brief SingleQuantile.hpp
 * @param Compute a single quantile.
 */

namespace quickstats {

/**
 * @brief Calculate a single quantile from a fixed number of elements.
 *
 * @tparam Output_ Floating-point type of the output quantile.
 * @tparam Number_ Integer type of the number of elements.
 *
 * A `SingleQuantileFixedNumber` instance computes a type 7 quantile, consistent with the default in R's `quantile` function.
 */
template<typename Output_, typename Number_>
class SingleQuantileFixedNumber {
public:
    /**
     * @param num Number of elements from which to compute a quantile.
     * This should be positive.
     * @param quantile Probability of the quantile to compute, in \f$[0, 1]\f$.
     */
    SingleQuantileFixedNumber(const Number_ num, const Output_ quantile) : my_num(num) {
        static_assert(std::is_integral<Number_>::value);
        static_assert(std::is_floating_point<Output_>::value);

        if (num <= 0) {
            throw std::out_of_range("'num' should be positive");
        }
        if (quantile < 0 || quantile > 1) {
            throw std::out_of_range("'quantile' should lie in [0, 1]");
        }

        const Number_ num_m1 = num - 1;
        const Output_ raw = static_cast<Output_>(num_m1) * quantile;
        const Output_ upper_index = std::ceil(raw);
        const Output_ lower_index = std::floor(raw);

        // Protect the cast from conversion imprecision between size_t and Float_,
        // e.g., if it rounds up or if it converts it to an Inf.
        const auto upper_san = sanisizer::from_float<Number_>(upper_index);
        if (upper_san <= num_m1) {
            my_upper_index = upper_san;
            my_upper_fraction = raw - lower_index;
            my_skip_lower = (upper_index == lower_index);
        } else {
            // Just gently cap it at the maximum value.
            my_upper_index = num_m1;
            my_upper_fraction = 0;
            my_skip_lower = true;
        }
    }

private:
    Number_ my_num, my_upper_index;
    Output_ my_upper_fraction;
    bool my_skip_lower;

public:
    /**
     * @tparam Input_ Numeric type of the input values.
     *
     * @param[in] ptr Pointer to the start of an array of length `num`.
     * This should not contain any NaN values.
     * On output, the elements may be reordered.
     *
     * @return Quantile of the specified probability. 
     */
    template<typename Input_>
    Output_ operator()(Input_* ptr) const {
        const auto target = ptr + my_upper_index;
        std::nth_element(ptr, target, ptr + my_num);

        // Avoid an extra memory access to get the lower index if we don't need it; this would also fail if upper_index = 0.
        const Output_ upper = *target;
        if (my_skip_lower) {
            return upper;
        }

        const Output_ lower = *std::max_element(ptr, target); 
        return lower + (upper - lower) * my_upper_fraction;
    }

    /**
     * Overload to compute the desired quantile from a sparse vector of length `num`.
     * This vector is assumed to have `nnz` structural non-zeros and `num - nnz` zeros.
     *
     * @tparam Input_ Numeric type of the input values.
     *
     * @param nnz Number of structural non-zeros in the sparse vector.
     * This should be no greater than `num`.
     * @param[in] ptr Pointer to the start of an array of length `nnz`, containing the values of the structural non-zeros of the sparse vector.
     * It should not contain any NaN values.
     * On output, the elements may be reordered.
     *
     * @return Quantile of the specified probability.
     */
    template<typename Input_>
    Output_ operator()(const Number_ nnz, Input_* ptr) const {
        assert(nnz <= my_num);
        if (nnz == my_num) {
            return operator()(ptr);
        } else if (nnz == 0) {
            return 0;
        }

        Number_ nnz_negative = 0;
        for (Number_ i = 0; i < nnz; ++i) {
            nnz_negative += (ptr[i] < 0);
        }

        if (my_upper_index < nnz_negative) {
            const auto target = ptr + my_upper_index;
            std::nth_element(ptr, target, ptr + nnz);

            const Output_ upper = *target;
            if (my_skip_lower) {
                return upper;
            }

            const Output_ lower = *std::max_element(ptr, target); 
            return lower + (upper - lower) * my_upper_fraction;
        }

        if (nnz_negative && my_upper_index == nnz_negative) {
            // The upper value is zero. It can't be positive, as that would
            // imply that there are no structural zeros and nnz == my_num.
            if (my_skip_lower) {
                return 0;
            }

            const auto target = ptr + (my_upper_index - 1);
            std::nth_element(ptr, target, ptr + nnz);
            return static_cast<Output_>(*target) * (1 - my_upper_fraction);
        }

        const Number_ nnz_zeros = my_num - nnz;
        const Number_ nnz_not_positive = nnz_zeros + nnz_negative;
        if (my_upper_index < nnz_not_positive) {
            return 0;
        }

        const auto target = ptr + (my_upper_index - nnz_zeros);
        std::nth_element(ptr, target, ptr + nnz);
        const Output_ upper = *target;
        if (my_skip_lower) {
            return upper;
        }

        if (sanisizer::is_equal(my_upper_index, nnz_not_positive)) {
            return upper * my_upper_fraction;
        }

        const Output_ lower = *std::max_element(ptr, target); 
        return lower + (upper - lower) * my_upper_fraction;
    }
};

/**
 * @brief Calculate a quantile from a variable number of elements.
 *
 * @tparam Output_ Floating-point type of the output quantile.
 * This should be capable of representing NaNs.
 * @tparam Number_ Integer type of the number of elements.
 *
 * This uses the same logic as the `SingleQuantile` class but supports any number of elements, from zero up to a specified maximum.
 */
template<typename Output_, typename Number_>
class SingleQuantileVariableNumber {
public:
    /**
     * @param max_num Maximum number of elements. 
     * Unlike `SingleQuantile`, this may be zero.
     * @param quantile Probability of the quantile to compute, in \f$[0, 1]\f$.
     */
    SingleQuantileVariableNumber(Number_ max_num, const double quantile) : my_quantile(quantile) {
        if (max_num >= 2) {
            sanisizer::resize(my_choices, max_num - 1);
        }
    }

private:
    std::vector<std::optional<SingleQuantileFixedNumber<Output_, Number_> > > my_choices;
    double my_quantile;

public:
    /**
     * @tparam Input_ Numeric type of the input values.
     *
     * @param num Number of the elements from which to compute the quantile.
     * This should be no greater than `max_num`.
     * @param[in] ptr Pointer to an array of length `num`.
     * This should not contain NaN values.
     * On output, the elements may be reordered.
     *
     * @return Quantile of the specified probability. 
     * If `num = 0`, NaN is returned instead.
     */
    template<typename Input_>
    Output_ operator()(const Number_ num, Input_* ptr) {
        if (num == 0) {
            return std::numeric_limits<Output_>::quiet_NaN();
        } else if (num == 1) {
            return *ptr;
        } else {
            auto& ocalc = my_choices[num - 2];
            if (!ocalc.has_value()) { // Instantiating them on demand.
                ocalc.emplace(num, my_quantile);
            }
            return (*ocalc)(ptr);
        }
    }

    /**
     * Overload to compute the desired quantile from a sparse vector of length `num`.
     * This vector is assumed to have `nnz` structural non-zeros and `num - nnz` zeros.
     *
     * This method is not thread-safe.
     *
     * @tparam Input_ Numeric type of the input values.
     *
     * @param num Number of elements from which to compute the quantile.
     * This should be no greater than `max_num`.
     * @param nnz Number of elements that are structural non-zero elements.
     * This should be no greater than `num`.
     * @param[in] ptr Pointer to the start of an array of length `num`.
     * This is expected to contain the values of the structural non-zero elements of the sparse vector.
     * It should not contain any NaN values.
     * On output, the elements may be reordered.
     *
     * @return Quantile of the specified probability. 
     * If `num = 0`, NaN is returned instead.
     */
    template<typename Input_>
    Output_ operator()(const Number_ num, const Number_ nnz, Input_* ptr) {
        if (num == 0) {
            return std::numeric_limits<Output_>::quiet_NaN();
        } else if (num == 1) {
            return (nnz ? *ptr : 0);
        } else {
            auto& ocalc = my_choices[num - 2];
            if (!ocalc.has_value()) { // Instantiating them on demand.
                ocalc.emplace(num, my_quantile);
            }
            return (*ocalc)(nnz, ptr);
        }
    }
};

}

#endif

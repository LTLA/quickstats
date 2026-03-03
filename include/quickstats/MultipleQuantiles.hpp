#ifndef QUICKSTATS_MULTIPLE_QUANTILES_HPP
#define QUICKSTATS_MULTIPLE_QUANTILES_HPP

#include <type_traits>
#include <stdexcept>
#include <cstddef>
#include <vector>
#include <optional>
#include <limits>

#include "sanisizer/sanisizer.hpp"

#include "SingleQuantile.hpp"
#include "utils.hpp"

namespace quickstats {

/**
 * @brief Calculate multiple quantiles from a fixed number of elements.
 *
 * @tparam Output_ Floating-point type of the output quantiles.
 * @tparam Number_ Integer type of the number of elements.
 *
 * A `MultipleQuantilesFixedNumber` instance computes type 7 quantiles, consistent with the default in R's `quantile` function.
 * It is equivalent to but more efficient than multiple calls to different `SingleQuantileFixedNumber` instances.
 */
template<class Output_, typename Number_>
class MultipleQuantilesFixedNumber {
public:
    /**
     * @tparam Quantiles_ Container that has a `size()` method and supports access by `[]`.
     *
     * @param num_obs Number of elements from which to compute a quantile.
     * This should be positive.
     * @param quantiles Container of sorted probabilities of the quantiles to compute.
     * Each entry should be in \f$[0, 1]\f$.
     */
    template<typename Quantiles_>
    MultipleQuantilesFixedNumber(const Number_ num_obs, const Quantiles_& quantiles) :
        my_len(num_obs),
        my_stacks(sanisizer::cast<I<decltype(my_stacks.size())> >(quantiles.size()))
    {
        static_assert(std::is_integral<Number_>::value);
        static_assert(std::is_floating_point<Output_>::value);

        if (num_obs <= 0) {
            throw std::runtime_error("'num_obs' should be positive");
        }
        const Number_ num_m1 = num_obs - 1;

        const auto num_quantiles = quantiles.size();
        for (I<decltype(num_quantiles)> q = 0; q < num_quantiles; ++q) {
            const auto quantile = quantiles[q];
            if (q && quantile < quantiles[q - 1]) {
                throw std::runtime_error("elements of 'quantiles' should be sorted");
            }

            auto& current = my_stacks[q]; 
            configure_single_quantile<Output_, Number_>(quantile, num_m1, current.upper_index, current.upper_fraction, current.skip_lower);
        }
    }

private:
    Number_ my_len;

    struct Configuration { 
        Number_ upper_index;
        Output_ upper_fraction;
        bool skip_lower;
    };

    std::vector<Configuration> my_stacks;

public:
    /**
     * @tparam Input_ Numeric type of the input values.
     * @tparam OutputFun_ Functor that accepts a `Number_` and an `Output_`.
     *
     * @param[in] ptr Pointer to the start of an array of length `num`.
     * This should not contain any NaN values.
     * On output, the elements may be reordered.
     * @param output Function that accepts a `Number_`, the index of the probability in `quantiles`;
     * and an `Output_`, the computed value of the quantile.
     * This will be called once for each quantile, in order of increasing index from 0 to `quantiles.size() - 1`.
     */
    template<typename Input_, class OutputFun_>
    void operator()(Input_* const ptr, OutputFun_ output) const {
        const auto end = ptr + my_len;
        const auto num_quantiles = my_stacks.size();

        Number_ last_index;
        Output_ lower_val;

        for (I<decltype(num_quantiles)> q = 0; q < num_quantiles; ++q) {
            const auto& curstack = my_stacks[q];
            const auto curindex = curstack.upper_index;

            const auto target = ptr + curindex;
            if (q == 0) {
                std::nth_element(ptr, target, end);
            } else if (curindex > last_index) {
                // +1, as we only need to search after the last_index; everything before or at last_index is known to be too small.
                std::nth_element(ptr + last_index + 1, target, end);
            }

            const Output_ upper_val = *target;
            if (curstack.skip_lower) {
                output(q, upper_val);
            } else {
                if (q == 0) {
                    lower_val = *std::max_element(ptr, target);
                } else if (curindex > last_index) { 
                    // No + 1, as ptr[last_index] might be the maximum.
                    lower_val = *std::max_element(ptr + last_index, target);
                }
                output(q, lower_val + (upper_val - lower_val) * curstack.upper_fraction);
            }

            last_index = curindex;
        }
    }

    /**
     * Overload to compute the desired quantile from a sparse vector of length `num`.
     * This vector is assumed to have `nnz` structural non-zeros and `num - nnz` zeros.
     *
     * @tparam Input_ Numeric type of the input values.
     * @tparam OutputFun_ Functor that accepts a `Number_` and an `Output_`.
     *
     * @param nnz Number of structural non-zeros in the sparse vector.
     * This should be no greater than `num`.
     * @param[in] ptr Pointer to the start of an array of length `nnz`, containing the values of the structural non-zeros of the sparse vector.
     * It should not contain any NaN values.
     * On output, the elements may be reordered.
     * @param output Function that accepts a `Number_`, the index of the probability in `quantiles`;
     * and an `Output_`, the computed value of the quantile.
     * This will be called once for each quantile, in order of increasing index from 0 to `quantiles.size() - 1`.
     */
    template<typename Input_, class OutputFun_>
    void operator()(const Number_ nnz, Input_* const ptr, OutputFun_ output) const {
        assert(nnz <= my_len);
        const auto num_quantiles = my_stacks.size();

        if (nnz == my_len) {
            operator()(ptr, std::move(output));
            return;
        } else if (nnz == 0) {
            for (I<decltype(num_quantiles)> q = 0; q < num_quantiles; ++q) {
                output(q, 0);
            }
            return; 
        }

        Number_ num_negative = 0;
        for (Number_ i = 0; i < nnz; ++i) {
            num_negative += (ptr[i] < 0);
        }

        // We need to use an optional here, as last_index may not be initialized for q > 0, e.g., if many quantiles are zero.
        std::optional<Number_> last_index;
        Output_ lower_val;
        I<decltype(num_quantiles)> q = 0;

        // Grind through all of the quantiles corresponding to negative values.
        for (; q < num_quantiles; ++q) {
            const auto& curstack = my_stacks[q];
            const auto curindex = curstack.upper_index;
            if (curindex >= num_negative) {
                break;
            }

            const auto target = ptr + curindex;
            if (!last_index.has_value()) {
                std::nth_element(ptr, target, ptr + nnz);
            } else if (curindex > *last_index) {
                std::nth_element(ptr + *last_index + 1, target, ptr + nnz);
            }

            const Output_ upper_val = *target;
            if (curstack.skip_lower) {
                output(q, upper_val);
            } else {
                if (!last_index.has_value()) {
                    lower_val = *std::max_element(ptr, target); 
                } else if (curindex > *last_index) {
                    lower_val = *std::max_element(ptr + *last_index, target); 
                }
                output(q, lower_val + (upper_val - lower_val) * curstack.upper_fraction);
            }

            last_index = curindex;
        }

        if (num_negative) {
            std::optional<Output_> upper_val;

            for (; q < num_quantiles; ++q) {
                const auto& curstack = my_stacks[q];
                const auto curindex = curstack.upper_index;
                if (curindex > num_negative) {
                    break;
                }

                if (curstack.skip_lower) {
                    // Upper value must be zero.
                    // It can't be positive, as that would imply that there are no structural zeros and nnz == my_len.
                    output(q, 0);

                } else {
                    if (!upper_val.has_value()) { // Only need to find it once given that everyone in this loop has 'upper_index == num_negative'.
                        const auto num_negative_m1 = num_negative - 1;
                        const auto target = ptr + num_negative_m1;

                        if (!last_index.has_value()) {
                            std::nth_element(ptr, target, ptr + nnz);
                        } else if (num_negative_m1 > *last_index) {
                            // Don't add 1, as we only know that last_index < num_negative from the break condition in the previous loop.
                            // It could be possible that last_index == num_negative - 1, in which case adding 1 would cause the start to go past 'target'.
                            std::nth_element(ptr + *last_index, target, ptr + nnz);
                        }

                        last_index = num_negative_m1;
                        upper_val = *target;
                    }

                    output(q, *upper_val * (1 - curstack.upper_fraction));
                }
            }
        }

        // Now we grind through all of the quantiles corresponding to zero values.
        const Number_ num_zeros = my_len - nnz;
        const Number_ num_not_positive = num_zeros + num_negative;
        for (; q < num_quantiles; ++q) {
            if (my_stacks[q].upper_index >= num_not_positive) {
                break;
            }
            output(q, 0);
        }

        // Finally, we grind through the quantiles corresponding to positive values.
        for (; q < num_quantiles; ++q) {
            const auto& curstack = my_stacks[q];
            const Number_ curindex = curstack.upper_index - num_zeros;

            const auto target = ptr + curindex;
            if (!last_index.has_value()) {
                std::nth_element(ptr, target, ptr + nnz);
            } else if (curindex > *last_index) {
                // Even with subtraction of num_zeros, we can be sure that curindex > last_index at the start of this loop.
                // This is because last_index must be pointing to a negative value if q > 0 at the start of this loop.
                std::nth_element(ptr + *last_index + 1, target, ptr + nnz);
            }

            const Output_ upper_val = *target;
            if (curstack.skip_lower) {
                output(q, upper_val);
            } else if (curindex == num_negative) {
                output(q, upper_val * curstack.upper_fraction);
            } else {
                if (!last_index.has_value()) {
                    lower_val = *std::max_element(ptr, target); 
                } else if (curindex > *last_index) {
                    lower_val = *std::max_element(ptr + *last_index, target); 
                }
                output(q, lower_val + (upper_val - lower_val) * curstack.upper_fraction);
            }

            last_index = curindex;
        }
    }
};

/**
 * @brief Calculate multiple quantiles from a variable number of elements.
 *
 * @tparam Output_ Floating-point type of the output quantile.
 * This should be capable of representing NaNs.
 * @tparam Number_ Integer type of the number of elements.
 * @tparam QuantilesPointer_ Pointer to a container that has a `size()` method and supports access by `[]`.
 * This may or may not be a smart pointer.
 *
 * This uses the same logic as the `MultipleQuantilesFixedNumber` class but supports any number of elements, from zero up to a specified maximum.
 */
template<typename Output_, typename Number_, class QuantilesPointer_>
class MultipleQuantilesVariableNumber {
public:
    /**
     * @param max_num Maximum number of elements. 
     * Unlike `MultipleQuantilesFixedNumber`, this may be zero.
     * @param quantiles_ptr Pointer to a container of sorted quantile probabilities.
     * Each probability should be in \f$[0, 1]\f$.
     */
    MultipleQuantilesVariableNumber(const Number_ max_num, QuantilesPointer_ quantiles_ptr) : my_quantiles_ptr(std::move(quantiles_ptr)) {
        if (max_num >= 2) {
            sanisizer::resize(my_choices, max_num - 1);
        }
    }

private:
    std::vector<std::optional<MultipleQuantilesFixedNumber<Output_, Number_> > > my_choices;
    QuantilesPointer_ my_quantiles_ptr;

    template<class OutputFun_>
    void fill(const Output_ val, OutputFun_ output) {
        const auto num_quantiles = my_quantiles_ptr->size();
        for (I<decltype(num_quantiles)> q = 0; q < num_quantiles; ++q) {
            output(q, val);
        }
    }

public:
    /**
     * This method is not thread-safe.
     *
     * @tparam Input_ Numeric type of the input values.
     * @tparam OutputFun_ Functor that accepts a `Number_` and an `Output_`.
     *
     * @param num Number of the elements from which to compute the quantile.
     * This should be no greater than `max_num`.
     * @param[in] ptr Pointer to an array of length `num`.
     * This should not contain NaN values.
     * On output, the elements may be reordered.
     * @param output Function that accepts a `Number_`, the index of the probability in `quantiles`;
     * and an `Output_`, the computed value of the quantile.
     * This will be called once for each quantile, in order of increasing index from 0 to `quantiles.size() - 1`.
     */
    template<typename Input_, class OutputFun_>
    void operator()(const Number_ num, Input_* ptr, OutputFun_ output) {
        if (num == 0) {
            fill(std::numeric_limits<Output_>::quiet_NaN(), std::move(output));
        } else if (num == 1) {
            fill(*ptr, std::move(output));
        } else {
            auto& ocalc = my_choices[num - 2];
            if (!ocalc.has_value()) { // Instantiating them on demand.
                ocalc.emplace(num, *my_quantiles_ptr);
            }
            (*ocalc)(ptr, std::move(output));
        }
    }

    /**
     * Overload to compute the desired quantile from a sparse vector of length `num`.
     * This vector is assumed to have `nnz` structural non-zeros and `num - nnz` zeros.
     *
     * This method is not thread-safe.
     *
     * @tparam Input_ Numeric type of the input values.
     * @tparam OutputFun_ Functor that accepts a `Number_` and an `Output_`.
     *
     * @param num Number of elements from which to compute the quantile.
     * This should be no greater than `max_num`.
     * @param nnz Number of elements that are structural non-zero elements.
     * This should be no greater than `num`.
     * @param[in] ptr Pointer to the start of an array of length `num`.
     * This is expected to contain the values of the structural non-zero elements of the sparse vector.
     * It should not contain any NaN values.
     * On output, the elements may be reordered.
     * @param output Function that accepts a `Number_`, the index of the probability in `quantiles`;
     * and an `Output_`, the computed value of the quantile.
     * This will be called once for each quantile, in order of increasing index from 0 to `quantiles.size() - 1`.
     */
    template<typename Input_, class OutputFun_>
    void operator()(const Number_ num, const Number_ nnz, Input_* ptr, OutputFun_ output) {
        if (num == 0) {
            fill(std::numeric_limits<Output_>::quiet_NaN(), std::move(output));
        } else if (num == 1) {
            fill(nnz ? *ptr : 0, std::move(output));
        } else {
            auto& ocalc = my_choices[num - 2];
            if (!ocalc.has_value()) { // Instantiating them on demand.
                ocalc.emplace(num, *my_quantiles_ptr);
            }
            (*ocalc)(nnz, ptr, std::move(output));
        }
    }
};

}

#endif

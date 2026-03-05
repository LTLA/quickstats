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
        Output_ last_quantile = -1;

        for (I<decltype(num_quantiles)> q = 0; q < num_quantiles; ++q) {
            const Output_ quantile = quantiles[q];
            auto& current = my_stacks[q]; 
            configure_single_quantile(quantile, num_m1, current.upper_index, current.upper_fraction, current.skip_lower);

            if (quantile < last_quantile) {
                throw std::runtime_error("elements of 'quantiles' should be sorted");
            }
            last_quantile = quantile;
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

        Number_ last_index = 0;
        Number_ last_index_p1 = 0; // yes, this is a deliberate starting value for 'last_index + 1'.
        Output_ lower_val, upper_val;

        for (I<decltype(num_quantiles)> q = 0; q < num_quantiles; ++q) {
            const auto& curstack = my_stacks[q];
            const auto curindex = curstack.upper_index;

            // Only need to search if 'cur_index > last_index' (i.e., 'cur_index >= last_index_p1'),
            // otherwise 'cur_index == last_index' and we can re-use existing lower/upper_val. 
            // The exception is if at the first iteration of this loop where 'last_index_p1 == 0 <= curindex',
            // where this condition is always true to force a search to initialize 'upper_val'.
            if (curindex >= last_index_p1) {
                const auto target = ptr + curindex;

                // +1, as we only need to search after the 'last_index'; everything before or at 'last_index' is known to be too small.
                // Again, the exception is at the first iteration of this loop, where we need to search from the start of the array to initialize 'upper_val'.
                std::nth_element(ptr + last_index_p1, target, end);
                upper_val = *target;

                if (curstack.skip_lower) {
                    output(q, upper_val);
                } else {
                    // No + 1, as 'ptr[last_index]' might be the maximum.
                    lower_val = *std::max_element(ptr + last_index, target);
                    output(q, interpolate(lower_val, upper_val, curstack.upper_fraction));
                }

                last_index = curindex;
                last_index_p1 = curindex + 1;

            } else {
                if (curstack.skip_lower) {
                    output(q, upper_val);
                } else {
                    // It is implicitly guaranteed that 'lower_val' is initialized at this point.
                    // It's impossible for an earlier quantile to have 'skip_lower = true' while a later quantile has 'skip_lower = false'.
                    output(q, interpolate(lower_val, upper_val, curstack.upper_fraction));
                }
            }
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

        Number_ last_index = 0;
        Number_ last_index_p1 = 0; // see comments for the dense case for an explanation.
        I<decltype(num_quantiles)> q = 0;

        // Processing all quantiles where both upper and lower values are negative.
        { 
            Output_ lower_val, upper_val;
            for (; q < num_quantiles; ++q) {
                const auto& curstack = my_stacks[q];
                const auto curindex = curstack.upper_index;
                if (curindex >= num_negative) {
                    break;
                }

                if (curindex >= last_index_p1) {
                    const auto target = ptr + curindex;
                    std::nth_element(ptr + last_index_p1, target, ptr + nnz);
                    upper_val = *target;

                    if (curstack.skip_lower) {
                        output(q, upper_val);
                    } else {
                        lower_val = *std::max_element(ptr + last_index, target); 
                        output(q, interpolate(lower_val, upper_val, curstack.upper_fraction));
                    }

                    last_index = curindex;
                    last_index_p1 = curindex + 1;

                } else {
                    if (curstack.skip_lower) {
                        output(q, upper_val);
                    } else {
                        output(q, interpolate(lower_val, upper_val, curstack.upper_fraction));
                    }
                }
            }
        }

        // Processing all quantiles where lower value is negative and upper value is zero.
        if (num_negative) {
            bool computed = false;
            Output_ lower_val;
            for (; q < num_quantiles; ++q) {
                const auto& curstack = my_stacks[q];
                const auto curindex = curstack.upper_index;
                if (curindex > num_negative) {
                    break;
                }

                if (curstack.skip_lower) {
                    // Upper value must be zero.
                    // It can't be positive, as that would imply that there are no structural zeros and 'nnz == my_len'.
                    output(q, 0);
                    continue;
                }

                if (!computed) { // Only need to search this once given that everyone in this loop has 'upper_index == num_negative'.
                    const auto num_negative_m1 = num_negative - 1;
                    const auto target = ptr + num_negative_m1;

                    // We know that 'curindex >= num_negative' from the exit condition in the previous loop.
                    // This implies that any previous loop iterations would have 'curindex < num_negative' such that 'last_index + 1 <= num_negative'.
                    // If 'last_index + 1 == num_negative', then 'last_index == num_negative - 1', and no search is required.
                    // So, we only search if 'last_index + 1 < num_negative', i.e., 'last_index + 1 <= num_negative - 1'.
                    // (If there were no runs of the previous loop, then 'last_index + 1 == 0' and we force a search anyway.)
                    if (num_negative_m1 >= last_index_p1) {
                        std::nth_element(ptr + last_index_p1, target, ptr + nnz);
                    }
                    lower_val = *target;

                    last_index = num_negative_m1;
                    last_index_p1 = num_negative;
                    computed = true;
                }

                output(q, lower_val * (1 - curstack.upper_fraction));
            }
        }

        // Processing all quantiles where both the lower and upper values are zero.
        const Number_ num_zeros = my_len - nnz;
        const Number_ num_not_positive = num_zeros + num_negative;
        for (; q < num_quantiles; ++q) {
            if (my_stacks[q].upper_index >= num_not_positive) {
                break;
            }
            output(q, 0);
        }

        // Processing all quantiles where the lower value is zero and the upper value is positive.
        {
            bool computed = false;
            Output_ upper_val; 
            for (; q < num_quantiles; ++q) {
                const auto& curstack = my_stacks[q];
                if (curstack.upper_index > num_not_positive) {
                    break;
                }

                if (!computed) { // Only need to search this once given that everyone in this loop has 'upper_index == num_not_positive'.
                    // The actual target is the first positive value at 'ptr[num_not_positive - num_zeros]', i.e., 'ptr[num_negative]'.
                    const auto target = ptr + num_negative;

                    // No point wrapping this in 'num_negative >= last_index + 1', as this will always be true from the exit conditions above.
                    // Or if none of the loops above were iterated over, we would still have 'last_index_p1 == 0', in which case this will also be true.
                    std::nth_element(ptr + last_index_p1, target, ptr + nnz);

                    last_index = num_negative;
                    last_index_p1 = num_negative + 1;
                    upper_val = *target;
                    computed = true;
                }

                if (curstack.skip_lower) {
                    output(q, upper_val);
                } else {
                    output(q, upper_val * curstack.upper_fraction);
                }
            }
        }

        // Processing all quantiles where the upper and lower values are positive.
        {
            Output_ upper_val, lower_val;
            for (; q < num_quantiles; ++q) {
                const auto& curstack = my_stacks[q];

                // This should always be positive as we know that 'upper_index > num_not_positive' and thus 'upper_index - num_zeros > num_negative >= 0'.
                const Number_ curindex = curstack.upper_index - num_zeros;

                // Even with subtraction of num_zeros, we can be sure that 'curindex >= last_index + 1' in the first iteration of this loop.
                // From the clauses above, we know that 'last_index <= num_negative' and 'upper_index > num_not_positive'.
                // So, subtracting 'num_zeros' gives us 'curindex > last_index' and thus 'curindex >= last_index + 1'.
                // This ensures that this condition will always be true, and the code will be run, and 'upper_val' will always be set before use.
                if (curindex >= last_index_p1) {
                    const auto target = ptr + curindex;
                    std::nth_element(ptr + last_index_p1, target, ptr + nnz);
                    upper_val = *target;

                    if (curstack.skip_lower) {
                        output(q, upper_val);
                    } else {
                        lower_val = *std::max_element(ptr + last_index, target); 
                        output(q, interpolate(lower_val, upper_val, curstack.upper_fraction));
                    }

                    last_index = curindex;
                    last_index_p1 = curindex + 1;

                } else {
                    if (curstack.skip_lower) {
                        output(q, upper_val);
                    } else {
                        output(q, interpolate(lower_val, upper_val, curstack.upper_fraction));
                    }
                }
            }
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

#ifndef QUICKSTATS_PAIRWISE_SUM_HPP
#define QUICKSTATS_PAIRWISE_SUM_HPP

#include <vector>
#include <cstddef>
#include <optional>

/**
 * @file pairwise_sum.hpp
 * @brief Pairwise summation.
 */

namespace quickstats {

/**
 * @brief Re-usable workspace for `pairwise_sum()`.
 *
 * @tparam Output_ Numeric type of the sum.
 */
template<typename Output_>
struct PairwiseSumWorkspace {
    /**
     * @cond
     */
    struct State {
        State(const std::size_t left_end) : left_end(left_end) {}
        std::size_t left_end;
        std::optional<Output_> left_sum;
    };
    std::vector<State> states;
    /**
     * @endcond
     */
};

/**
 * Perform pairwise summation on an array of numeric elements.
 * The array is recursively halved until each subarray is no greater than `limit_`.
 * Elements in each subarray are summed in sequence, and the subtotals are then added together to obtain the total sum.
 * Compared to naive summation, this reduces round-off error from floating-point imprecision with minimal loss of performance. 
 *
 * @tparam limit_ Maximum number of elements to sum directly.
 * @tparam Input_ Numeric type of the input data.
 * @tparam Modifier_ Function to apply to each element of the input data to modify it.
 * @tparam Output_ Numeric type of the sum.
 *
 * @param num_total Total number of observations.
 * @param[in] ptr Pointer to an array of length `num_total`, containing the elements to be summed.
 * @param mod Function that accepts an index into `ptr` (as a `std::size_t`) and the value of the element (as an `Input_`) and returns an `Output_`.
 * The return value will be used in the summation.
 * @param work Workspace that can be re-used across multiple `pairwise_sum()` calls.
 *
 * @return Sum of all (modified) elements in `[ptr, ptr + num_total)`.
 */
template<std::size_t limit_ = 128, typename Input_, class Modifier_, typename Output_>
Output_ pairwise_sum(const std::size_t num_total, const Input_* const ptr, Modifier_ mod, PairwiseSumWorkspace<Output_>& work) {
    work.states.clear();

    std::size_t start = 0, end = num_total;
    Output_ out = 0;
    while (1) {
        const std::size_t len = end - start;
        if (len > limit_) {
            work.states.emplace_back(end);
            end = start + len / 2;
            continue;
        }

        Output_ tmp = 0;
        for (std::size_t i = start; i < end; ++i) {
            tmp += mod(i, ptr[i]);
        }

        start = end;
        while (work.states.size() && work.states.back().left_sum.has_value()) {
            tmp += *(work.states.back().left_sum);
            start = work.states.back().left_end;
            work.states.pop_back();
        }

        if (work.states.empty()) {
            out = tmp;
            break;
        }

        work.states.back().left_sum = tmp;
        end = work.states.back().left_end;
    }

    return out;
}

/**
 * Overload of `pairwise_sum()` to conveniently compute the sum of an array without any modification of its elements.
 *
 * @tparam limit_ Maximum number of elements to sum directly.
 * @tparam Input_ Numeric type of the input data.
 * @tparam Output_ Numeric type of the sum.
 *
 * @param num_total Total number of observations.
 * @param[in] ptr Pointer to an array of length `num_total`, containing the elements to be summed.
 * @param work Workspace that can be re-used across multiple `pairwise_sum()` calls.
 *
 * @return Sum of all elements in `[ptr, ptr + num_total)`.
 */
template<std::size_t limit_ = 128, typename Input_, typename Output_>
Output_ pairwise_sum(const std::size_t num_total, const Input_* const ptr, PairwiseSumWorkspace<Output_>& work) {
    return pairwise_sum<limit_>(
        num_total,
        ptr,
        [&](std::size_t, const Input_ val) -> Output_ {
            return val;
        },
        work
    );
}

}

#endif

#ifndef QUICKSTATS_RSS_HPP
#define QUICKSTATS_RSS_HPP

#include <cassert>
#include <limits>
#include <cstddef>
#include <algorithm>

#include "sanisizer/sanisizer.hpp"

#include "utils.hpp"
#include "pairwise_sum.hpp"

/**
 * @file rss.hpp
 * @brief Compute residual sum of squares.
 */

namespace quickstats {

/** 
 * @brief Result of `rss()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Output_ = double>
struct RssResult { 
    /**
     * Sample mean.
     * This is set to NaN if there are no input values.
     */
    Output_ mean = 0;

    /**
     * Residual sum of squares from the mean.
     * This is set to 0 if there are no input values.
     */
    Output_ rss = 0;
};


/** 
 * @brief Re-usable workspace for `rss()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Output_ = double>
struct RssWorkspace {
    /**
     * @cond
     */
    PairwiseSumWorkspace<Output_> pswork;
    /**
     * @endcond
     */
};

/**
 * Compute the residual sum of squares from a sparse vector.
 * This uses the standard two-pass algorithm with naive accumulation of the sum of squared differences;
 * thus, it is best used with a sufficiently high-precision `Output_` like `double`.
 *
 * No consideration is given to special values like NaNs in the values of the structural non-zeros.
 * If these are to be skipped, consider using `skip_values()` before calling this method.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 * @tparam limit_ Maximum number of elements to sum directly, see `pairwise_sum()` for details.
 * @tparam Input_ Numeric type of the input values.
 *
 * @param num_total Total number of elements in the sparse vector.
 * @param num_non_zero Number of structural non-zeros in the sparse vector.
 * This should be no greater than `num_total`.
 * `num_total - num_non_zero` is the number of structural zeros.
 * @param[in] ptr Pointer to an array of length `num_non_zero`, containing the values of the structural non-zeros in the sparse vector.
 * @param work Workspace that can be re-used across multiple `rss()` calls.
 *
 * @return The sample mean and residual sum of squares of the sparse vector.
 */
template<typename Output_ = double, std::size_t limit_ = 128, typename Input_>
RssResult<Output_> rss(const std::size_t num_total, const std::size_t num_non_zero, const Input_* const ptr, RssWorkspace<Output_>& work) {
    static_assert(std::is_floating_point<Output_>::value);

    RssResult<Output_> output;
    if (num_total == 0) {
        output.mean = std::numeric_limits<Output_>::quiet_NaN();
        return output;
    }

    Output_& mean = output.mean;
    mean = pairwise_sum<limit_>(num_non_zero, ptr, work.pswork);
    mean /= num_total;

    Output_& ssd = output.rss;
    ssd = pairwise_sum<limit_>(
        num_non_zero,
        ptr, 
        [&](std::size_t, const Input_ val) -> Output_ {
            const auto delta = static_cast<Output_>(val) - mean;
            return delta * delta;
        },
        work.pswork
    );

    assert(num_non_zero <= num_total);
    if (num_non_zero < num_total) {
        ssd += static_cast<Output_>(num_total - num_non_zero) * mean * mean;
    }

    return output;
}

/**
 * Compute the residual sum of squares from the mean from a dense array.
 * This uses the standard two-pass algorithm with naive accumulation of the sum of squared differences;
 * thus, it is best used with a sufficiently high-precision `Output_` like `double`.
 *
 * No consideration is given to special values like NaNs in the dense array.
 * If these are to be skipped, consider using `skip_values()` before calling this method.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 * @tparam Input_ Numeric type of the input values.
 *
 * @param num_total Total number of elements in the array.
 * @param[in] ptr Pointer to an array of length `num_total`.
 * @param work Workspace that can be re-used across multiple `rss()` calls.
 *
 * @return The sample mean and residual sum of squares of the array.
 */
template<typename Output_ = double, typename Input_>
RssResult<Output_> rss(const std::size_t num_total, const Input_* const ptr, RssWorkspace<Output_>& work) {
    return rss(num_total, num_total, ptr, work);
}

/**
 * @cond
 */
template<typename Output_ = double, typename Value_, typename Count_>
void welford_add(Output_& mean, Output_& sumsq, const Value_ value, const Count_ count) {
    Output_ delta = static_cast<Output_>(value) - mean;
    mean += delta / count;
    sumsq += delta * (static_cast<Output_>(value) - mean);
}
/**
 * @endcond
 */

/**
 * @brief Running residual sum of squares from dense data.
 *
 * Compute running means and residual sum of squares (RSS) from dense data using Welford's method.
 *
 * Consider a scenario with a set of equilength "objective" vectors \f$[v_1, v_2, v_3, ..., v_n]\f$.
 * We wish to compute the RSS for each objective vector but our data is organized into "observed" vectors \f$[p_1, p_2, p_3, ..., p_m]\f$,
 * where the \f$j\f$-th element of \f$p_i\f$ is the \f$i\f$-th element of \f$v_j\f$, i.e., the layout is transposed.
 *
 * The idea is to repeatedly call `add()` on each observed vector from 1 to \f$m\f$, followed by a call to `finish()`.
 * `mean[i]` and `rss[i]` will subsequently be filled with the mean and RSS, respectively, for the objective vector `i`.
 *
 * @tparam Input_ Numeric type of the input data.
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Input_, typename Output_ = double>
class RssRunningDense {
public:
    /**
     * @param num_obj Number of objective vectors, i.e., \f$n\f$.
     * @param[out] mean Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     * @param[out] rss Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     */
    RssRunningDense(const std::size_t num_obj, Output_* const mean, Output_* const rss) :
        my_num_obj(num_obj),
        my_mean(mean),
        my_rss(rss)
    {
        assert(check_zeroed(num_obj, mean));
        assert(check_zeroed(num_obj, rss));
    }

public:
    /**
     * Add the next observed vector to the variance calculation.
     *
     * No consideration is given to special values like NaNs in the vector.
     * If these are to be skipped, consider using `RssRunningDenseSkip` instead.
     *
     * @param[in] ptr Pointer to an array of values of length `num_obj`, corresponding to an observed vector.
     */
    void add(const Input_* const ptr) {
        my_count = sanisizer::sum<std::size_t>(my_count, 1);
        for (std::size_t i = 0; i < my_num_obj; ++i) {
            welford_add(my_mean[i], my_rss[i], ptr[i], my_count);
        }
    }

    /**
     * Finalize the statistics.
     * This should only be called after all calls to `add()`.
     */
    void finish() {
        if (my_count == 0) {
            std::fill_n(my_mean, my_num_obj, std::numeric_limits<Output_>::quiet_NaN());
        }
    }

private:
    std::size_t my_num_obj;
    Output_* my_mean;
    Output_* my_rss;
    std::size_t my_count = 0;

    static_assert(std::is_floating_point<Output_>::value);
};

/**
 * @brief Running residual sum of squares from dense data, with skipping.
 *
 * This is a variant of `RssRunningDense` that provides an option to skip elements of each observed vector, e.g., NaNs.
 * After the `finish()` call, `mean[i]` and `rss[i]` will contain the mean and RSS for the unskipped elements of objective vector `i`,
 * while `num_unskipped[i]` will contain the number of unskipped elements. 
 *
 * @tparam Count_ Integer type of the number of elements.
 * @tparam Input_ Numeric type of the input data.
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Count_, typename Input_, typename Output_ = double>
class RssRunningDenseSkip {
public:
    /**
     * @param num_obj Number of objective vectors, i.e., \f$n\f$.
     * @param[out] mean Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     * @param[out] rss Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     * @param[out] num_unskipped Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     */
    RssRunningDenseSkip(const std::size_t num_obj, Output_* mean, Output_* rss, Count_* num_unskipped) :
        my_num_obj(num_obj),
        my_mean(mean),
        my_rss(rss),
        my_num_unskipped(num_unskipped)
    {
        assert(check_zeroed(num_obj, mean));
        assert(check_zeroed(num_obj, rss));
        assert(check_zeroed(num_obj, num_unskipped));
    }

public:
    /**
     * Add the next observed vector to the variance calculation.
     *
     * @tparam Skip_ Function that indicates whether to skip a particular element. 
     *
     * @param[in] ptr Pointer to an array of values of length `num_obj`, corresponding to an observed vector.
     * @param skip Function that accepts the index of an objective vector `i` (as a `std::size_t`) and its value `ptr[i]` (as a `Input_`),
     * and returns a boolean indicating whether to skip this element.
     */
    template<class Skip_>
    void add(const Input_* ptr, Skip_ skip) {
        // my_count is the upper bound of all my_num_unskipped, so we check it once here to avoid having to check it in the loop.
        my_count = sanisizer::sum<Count_>(my_count, 1);

        for (std::size_t i = 0; i < my_num_obj; ++i) {
            const auto val = ptr[i];
            if (!skip(i, val)) {
                welford_add(my_mean[i], my_rss[i], val, ++(my_num_unskipped[i]));
            }
        }
    }

    /**
     * Finalize the statistics.
     * This should only be called after all calls to `add()`.
     */
    void finish() {
        if (my_count == 0) {
            std::fill_n(my_mean, my_num_obj, std::numeric_limits<Output_>::quiet_NaN());
        } else {
            for (std::size_t i = 0; i < my_num_obj; ++i) {
                if (my_num_unskipped[i] == 0) {
                    my_mean[i] = std::numeric_limits<Output_>::quiet_NaN();
                }
            }
        }
    }

private:
    std::size_t my_num_obj;
    Output_* my_mean;
    Output_* my_rss;
    Count_ my_count = 0;
    Count_* my_num_unskipped;

    static_assert(std::is_integral<Count_>::value);
    static_assert(std::is_floating_point<Output_>::value);
};

/**
 * @cond
 */
template<typename Output_ = double, typename Count_>
void welford_add_zeros(Output_& mean, Output_& sumsq, const Count_ num_total, const Count_ num_non_zero) {
    const auto ratio = static_cast<Output_>(num_non_zero) / static_cast<Output_>(num_total);
    sumsq += mean * mean * ratio * (num_total - num_non_zero);
    mean *= ratio;
}
/**
 * @endcond
 */

/** 
 * @brief Running residual sum of squares from sparse data.
 *
 * Compute running means and residual sum of squares (RSS) from sparse data using Welford's method.
 * This does the same as `RssRunningDense` but for sparse observed vectors, i.e., each vector only has the values/indices of the structural non-zero elements.
 * After the `finish()` call, `mean[i]` and `rss[i]` will contain the mean and RSS for objective vector `i`.
 * while `num_non_zero[i]` will contain the number of structural non-zero elements. 
 *
 * @tparam Count_ Integer type of the number of elements.
 * @tparam Input_ Numeric type of the input data.
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Count_, typename Input_, typename Output_ = double>
class RssRunningSparse {
public:
    /**
     * @param num_obj Number of objective vectors, i.e., \f$n\f$.
     * @param[out] mean Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     * @param[out] rss Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     * @param[out] num_non_zero Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     */
    RssRunningSparse(const std::size_t num_obj, Output_* const mean, Output_* const rss, Count_* const num_non_zero) : 
        my_num_obj(num_obj),
        my_mean(mean),
        my_rss(rss),
        my_num_non_zero(num_non_zero)
    {
        assert(check_zeroed(num_obj, mean));
        assert(check_zeroed(num_obj, rss));
        assert(check_zeroed(num_obj, num_non_zero));
    }

    /**
     * Add the next observed sparse vector to the variance calculation.
     *
     * No consideration is given to special values like NaNs in the structural non-zeros.
     * If these are to be skipped, consider using `RssRunningDenseSkip` instead.
     *
     * @tparam Index_ Integer type of the indices of the structural non-zeros.
     *
     * @param num_non_zero_obs Number of non-zero elements in the observed vector, i.e., the length of the `value` and `index` arrays.
     * @param[in] value Pointer to an array containing the values of the structural non-zero elements.
     * @param[in] index Pointer to an array containing the indices of the structural non-zero elements.
     * Elements should be unique and less than `num_obj` in the constructor.
     * Each element should correspond to an element in `value`.
     */
    template<typename Index_>
    void add(const std::size_t num_non_zero_obs, const Input_* const value, const Index_* const index) {
        static_assert(std::is_integral<Index_>::value);

        // my_count is the upper bound of all my_num_non_zero, so no need to check individual increments.
        my_count = sanisizer::sum<Count_>(my_count, 1);

        for (std::size_t i = 0; i < num_non_zero_obs; ++i) {
            const auto ri = index[i]; 
            welford_add(my_mean[ri], my_rss[ri], value[i], ++(my_num_non_zero[ri]));
        }
    }

    /**
     * Finalize the statistics.
     * This should only be called after all calls to `add()`.
     */
    void finish() {
        if (my_count == 0) {
            std::fill_n(my_mean, my_num_obj, std::numeric_limits<Output_>::quiet_NaN());
        } else {
            for (std::size_t i = 0; i < my_num_obj; ++i) {
                welford_add_zeros(my_mean[i], my_rss[i], my_count, my_num_non_zero[i]);
            }
        }
    }

private:
    std::size_t my_num_obj;
    Output_* my_mean;
    Output_* my_rss;
    Count_* my_num_non_zero;
    Count_ my_count = 0;

    static_assert(std::is_integral<Count_>::value);
    static_assert(std::is_floating_point<Output_>::value);
};

/** 
 * @brief Running residual sum of squares from sparse data, with skipping.
 *
 * This is a variant of `RssRunningSparse` that provides an option to skip elements of each observed vector, e.g., NaNs.
 * After the `finish()` call, `mean[i]` and `rss[i]` will contain the mean and RSS for the unskipped elements of objective vector `i`;
 * `num_non_zero[i]` will contain the number of structural non-zero elements;
 * and `num_unskipped[i]` will contain the number of unskipped elements. 
 *
 * Compute running means and residual sum of squares (RSS) from sparse data using Welford's method.
 * This does the same as `RssRunningDense` but for sparse observed vectors, i.e., each vector only has the values/indices of the structural non-zero elements.
 * After the `finish()` call, `mean[i]` and `rss[i]` will contain the mean and RSS for objective vector `i`.
 * while `num_non_zero[i]` will contain the number of structural non-zero elements. 
 *
 * @tparam Count_ Integer type of the number of elements.
 * @tparam Input_ Numeric type of the input data.
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Count_, typename Input_, typename Output_ = double>
class RssRunningSparseSkip {
public:
    /**
     * @param num_obj Number of objective vectors, i.e., \f$n\f$.
     * @param[out] mean Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     * @param[out] rss Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     * @param[out] num_non_zero Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     * @param[out] num_unskipped Pointer to an output array of length `num_obj`.
     * This should be zeroed on input.
     */
    RssRunningSparseSkip(const std::size_t num_obj, Output_* const mean, Output_* const rss, Count_* const num_non_zero, Count_* const num_unskipped) : 
        my_num_obj(num_obj),
        my_mean(mean),
        my_rss(rss),
        my_num_non_zero(num_non_zero),
        my_num_unskipped(num_unskipped)
    {
        assert(check_zeroed(num_obj, mean));
        assert(check_zeroed(num_obj, rss));
        assert(check_zeroed(num_obj, num_non_zero));
        assert(check_zeroed(num_obj, num_unskipped));
    }

    /**
     * Add the next observed sparse vector to the variance calculation.
     *
     * @tparam Skip_ Function that indicates whether to skip a particular element. 
     * @tparam Index_ Integer type of the indices of the structural non-zeros.
     *
     * @param num_non_zero_obs Number of non-zero elements in the observed vector, i.e., the length of the `value` and `index` arrays.
     * @param[in] value Pointer to an array containing the values of the structural non-zero elements.
     * @param[in] index Pointer to an array containing the indices of the structural non-zero elements.
     * Elements should be unique and less than `num_obj` in the constructor.
     * Each element should correspond to an element in `value`.
     * @param skip Function that accepts the index of an objective vector `index[i]` (as an `Index_`) and its value `value[i]` (as an `Input_`) for some `i < num_non_zero_obs`,
     * and returns a boolean indicating whether to skip this element.
     */
    template<typename Index_, class Skip_>
    void add(const std::size_t num_non_zero_obs, const Input_* value, const Index_* index, Skip_ skip) {
        static_assert(std::is_integral<Index_>::value);

        // my_count is the upper bound of all my_num_non_zero, so no need to check individual increments.
        my_count = sanisizer::sum<Count_>(my_count, 1);

        for (std::size_t i = 0; i < num_non_zero_obs; ++i) {
            const auto val = value[i];
            const auto ri = index[i]; 
            if (skip(ri, val)) {
                ++my_num_unskipped[ri]; // storing the number that was skipped so we don't have to add the zeros later.
            } else {
                welford_add(my_mean[ri], my_rss[ri], val, ++(my_num_non_zero[ri]));
            }
        }
    }

    /**
     * Finalize the statistics.
     * This should only be called after all calls to `add()`.
     */
    void finish() {
        for (std::size_t i = 0; i < my_num_obj; ++i) {
            my_num_unskipped[i] = my_count - my_num_unskipped[i];
        }

        if (my_count == 0) {
            std::fill_n(my_mean, my_num_obj, std::numeric_limits<Output_>::quiet_NaN());
        } else {
            for (std::size_t i = 0; i < my_num_obj; ++i) {
                if (my_num_unskipped[i] == 0) {
                    my_mean[i] = std::numeric_limits<Output_>::quiet_NaN();
                } else {
                    welford_add_zeros(my_mean[i], my_rss[i], my_num_unskipped[i], my_num_non_zero[i]);
                }
            }
        }
    }

private:
    std::size_t my_num_obj;
    Output_* my_mean;
    Output_* my_rss;
    Count_* my_num_non_zero;
    Count_* my_num_unskipped;
    Count_ my_count = 0;

    static_assert(std::is_integral<Count_>::value);
    static_assert(std::is_floating_point<Output_>::value);
};

/**
 * Recenter the residual sum of squares, i.e., sum of squares from a different mean.
 * This is typically used to combine RSS values from different subsets of the data, e.g., when splitting calculations across cores.
 * In such cases, the global mean across the entire dataset should be used as the new mean,
 * and the sum of the recentered RSS values will be the RSS of the entire dataset.
 * (This approach is more numerically stable than computing the sum of squared observations and then computing the difference with the squared mean.)
 *
 * @param num_total Total number of elements used to compute the RSS.
 * @param old_rss The old value of the RSS.
 * @param old_mean The old mean used to compute the RSS.
 * @param new_mean The new mean. 
 *
 * @tparam Float_ Floating-point type of the various statistics.
 *
 * @return The recentered RSS, or `old_rss` (which should be zero) if `num_total == 0`.
 */
template<typename Float_>
Float_ recenter_rss(const std::size_t num_total, const Float_ old_rss, const Float_ old_mean, const Float_ new_mean) {
    if (num_total == 0) {
        return old_rss;
    } else {
        const Float_ delta = old_mean - new_mean;
        return old_rss + num_total * delta * delta;
    }
}

}

#endif

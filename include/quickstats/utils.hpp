#ifndef QUICKSTATS_UTILS_HPP
#define QUICKSTATS_UTILS_HPP

#include <type_traits>

namespace quickstats {

/**
 * @cond
 */
template<typename Value_>
using I = std::remove_cv_t<std::remove_reference_t<Value_> >;
/**
 * @endcond
 */

}

#endif

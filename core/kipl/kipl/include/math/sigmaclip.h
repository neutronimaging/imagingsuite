#ifndef KIPL_MATH_SIGMACLIP_H
#define KIPL_MATH_SIGMACLIP_H

#include <vector>
#include <tuple>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <type_traits>

namespace kipl { namespace math {

/// @brief Apply sigma clipping to a dataset.
/// @tparam T The data type (e.g., float, double).
/// @param data The input data vector.
/// @param low The lower sigma threshold factor.
/// @param high The upper sigma threshold factor.
/// @return A tuple containing the clipped data, final lower bound, and final upper bound.
template<typename T>
std::tuple<std::vector<T>, double, double>
sigma_clip(const std::vector<T> &data, double low = 3.0, double high = 3.0);

}} // namespace kipl::math  

#include "core/sigmaclip.hpp"
#endif // KIPL_MATH_SIGMACLIP_H
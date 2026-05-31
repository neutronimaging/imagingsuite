//<LICENCE>
#ifndef UNIQUE_H
#define UNIQUE_H

#include <vector>

#include <base/kiplenums.h>

namespace kipl {
namespace algorithms {


/// @brief Returns a vector containing the unique values from the input vector. 
/// @tparam T Data type of the vector elements.  
/// @param v Input vector.
/// @return A vector containing the unique values from the input vector.
template<class T>
std::vector<T> dilate_points(std::vector<T> &v, const std::vector<size_t>& dims, kipl::base::eConnectivity conn);

}}
#endif

#include "core/listmorphology.hpp"
//<LICENCE>
#ifndef LISTMORPHOLOGY_H
#define LISTMORPHOLOGY_H

#include <vector>

#include <base/kiplenums.h>

namespace kipl {
namespace algorithms {


/// @brief Returns a vector containing the dilated points of the input vector. 
/// @tparam T Data type of the vector elements.  
/// @param v Input vector with points to dilate.
/// @return A vector containing the dilated points from the input vector.
template<class T>
std::vector<T> dilate_points(std::vector<T> &v, const std::vector<size_t>& dims, kipl::base::eConnectivity conn);

}}
#endif

#include "core/listmorphology.hpp"
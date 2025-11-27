//<LICENCE>
#ifndef UNIQUE_H
#define UNIQUE_H

#include <vector>

namespace kipl {
namespace algorithms {


/// @brief Returns a vector containing the unique values from the input vector. 
/// @tparam T Data type of the vector elements.  
/// @param v Input vector.
/// @return A vector containing the unique values from the input vector.
template<class T>
std::vector<T> unique_values(std::vector<T> v);

}}
#endif

#include "core/unique.hpp"
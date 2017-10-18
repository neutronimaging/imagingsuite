/**
 * \file string_util.hpp
 */
#ifndef __STRING_UTIL_HPP_GUARD__
#define __STRING_UTIL_HPP_GUARD__

#include <string>

namespace nxsum {
  /**
   * Convert a scalar number to a string.
   *
   * \param thing The number to convert.
   *
   * \return The string version of the number.
   */
  template <typename NumT> std::string toString(const NumT thing);

  /**
   * Convert a multidimensional array to a pointer.
   *
   * \param data The multidimensional array.
   * \param dims Dimensions of the array.
   * \param rank The number of dimensions.
   * \param type According to NeXus API.
   *
   * \return The string version of the array.
   */
  std::string toString(const void *data, const int dims[], const int rank,
                       const int type);
  /**
   * Convert a 1D array to a pointer.
   *
   * \param data The 1D array.
   * \param length Number of elements in the array.
   * \param type According to NeXus API.
   *
   * \return The string version of the array.
   */
  std::string toString(const void *data, const int length, const int type);

  std::string toUpperCase(const std::string &orig);
}
#endif

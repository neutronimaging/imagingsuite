/*
 * Copyright (c) 2007, P.F.Peterson <petersonpf@ornl.gov>
 *               Spallation Neutron Source at Oak Ridge National Laboratory
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "napiconfig.h" // needed for HAVE_STDINT_H
#include "nxsummary.hpp"
#include "string_util.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <napi.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "data_util.hpp"

// use STDINT if possible, otherwise define the types here
#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
typedef signed char             int8_t;
typedef short int               int16_t;
typedef int                     int32_t;
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
#ifdef _MSC_VER
typedef signed __int64          int64_t; 
typedef unsigned __int64        uint64_t;
#endif //_MSC_VER
#endif //HAVE_STDINT_H


using std::runtime_error;
using std::string;
using std::ostringstream;
using std::vector;

static const size_t NX_MAX_RANK = 25;

namespace nxsum {
  template <typename NumT>
  string toString(const NumT thing) {
    ostringstream s;
    s << thing;
    return s.str();
  }

  // explicit instantiations so they get compiled in
  template string toString<uint32_t>(const uint32_t thing);
  template string toString<int>(const int thing);
  template string toString<double>(const double thing);
  template string toString<float>(const float thing);

  template <typename NumT>
  string toString(const NumT *data, const int dims[], const int rank) {
    int num_ele = 1;
    for (size_t i = 0; i < rank; ++i ) {
      num_ele *= dims[i];
    }

    if (num_ele == 1)
      {
        return toString(data[0]);
      }

    if ((rank == 1) && (num_ele < NX_MAX_RANK))
      {
        ostringstream s;
        s << '[';
        size_t length = dims[0];
        for (size_t i = 0; i < length; ++i) {
          s << toString(data[i]);
          if (i+1 < length)
            {
              s << ',';
            }
        }
        s << ']';
        return s.str();
      }
    else
      {
        throw runtime_error("Do not know how to work with arrays");
      }
  }

  string toString(const void *data, const int dims[], const int rank,
                  const int type) {
    if (type == NX_CHAR)
      {
        return (char *) data;
      }
    else if (type == NX_FLOAT32)
      {
        return toString((float *)data, dims, rank);
      }
    else if (type == NX_FLOAT64)
      {
        return toString((double *)data, dims, rank);
      }
    else if (type == NX_INT8)
      {
        return toString((int8_t *)data, dims, rank);
      }
    else if (type == NX_UINT8)
      {
	return toString((uint8_t *)data, dims, rank);
      }
    else if (type == NX_INT16)
      {
        return toString((int16_t *)data, dims, rank);
      }
    else if (type == NX_UINT16)
      {
	return toString((uint16_t *)data, dims, rank);
      }
    else if (type == NX_INT32)
      {
        return toString((int32_t *)data, dims, rank);
      }
    else if (type == NX_UINT32)
      {
	return toString((uint32_t *)data, dims, rank);
      }
    else if (type == NX_INT64)
      {
        return toString((int64_t *)data, dims, rank);
      }
    else if (type == NX_UINT64)
      {
	return toString((uint64_t *)data, dims, rank);
      }
    else
      {
        ostringstream s;
        s << "Do not know how to work with type=" << nxtypeAsString(type);
        throw runtime_error(s.str());
      }
  }

  string toString(const void *data, const int length, const int type) {
    int dims[1]  = {length};
    return toString(data, dims, 1, type);
  }

  string toUpperCase(const string &orig) {
    string result = orig;
    std::transform(orig.begin(), orig.end(), result.begin(), (int(*)(int))std::toupper);
    return result;
  }
}

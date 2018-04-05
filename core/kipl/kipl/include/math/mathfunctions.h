//<LICENCE>

#ifndef _MATHFUNCTIONS_H_
#define _MATHFUNCTIONS_H_

#include <complex>

#include "../base/timage.h"


namespace kipl { namespace math {
template <typename T, size_t N>
kipl::base::TImage<T,N> abs(kipl::base::TImage<T,N> img);

//template <typename T, size_t N>
//kipl::base::TImage<T,N> abs(const kipl::base::TImage<std::complex<T>,N> cimg);ame T

template <typename T, size_t N>
kipl::base::TImage<T,N> real(const kipl::base::TImage<std::complex<T>,N> cimg);

template <typename T, size_t N>
kipl::base::TImage<T,N> imag(const kipl::base::TImage<std::complex<T>,N> cimg);

template <typename T, size_t N>
kipl::base::TImage<T,N> sqr(const kipl::base::TImage<T,N> img);

template <typename T, size_t N>
kipl::base::TImage<T,N> sqrt(const kipl::base::TImage<T,N> img);

/// \brief Compute the sigmoid function of the input data
/// \param x     - input data
/// \param level - value for center point in the sigmoid function
/// \param width - width of the sigmoid function (for w=1, 99.9% of the sigmoid are within +/-2pi)
template <typename T>
T Sigmoid(const T x, const T level, const T width);

/// \brief Compute the sigmoid function of the input data
/// \param img   - input data
/// \param level - value for center point in the sigmoid function
/// \param width - width of the sigmoid function (for w=1, 99.9% of the sigmoid are within +/-2pi)
template <typename T, size_t N>
kipl::base::TImage<T,N> SigmoidWeights(kipl::base::TImage<T,N> img, const T level, const T width);

/// \brief Compute the sigmoid function of the input data
/// \param img   - input data
/// \param level - value for center point in the sigmoid function
/// \param width - width of the sigmoid function (for w=1, 99.9% of the sigmoid are within +/-2pi)
template <typename T>
T SigmoidWeights(T val,T a, T b, const float level, const float width);

template <typename T>
double Entropy(T const * const data, const size_t N);

/// \brief Determines the sign of the argument
/// \param x The number to define the sign for.
/// \returns -1 for negative, 0 for zero, +1 for positive.
template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}


kipl::base::TImage<float,2> Unwrap(kipl::base::TImage<float,2> img);

int pow(float *data, size_t N, float exponent);



}}

#include "core/mathfunctions.hpp"

#endif

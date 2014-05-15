#ifndef _COMPLEXIMAGE_H_
#define _COMPLEXIMAGE_H_

#include "../base/timage.h"
#include <complex>

namespace kipl { namespace complexmath {
template <typename T, size_t N>
kipl::base::TImage<T,N> abs(const kipl::base::TImage<T,N> img);

template <typename T, size_t N>
kipl::base::TImage<T,N> abs(const kipl::base::TImage<std::complex<T>,N> cimg);

template <typename T, size_t N>
kipl::base::TImage<T,N> arg(const kipl::base::TImage<std::complex<T>,N> cimg);

template <typename T, size_t N>
kipl::base::TImage<T,N> real(const kipl::base::TImage<std::complex<T>,N> cimg);

template <typename T, size_t N>
kipl::base::TImage<T,N> imag(const kipl::base::TImage<std::complex<T>,N> cimg);

template <typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> conj(const kipl::base::TImage<std::complex<T>,N> cimg);

template <typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> complex(const kipl::base::TImage<T,N> &re, const kipl::base::TImage<T,N> &im);

// Complex valued arithmetics
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator+(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator+(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB);
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator+(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);

template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator-(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator-(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB);
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator-(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);

template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator*(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator*(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB);
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator*(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);

template<typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> operator/(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> operator/(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB);
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> operator/(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);



}}

#include "core/compleximage.hpp"

#endif

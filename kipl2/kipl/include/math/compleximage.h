//<LICENCE>

#ifndef _COMPLEXIMAGE_H_
#define _COMPLEXIMAGE_H_

#include <complex>

#include "../base/timage.h"

namespace kipl {
/// \brief A name space that contains operations on complex valued images
namespace complexmath {
/// \brief Compute the pixelwise absolute value of a real valued image
/// \param img The image to process
/// \returns The absolute value of img
template <typename T, size_t N>
kipl::base::TImage<T,N> abs(const kipl::base::TImage<T,N> img);

/// \brief Compute the pixelwise absolute value of a complex valued image
/// \param cimg The image to process
/// \returns The absolute value of img
template <typename T, size_t N>
kipl::base::TImage<T,N> abs(const kipl::base::TImage<std::complex<T>,N> cimg);

/// \brief Compute the pixelwise argument of a complex valued image
/// \param img The image to process
/// \returns The argument of img
template <typename T, size_t N>
kipl::base::TImage<T,N> arg(const kipl::base::TImage<std::complex<T>,N> cimg);

/// \brief Extracts the real part of a complex valued image
/// \param cimg The image to process
/// \returns The real part of cimg
template <typename T, size_t N>
kipl::base::TImage<T,N> real(const kipl::base::TImage<std::complex<T>,N> cimg);

/// \brief Extracts the imagingary part of a complex valued image
/// \param cimg The image to process
/// \returns The imaginary part of cimg
template <typename T, size_t N>
kipl::base::TImage<T,N> imag(const kipl::base::TImage<std::complex<T>,N> cimg);

/// \brief Computes the pixelwise complex conjugate a complex valued image
/// \param cimg The image to process
/// \returns The complex conjugate of cimg
template <typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> conj(const kipl::base::TImage<std::complex<T>,N> cimg);

/// \brief Combines two real valued image into a complex image
/// \param re The real part image
/// \param im The imaginary part image
/// \returns Returns an image built as re + j im.
template <typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> complex(const kipl::base::TImage<T,N> &re, const kipl::base::TImage<T,N> &im);

// Complex valued arithmetics
/// \brief Computes the sum of a real valued and a complex valued image
/// \param imgA real valued image
/// \param imgB a complex valued image
/// \returns the pixelwise sum imgA+imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator+(	const kipl::base::TImage<T,N> &imgA, 
                                                    const kipl::base::TImage<std::complex<T>,N> &imgB);

/// \brief Computes the sum of a complex valued and a real valued image
/// \param imgA complex valued image
/// \param imgB real valued image
/// \returns the pixelwise sum imgA+imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator+(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB);

/// \brief Computes the sum of two complex valued images
/// \param imgA complex valued image
/// \param imgB complex valued image
/// \returns the pixelwise sum imgA+imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator+(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);

/// \brief Computes the difference between a complex valued and a real valued image
/// \param imgA complex valued image
/// \param imgB real valued image
/// \returns the pixelwise difference imgA-imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator-(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);

/// \brief Computes the difference between a complex valued and a real valued image
/// \param imgA complex valued image
/// \param imgB real valued image
/// \returns the pixelwise difference imgA-imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator-(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB);

/// \brief Computes the difference between two complex valued images
/// \param imgA complex valued image
/// \param imgB real valued image
/// \returns the pixelwise difference imgA-imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator-(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);

/// \brief Computes the product between a complex valued and a real valued image
/// \param imgA complex valued image
/// \param imgB real valued image
/// \returns the pixelwise product imgA*imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator*(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);

/// \brief Computes the product between a complex valued and a real valued image
/// \param imgA complex valued image
/// \param imgB real valued image
/// \returns the sum imgA*imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator*(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB);

/// \brief Computes the product between two complex valued images
/// \param imgA complex valued image
/// \param imgB real valued image
/// \returns the pixelwise product imgA*imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator*(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);

/// \brief Computes the division between a complex valued and a real valued image
/// \param imgA complex valued image
/// \param imgB real valued image
/// \returns the pixelwise division imgA/imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> operator/(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);

/// \brief Computes the division between a complex valued and a real valued image
/// \param imgA complex valued image
/// \param imgB real valued image
/// \returns the pixelwise division imgA/imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> operator/(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB);

/// \brief Computes the division between two complex valued images
/// \param imgA complex valued image
/// \param imgB real valued image
/// \returns the pixelwise division imgA/imgB
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> operator/(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB);



}}

#include "core/compleximage.hpp"

#endif

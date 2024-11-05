

#ifndef IMAGESAMPLERS_H
#define IMAGESAMPLERS_H

#include "../kipl_global.h"
#include "timage.h"

namespace kipl { namespace base {
// \todo Implement... but what is it used for?
KIPLSHARED_EXPORT void BinDimensions(size_t const * const origdims, size_t nDims, size_t const * const nbin,size_t const * const resultdims);

/// \brief Downsizes an image by bining pixels
///	\param img input image
///	\param result resulting binned image
///	\param nbin array containing number of bins in each principal direction
template <class ImgType, size_t NDim>
int ReBin(const kipl::base::TImage<ImgType,NDim> &img, 
		kipl::base::TImage<ImgType,NDim> &result, 
		size_t const * const nbin);

/// \brief Downsizes an image by bining pixels
///	\param img input image
///	\param result resulting binned image
///	\param nbin array containing number of bins in each principal direction
template <class ImgType, size_t NDim>
int GaussianSampler(	const kipl::base::TImage<ImgType,NDim> &img, 
		kipl::base::TImage<ImgType,NDim> &result, 
						int scale, 
						double sigma); 
/*
template <class ImgType, size_t NDim>
int ReBlock(kipl::base::TImage<ImgType,NDim> &img, 
		kipl::base::TImage<ImgType,NDim> &result, 
		size_t *nblock);

/// \brief Mirrors the image for the given plane
/// \param img input image
/// \param result reversed image
/// \param axis selects which axis to reverse
template <class ImgType, size_t NDim>
int ReverseAxis(kipl::base::TImage<ImgType,NDim> &img, 
		kipl::base::TImage<ImgType,NDim> &result, 
		kipl::base::AxisType axis);

/// \brief Permutes the data to new axes
/// \param img input image
/// \param result permuted image
/// \param perm permutation array containing the axis indices (0=x-axis, 1=y-axis, 2=z-axis)
template <class ImgType, size_t NDim>
int PermuteAxis(kipl::base::TImage<ImgType,NDim> &img, 
		kipl::base::Image<ImgType,NDim> &result,
		size_t *perm);
*/
}}

#include "core/imagesamplers.hpp"
#endif

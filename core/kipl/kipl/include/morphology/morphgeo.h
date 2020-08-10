//<LICENCE>

#ifndef MORPHGEO_H
#define MORPHGEO_H
#include <iostream>
#include <deque>

#include "../base/timage.h"
#include "../base/imageoperators.h"
#include "../base/kiplenums.h"

#include "morphology.h"
#include "morphfilters.h"

using namespace std;

namespace kipl { namespace morphology {

/// \brief Performs a Self dual Geodesic Reconstruction of the image
///	\param f mask image
///	\param g marker image
///	\param conn Selects connectivity
///
///	if \f$f\leq g\f$ will reconstruction by erosion \f$R^{\epsilon}_f\f$ be performed, if 
///	\f$g\leq f\f$ will reconstruction by dilation \f$R^{\delta}_f\f$ be performed
/// 
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
template <typename ImgType, size_t NDim>
kipl::base::TImage<ImgType,NDim> SelfDualReconstruction(kipl::base::TImage<ImgType,NDim> &f, 
		kipl::base::TImage<ImgType,NDim> &g, 
        kipl::base::eConnectivity conn);

/// \brief Performs a Reconstruction by dilation of the image (\f$R^{\delta}_g(f) \f$)
///	\param g mask image
///	\param f marker image
///	\param conn Selects connectivity
///
///	\note \f$f \leq g \f$ 
/// 
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
///	\todo Fix edge processing 3D
template <typename ImgType,size_t NDimG, size_t NDimF>
kipl::base::TImage<ImgType,NDimG> RecByDilation(const kipl::base::TImage<ImgType,NDimG> &g, 
		const kipl::base::TImage<ImgType,NDimF> &f, 
        kipl::base::eConnectivity conn);

/// \brief Performs a Reconstruction by erosion of the image
///	\param f mask image
///	\param g marker image
///	\param conn Selects connectivity
///
///	\note \f$g \leq f \f$ 
/// 
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
template <typename ImgType, size_t NDimG, size_t NDimF>
kipl::base::TImage<ImgType,NDimG> RecByErosion(const kipl::base::TImage<ImgType,NDimG> &g, 
		const kipl::base::TImage<ImgType,NDimF> &f, 
        kipl::base::eConnectivity  conn);

/// \brief Removes the objects that are connected to the edge of the image
///	\param img Input image
///	\param conn Connectivity of the reconstruction
///
///	Computes \f$img-R^{\delta}_{img}(fm)\f$ where the border of fm is equal 
///	to the border of img while the rest of fm is equal to 0
template <typename ImgType, size_t NDim>
kipl::base::TImage<ImgType,NDim> RemoveEdgeObj(kipl::base::TImage<ImgType,NDim> &img, 
        kipl::base::eConnectivity conn);

// /// \brief Opens the image by reconstruction
// ///	\param img Input image
// ///	\param er The erosion operator
// ///	\param conn Connectivity selector
// ///
// ///	Computes \f$R^{\delta}_{img}(\epsilon(img))\f$
//template <typename ImgType>
//	kipl::base::TImage<ImgType,2> OpenByRec(kipl::base::TImage<ImgType,2> &img,
//			CGrayErode<ImgType,2> &er,
//			kipl::base::MorphConnect conn=kipl::morphology::conn4);

// /// \brief Closes the image by reconstruction
// ///	\param img Input image
// ///	\param di The dilation operator
// ///	\param conn Connectivity selector
// ///
// ///	Computes \f$R^{\epsilon}_{img}(\delta(img))\f$
//template <typename ImgType,size_t NDim>
//kipl::base::TImage<ImgType,NDim> CloseByRec(kipl::base::TImage<ImgType,NDim> &img,
//			TDilate<ImgType,NDim> &di,
//			kipl::morphology::MorphConnect conn=NDim==2 ? kipl::morphology::conn4 : kipl::morphology::conn6);

// /// \brief Closes the image by reconstruction
// ///	\param img Input image
// ///	\param di The dilation operator
// ///	\param conn Connectivity selector
// ///
// ///	Computes \f$R^{\epsilon}_{img}(\delta(img))\f$
//template <typename ImgType, size_t NDim>
//	kipl::base::TImage<ImgType,NDim> GrayCloseByRec(kipl::base::TImage<ImgType,NDim> &img,
//			TGrayDilate<ImgType,NDim> &di,
//			kipl::morphology::MorphConnect conn=NDim==2 ? kipl::morphology::conn4 : kipl::morphology::conn6);


}} // end namespace morphology

namespace kipl { namespace morphology { namespace old {
/// \brief Performs a Self dual Geodesic Reconstruction of the image
///	\param f mask image
///	\param g marker image
///	\param conn Selects connectivity
///
///	if \f$f\leq g\f$ will reconstruction by erosion \f$R^{\epsilon}_f\f$ be performed, if
///	\f$g\leq f\f$ will reconstruction by dilation \f$R^{\delta}_f\f$ be performed
///
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
template <typename ImgType, size_t NDim>
kipl::base::TImage<ImgType,NDim> SelfDualReconstruction(kipl::base::TImage<ImgType,NDim> &f,
        kipl::base::TImage<ImgType,NDim> &g,
        kipl::morphology::MorphConnect conn);

/// \brief Performs a Reconstruction by dilation of the image (\f$R^{\delta}_g(f) \f$)
///	\param g mask image
///	\param f marker image
///	\param conn Selects connectivity
///
///	\note \f$f \leq g \f$
///
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
///	\todo Fix edge processing 3D
template <typename ImgType,size_t NDimG, size_t NDimF>
kipl::base::TImage<ImgType,NDimG> RecByDilation(const kipl::base::TImage<ImgType,NDimG> &g,
        const kipl::base::TImage<ImgType,NDimF> &f,
        kipl::morphology::MorphConnect conn);

/// \brief Performs a Reconstruction by erosion of the image
///	\param f mask image
///	\param g marker image
///	\param conn Selects connectivity
///
///	\note \f$g \leq f \f$
///
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
template <typename ImgType, size_t NDimG, size_t NDimF>
kipl::base::TImage<ImgType,NDimG> RecByErosion(const kipl::base::TImage<ImgType,NDimG> &g,
        const kipl::base::TImage<ImgType,NDimF> &f,
        kipl::morphology::MorphConnect conn);

/// \brief Removes the objects that are connected to the edge of the image
///	\param img Input image
///	\param conn Connectivity of the reconstruction
///
///	Computes \f$img-R^{\delta}_{img}(fm)\f$ where the border of fm is equal
///	to the border of img while the rest of fm is equal to 0
template <typename ImgType, size_t NDim>
kipl::base::TImage<ImgType,NDim> RemoveEdgeObj(kipl::base::TImage<ImgType,NDim> &img,
        kipl::morphology::MorphConnect conn);

}}}

#include "core/morphgeo.hpp"
#endif

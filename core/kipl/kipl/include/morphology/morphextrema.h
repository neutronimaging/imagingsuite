//<LICENSE>

#ifndef __MORPHEXTREMA_H
#define __MORPHEXTREMA_H

#include <deque>
#include <iostream>

#include "../base/KiplException.h"
#include "../base/timage.h"
#include "../base/kiplenums.h"
#include "../math/image_statistics.h"
#include "morphology.h"
#include "morphgeo.h"


/// Reconstruction based extrem operations
namespace kipl {
namespace morphology {
/// \brief Computes the regional minimum image
///	\param img Input image
///	\param extremes Resulting image
///	\param conn Connectivity selector
///
///	\todo Fix edge processing

template <typename ImgType,size_t N>
int RMin(const kipl::base::TImage<ImgType,N> &img, kipl::base::TImage<ImgType,N> &extremes, kipl::base::eConnectivity conn, bool bilevel=true);

/// \brief Computes the regional maximum image
///	\param img Input image
///	\param extremes Resulting image
///	\param conn Connectivity selector
///
///	\todo Fix edge processing
template <typename ImgType,size_t N>
int RMax(const kipl::base::TImage<ImgType,N> &img, kipl::base::TImage<ImgType,N> &extremes, kipl::base::eConnectivity conn);

///    \brief Computes the h maximum image
///	\param img Input image
///	\param result Resulting image
///	\param h threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$R^{\delta}_{img}(img-h)\f$
///	\todo Fix edge processing
template <typename ImgType,size_t N>
int hMax(const kipl::base::TImage<ImgType,N> &img,kipl::base::TImage<ImgType,N> &result,ImgType h, kipl::base::eConnectivity conn);

/// \brief Computes the extended maximum image
///	\param img Input image
///	\param res Resulting image
///	\param h Threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$RMAX(hMAX_h(img)\f$
///	\todo Fix edge processing
template <typename ImgType, int NDim>
int ExtendedMax(const kipl::base::TImage<ImgType,NDim> &img, kipl::base::TImage<ImgType,NDim> &res, ImgType h, kipl::base::eConnectivity conn);

/// \brief Computes the h minimum image
///	\param img Input image
///	\param res Resulting image
///	\param h threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$R^{\epsilon}_{img}(img+h)\f$
///	\todo Fix edge processing
template <class ImgType, size_t N>
int hMin(const kipl::base::TImage<ImgType,N> &img,kipl::base::TImage<ImgType,N> &res, ImgType h, kipl::base::eConnectivity conn);

/// \brief Computes the extended minimum image
///	\param img Input image
///	\param res Output image
///	\param h threshold level
///	\param conn Connectivity selector
///	\param bilevel obsolete parameter
///
///	The method computes \f$RMIN(hMIN_h(img)\f$
///	\todo Fix edge processing
template <typename T, size_t N>
int ExtendedMin(const kipl::base::TImage<T,N> &img,kipl::base::TImage<T,N> &res, T h, kipl::base::eConnectivity conn, bool bilevel=true);

//  \brief Imposes minimum according to a marker image
//	\param f Input image
//	\param fm Marker image fm=0 for min and =max{f} otherwise
//	\param res Resulting image
//	\param conn Connectivity of the reconstruction

//	Computes \f$R^{\epsilon}_{(f+1)\wedge f_m}(f_m)\f$
template <typename T, size_t N>
int MinImpose(const kipl::base::TImage<T,N> &f, const kipl::base::TImage<T,N> &fm, kipl::base::TImage<T,N> &res, kipl::base::eConnectivity conn);

template <typename T>
kipl::base::TImage<T,2> FillHole(kipl::base::TImage<T,2> &img, kipl::base::eConnectivity conn);

template <typename T>
kipl::base::TImage<T,2> FillPeaks(kipl::base::TImage<T,2> &img, kipl::base::eConnectivity conn);

template <typename T>
kipl::base::TImage<T,2> FillExtrema(kipl::base::TImage<T,2> &img);

template <typename T>
kipl::base::TImage<T,2> FillHole2(kipl::base::TImage<T,2> &img, kipl::base::eConnectivity conn);
}}

/// Reconstruction based extrem operations
namespace kipl {
namespace morphology {
namespace old {
/// \brief Computes the regional minimum image
///	\param img Input image
///	\param extremes Resulting image
///	\param conn Connectivity selector
///
///	\todo Fix edge processing

template <typename ImgType,size_t N>
int RMin(const kipl::base::TImage<ImgType,N> &img, kipl::base::TImage<ImgType,N> &extremes, MorphConnect conn, bool bilevel=true);

/// \brief Computes the regional maximum image
///	\param img Input image
///	\param extremes Resulting image
///	\param conn Connectivity selector
///
///	\todo Fix edge processing
template <typename ImgType,size_t N>
int RMax(const kipl::base::TImage<ImgType,N> &img, kipl::base::TImage<ImgType,N> &extremes, MorphConnect conn);

///    \brief Computes the h maximum image
///	\param img Input image
///	\param result Resulting image
///	\param h threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$R^{\delta}_{img}(img-h)\f$
///	\todo Fix edge processing
template <typename ImgType,size_t N>
int hMax(const kipl::base::TImage<ImgType,N> &img,kipl::base::TImage<ImgType,N> &result,ImgType h, MorphConnect conn=conn4);

/// \brief Computes the extended maximum image
///	\param img Input image
///	\param res Resulting image
///	\param h Threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$RMAX(hMAX_h(img)\f$
///	\todo Fix edge processing
template <typename ImgType, int NDim>
int ExtendedMax(const kipl::base::TImage<ImgType,NDim> &img, kipl::base::TImage<ImgType,NDim> &res, ImgType h, MorphConnect conn=conn4);

/// \brief Computes the h minimum image
///	\param img Input image
///	\param res Resulting image
///	\param h threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$R^{\epsilon}_{img}(img+h)\f$
///	\todo Fix edge processing
template <class ImgType, size_t N>
int hMin(const kipl::base::TImage<ImgType,N> &img,kipl::base::TImage<ImgType,N> &res, ImgType h, MorphConnect conn=conn4);

/// \brief Computes the extended minimum image
///	\param img Input image
///	\param res Output image
///	\param h threshold level
///	\param conn Connectivity selector
///	\param bilevel obsolete parameter
///
///	The method computes \f$RMIN(hMIN_h(img)\f$
///	\todo Fix edge processing
template <typename T, size_t N>
int ExtendedMin(const kipl::base::TImage<T,N> &img,kipl::base::TImage<T,N> &res, T h, MorphConnect conn=conn4, bool bilevel=false);

//  \brief Imposes minimum according to a marker image
//	\param f Input image
//	\param fm Marker image fm=0 for min and =max{f} otherwise
//	\param res Resulting image
//	\param conn Connectivity of the reconstruction

//	Computes \f$R^{\epsilon}_{(f+1)\wedge f_m}(f_m)\f$
template <typename T, size_t N>
int MinImpose(const kipl::base::TImage<T,N> &f, const kipl::base::TImage<T,N> &fm, kipl::base::TImage<T,N> &res, MorphConnect conn=conn4);

template <typename T>
kipl::base::TImage<T,2> FillHole(kipl::base::TImage<T,2> &img, kipl::morphology::MorphConnect conn);

template <typename T>
kipl::base::TImage<T,2> FillPeaks(kipl::base::TImage<T,2> &img, kipl::morphology::MorphConnect conn);

template <typename T>
kipl::base::TImage<T,2> FillExtrema(kipl::base::TImage<T,2> &img);

template <typename T>
kipl::base::TImage<T,2> FillHole2(kipl::base::TImage<T,2> &img, kipl::morphology::MorphConnect conn);
}}}

#include "core/morphextrema.hpp"
#endif

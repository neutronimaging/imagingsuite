//<LICENCE>

#ifndef TEXTRACTOR_H_
#define TEXTRACTOR_H_

#include "timage.h"
#include "kiplenums.h"

namespace kipl { namespace base {

/// \brief Extracts a 2D slice from a 3D image
/// \param img The source image
/// \param index The slice index along the chosen axis
/// \param plane The chosen image plane
/// \returns A 2D slice
template<typename T,size_t N>
kipl::base::TImage<T,2> ExtractSlice(const kipl::base::TImage<T,N> &img, const size_t index, const kipl::base::eImagePlanes plane, const std::vector<size_t> & roi={});

/// \brief Inserts a 2D slice into a 3D image
/// \param slice The slice to insert
/// \param img The target volume
/// \param index The slice index along the chosen axis
/// \param plane The chosen image plane
template<typename T>
void InsertSlice(kipl::base::TImage<T,2> &slice, kipl::base::TImage<T,3> &volume, const size_t index, const kipl::base::eImagePlanes plane);

template<typename T>
kipl::base::TImage<float,2> getPatch(kipl::base::TImage<T,2> srcImg, size_t x,size_t y,size_t h,size_t w, size_t margin);

template<typename T>
void putPatch(kipl::base::TImage<T,2> patch, size_t x,size_t y, size_t margin, kipl::base::TImage<T,2> dstImg);

}}

#include "core/textractor.hpp"
#endif /*TEXTRACTOR_H_*/

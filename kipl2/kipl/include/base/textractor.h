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
template<typename T>
kipl::base::TImage<T,2> ExtractSlice(kipl::base::TImage<T,3> &img, const size_t index, const kipl::base::eImagePlanes plane, size_t *roi=NULL);

/// \brief Inserts a 2D slice into a 3D image
/// \param slice The slice to insert
/// \param img The target volume
/// \param index The slice index along the chosen axis
/// \param plane The chosen image plane
template<typename T>
void InsertSlice(kipl::base::TImage<T,2> &slice, kipl::base::TImage<T,3> &volume, const size_t index, const kipl::base::eImagePlanes plane);

}}

#include "core/textractor.hpp"
#endif /*TEXTRACTOR_H_*/

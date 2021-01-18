//<LICENCE>

#ifndef TSUBIMAGE_H_
#define TSUBIMAGE_H_

#include <vector>

#include "timage.h"

namespace kipl { namespace base {

/// \brief The class offers different ways to extract sub images and put them back again.
template <typename T, size_t NDims>
class TSubImage {
public:
    /// \brief Gets a sub image using start position and width for each axis
    /// \param src The source image
    /// \param nStart array with starting positions for each image axis
    /// \param nLength array with subimage width for each image axis
    /// \returns the extracted image
    static TImage<T,NDims> Get(TImage<T,NDims> const src, const std::vector<size_t> & nStart, const std::vector<size_t> & nLength);

    /// \brief Gets a sub image using roi coordinates
    /// \param src The source image
    /// \param roi array containing coordinate tuples for the diagonal corners of the image
    /// \param includeCoord include the end coordinate (e.g. x1-x0+1) on true or (x1-x0) on false
    /// \returns the extracted image
    /// \test Unit test is available
    static TImage<T,NDims> Get(TImage<T,NDims> const src, const std::vector<size_t> & roi, bool includeCoord=true);

    /// \brief Puts a sub image back into a larger image
    /// \param src The sub image to put
    /// \param dest the larger image that will receive the sub image
    /// \param nStart tuple with the coordinate of the insertion corner
	static void Put(const TImage<T,NDims> src, TImage<T, NDims> dest, size_t const *const nStart);
};

/// \brief Extracts a 2D slice in the XY plane from a 3D image
/// \param volume the source image
/// \param idx the slice index along the Z-axis
/// \returns The extracted slice
template <typename T>
TImage<T,2> ExtractSlice(TImage<T,3> volume, size_t idx);

/// \brief Inserts a 2D slice into the XY plane of a 3D image
/// \param volume the source image
/// \param slice the image to insert in the slice plane
/// \param idx the slice index along the Z-axis
template <typename T>
void InsertSlice(TImage<T,3> volume, TImage<T,2> slice, size_t idx);
}}

#include "core/tsubimage.hpp"

#endif /*TEXTRACTOR_H_*/

//<LICENCE>

#ifndef TPERMUTEIMAGE_H
#define TPERMUTEIMAGE_H

#include "timage.h"
#include "kiplenums.h"

namespace kipl { namespace base {
/// \brief Mirrors the pixels in the specified direction
/// \param img The image to mirror
/// \param axis The image axis to mirror around
/// \todo Check if this can be merged or combined with flip image
/// \returns the mirrored image
template <typename T, size_t N>
TImage<T,N> Mirror(TImage<T,N> img, eImageAxes axis);

/// \brief image (2D only) transposing class
///
/// The template argument N defines the buffer size
template<class T, size_t N=16>
class Transpose {
public:  
    /// \brief Basic C'tor
	Transpose() : bUseReference(false) {}

    /// \brief Transpose an image in pointer format
    /// \param fSrc reference to the source image data
    /// \param fDest reference to the destimation image
    /// \param nDims array containing the image dimensions
    /// \returns Always true
    bool operator() (T const * const fSrc, T *fDest, size_t const * const nDims);

    /// \brief Transposing a  TImage
    /// \param src The image to transpose
    ///
    /// \returns the transposed image
    TImage<T,2> operator() (const TImage<T,2> src);

    /// \var A switch to use the refence code for performance evaluation
    bool bUseReference;
protected:
  void TransposeSubBlock(T const * const fSrc, 
                       T *block, 
                       size_t const * const nDims);

  void InsertSubBlock(T const * const block, 
                       T *fDest, 
                       size_t const * const nDims);

  void TransposeRef(T const * const fSrc, T *fDest, size_t const * const nDims);

};

/// \brief Axis permutation class
template <typename T>
class PermuteAxes {
public:
    /// \brief Permutes the axes of a 3D image
    /// \param src The image to permute
    /// \param permutation Selects which permutation to use
    /// \returns the permuted image
	kipl::base::TImage<T,3> operator() (kipl::base::TImage<T,3> src, const kipl::base::eAxisPermutations permutation);
};

}}
#include "core/transpose.hpp"
#include "core/tpermuteimage.hpp"
#endif


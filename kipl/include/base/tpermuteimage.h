//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//

#ifndef _TPERMUTEIMAGE_H_
#define _TPERMUTEIMAGE_H_

#include "timage.h"
#include "kiplenums.h"
namespace kipl { namespace base {
 
template <typename T, size_t N>
TImage<T,N> Mirror(TImage<T,N> img, eImageAxes axis);

template<class T, size_t N=16>
class Transpose {
public:  
	Transpose() : bUseReference(false) {}
  bool operator() (T const * const fSrc, T *fDest, size_t const * const nDims);
  TImage<T,2> operator() (const TImage<T,2> src);
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

enum eAxisPermutations {
		PermuteXYZ,
		PermuteXZY,
		PermuteYXZ,
		PermuteYZX,
		PermuteZXY,
		PermuteZYX
};

template <typename T>
class PermuteAxes {
public:
	kipl::base::TImage<T,3> operator() (kipl::base::TImage<T,3> src, const kipl::base::eAxisPermutations permutation);
};

}}
#include "core/transpose.hpp"
#include "core/tpermuteimage.hpp"
#endif


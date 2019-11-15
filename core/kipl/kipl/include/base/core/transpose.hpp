//<LICENCE>

#ifndef __TRANSPOSE_HPP
#define __TRANSPOSE_HPP

#include <memory.h>
#include "../timage.h"

namespace kipl { namespace base {
template <class T, size_t N>
inline void Transpose<T,N>::TransposeSubBlock(T const * const fSrc, 
                       T *block, 
                       size_t const * const nDims)
{
  const size_t sizeU=nDims[0];
  const size_t nU=sizeU*N;
  for (size_t v=0, i=0; v<N; v++) {
    for (size_t u=0; u<nU; u+=sizeU,i++) {
       block[i]=fSrc[u+v];

    }
  } 
}

template <class T, size_t N>
inline void Transpose<T,N>::InsertSubBlock(T const * const block, 
                       T *fDest, 
                       size_t const * const nDims)
{
  const size_t M=N*sizeof(T);

  for (size_t v=0; v<N; v++) {
    memcpy(fDest+v*nDims[1],block+v*N, M);
  } 
  
}

template <class T, size_t N>
TImage<T,2> Transpose<T,N>::operator()(const TImage<T,2> &src)
{
	size_t dims[2]={src.Size(1), src.Size(0)};
	TImage<T,2> dest(dims);

	if (bUseReference==false)
		this->operator()(src.GetDataPtr(),dest.GetDataPtr(),src.Dims());
	else
		this->TransposeRef(src.GetDataPtr(),dest.GetDataPtr(),src.Dims());

	return dest;
}

template <class T, size_t N>
bool Transpose<T,N>::operator()(T const * const fSrc, T *fDest, size_t const * const nDims)
{
  T * block = new T[N*N];

  const size_t sizeU=nDims[0];
  const size_t sizeV=nDims[1];

  const size_t restU=sizeU%N;
  const size_t restV=sizeV%N;

  const size_t endU=sizeU-restU;
  const size_t endV=sizeV-restV;

  for (size_t v=0; v<endV; v+=N) {
    for (size_t u=0; u<endU; u+=N) {
      TransposeSubBlock(fSrc+u+v*sizeU, block, nDims);
      InsertSubBlock(block,fDest+v+u*sizeV,nDims);
    }
  }

  for (size_t v=0; v<sizeV; v++) {
    size_t vv=v*sizeU;
    for (size_t u=endU; u<sizeU; u++) {
      fDest[v+u*sizeV]=fSrc[u+vv];
    }
  }

  for (size_t v=endV; v<sizeV; v++) {
    size_t vv=v*sizeU;
    for (size_t u=0; u<endU; u++) {
      fDest[v+u*sizeV]=fSrc[u+vv];
    }
  }

  delete [] block;

  return true;
}

template <class T, size_t N>
void Transpose<T,N>::TransposeRef(T const * const fSrc, T *fDest, size_t const * const nDims)
{
  for (size_t v=0, i=0; v<nDims[1]; v++) {
	for (size_t u=0; u<nDims[0]; u++,i++) {
	  fDest[v+u*nDims[1]]=fSrc[i];
	}
  }
}

}}
#endif


#ifndef __morphfilters_hpp_
#define __morphfilters_hpp_

#include <iostream>
#include <limits>

#include "../../filters/filterbase.h"
#include "../morphfilters.h"

namespace kipl { namespace morphology {
template<typename T>
void ErodeInnerLoop(T const * const src, T * dest, T value, size_t N)
{
    (void)value;
	for (size_t j=0; j<N; j++) {
		dest[j]=min(dest[j],src[j]);
	} 

}

template<typename T>
void DilateInnerLoop(T const * const src, T * dest, T value, size_t N)
{
	for (size_t j=0; j<N; j++) {
		dest[j]=max(dest[j],src[j]);
	} 

}
//
//template<>
//void ErodeInnerLoop(float const * const src, float * dest, float value, size_t N)
//{
//	for (size_t j=0; j<N; j++) {
//		dest[j]=min(dest[j],src[j]);
//	} 
//
//}

template <typename T, size_t nDims>
TErode<T,nDims>::TErode(const std::vector<T> & kernel,
        const std::vector<size_t> & kDims)
			: kipl::filters::TFilterBase<T,nDims>(kernel, kDims)
{

}

template <typename T, size_t nDims>
void TErode<T,nDims>::InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest)
{
    (void)src;
    dest=std::numeric_limits<T>::max();
	//memcpy(pRes,img+nCenter,len*sizeof(float));	
}

template <typename T, size_t nDims>
TErode<T,nDims>::~TErode(void)
{
}

template <typename T, size_t nDims>
void TErode<T,nDims>::InnerLoop(T const * const src, T *dest, T value, size_t N)
{
	ErodeInnerLoop(src,dest,value,N);
}

// Dilation
template <typename T, size_t nDims>
TDilate<T,nDims>::TDilate(const std::vector<T> &kernel,
            const std::vector<size_t> &kDims)
			: kipl::filters::TFilterBase<T,nDims>(kernel, kDims)
{

}

template <typename T, size_t nDims>
void TDilate<T,nDims>::InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest)
{
	dest=-std::numeric_limits<T>::max();
	
}

template <typename T, size_t nDims>
TDilate<T,nDims>::~TDilate(void)
{
}

template <typename T, size_t nDims>
void TDilate<T,nDims>::InnerLoop(T const * const src, T *dest, T value, size_t N)
{
	DilateInnerLoop(src,dest,value,N);
}
}}

#endif

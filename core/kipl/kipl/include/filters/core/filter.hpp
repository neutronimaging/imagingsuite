//<LICENCE>

#ifndef FILTER_HPP_
#define FILTER_HPP_
#include "../filter.h"

namespace kipl { namespace filters {

template<typename T>
void FilterInnerLoop(T const * const src, T * dest, T value, size_t N)
{
	for (size_t j=0; j<N; j++) {
		dest[j]+=src[j]*value;
	} 
}

template <typename T, size_t nDims>
TFilter<T,nDims>::TFilter(const std::vector<T> & kernel, const std::vector<size_t> & kDims)
	: kipl::filters::TFilterBase<T,nDims>(kernel,kDims)
{}
	
template <typename T, size_t nDims>
TFilter<T,nDims>::~TFilter(void)
{}

template <typename T, size_t nDims>
void TFilter<T,nDims>::InnerLoop(T const * const src, T *dest, T value, size_t N)
{
	FilterInnerLoop(src,dest,value,N);	
}

template <typename T, size_t nDims>
void TFilter<T,nDims>::InitResultArray(const kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest)
{
    dest.resize(src.dims());
	dest=static_cast<T>(0);
}


}}

#endif /*FILTER_HPP_*/

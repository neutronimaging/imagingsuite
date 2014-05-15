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

#ifndef IMAGEOPERATORS_H_
#define IMAGEOPERATORS_H_

#include "timage.h"

namespace kipl { namespace base {

template <typename T, size_t NDims>
T min(const kipl::base::TImage<T,NDims> img)
{
	const size_t N=img.Size();
	T const * const pImg=img.GetDataPtr();
	T val=pImg[0];
	for (size_t i=1; i<N; i++) {
		if (pImg[i]<val)
			val=pImg[i];
	}
	
	return val;
}

template <typename T, size_t NDims>
T max(const kipl::base::TImage<T,NDims> img)
{
	const size_t N=img.Size();
	T const * const pImg=img.GetDataPtr();
	T val=pImg[0];
	for (size_t i=1; i<N; i++) {
		if (val<pImg[i])
			val=pImg[i];
	}
	
	return val;
}

template <typename T, size_t NDims>
double sum(const kipl::base::TImage<T,NDims> img)
{
	const size_t N=img.Size();
	T const * const pImg=img.GetDataPtr();
	double sum=static_cast<double>(pImg[0]);
	for (size_t i=1; i<N; i++) {
		sum+=static_cast<double>(pImg[i]);
	}
	
	return static_cast<T>(sum);
}


template <typename T, size_t NDims>
double mean(const kipl::base::TImage<T,NDims> img)
{	
	return sum(img)/img.Size();
}

template <typename T, size_t NDims>
size_t count_NaN(const kipl::base::TImage<T,NDims> img)
{
	T const * const pData=img.GetDataPtr();
	size_t cnt=0L;
	
	for (size_t i=0L; i<img.Size(); i++) {
		cnt+=isnan(pData[i]);
	}
	
	return cnt;
}

template <typename T, size_t NDims>
size_t count_Inf(const kipl::base::TImage<T,NDims> img)
{
	T const * const pData=img.GetDataPtr();
	size_t cnt=0L;
	
	for (size_t i=0L; i<img.Size(); i++) {
		cnt+=isinf(pData[i]);
	}
	
	return cnt;
}

}}
#endif /*IMAGEOPERATORS_H_*/

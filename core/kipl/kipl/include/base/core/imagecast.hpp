//<LICENCE>

#ifndef _IMAGECAST_HPP_
#define _IMAGECAST_HPP_

#include <limits>

#include "../../logging/logger.h"


namespace kipl { namespace base {

template <typename T1, typename T2 , size_t N>
TImage<T1,N> ImageCaster<T1,T2,N>::cast(const TImage<T2,N> & img)
{
//	if (typeid(T1)==typeid(T2)) {
//		kipl::logging::Logger logger("ImageCaster",std::cout);
//		logger(kipl::logging::Logger::LogWarning,"Casting same type, use a copy operator instead");
//	}

	const size_t nData=img.Size();

	TImage<T1,N> res(img.Dims());
	const T2 * pImg=img.GetDataPtr();
	T1 * pRes=res.GetDataPtr();
	
	for (size_t i=0; i<nData; i++)
		pRes[i]=static_cast<T1>(pImg[i]);

	res.info=img.info;
	return res;
}

template <typename T1, typename T2 , size_t N>
TImage<T1,N> ImageCaster<T1,T2,N>::cast(const TImage<T2,N> & img, T2 lo, T2 hi)
{
	const size_t nData=img.Size();

	TImage<T1,N> res(img.Dims());
	const T2 * pImg=img.GetDataPtr();
	T1 * pRes=res.GetDataPtr();
	T2 val;
#undef max
	const float scale=static_cast<float>(std::numeric_limits<T1>::max())/(hi-lo);
    for (size_t i=0; i<nData; i++) {
		val=pImg[i];
		if (val<lo)
			val=lo;
		else if (hi<val)
			val=hi;
		
		pRes[i]=static_cast<T1>(scale*(val-lo));
	}

	res.info=img.info;
	return res;
	
}


}}

#endif

#ifndef _MATHFUNCTIONS_HPP_
#define _MATHFUNCTIONS_HPP_

#include "../../base/timage.h"
#include <complex>

namespace kipl { namespace math {

template <typename T, size_t N>
kipl::base::TImage<T,N> abs(kipl::base::TImage<T,N> img)
{
	kipl::base::TImage<T,N> result(img.Dims());
	
	const size_t n=img.Size();
	T * pImg=img.GetDataPtr();
	T * pResult=result.GetDataPtr();
	
	for (size_t i=0; i<n; i++)
		pResult[i]=fabs(pImg[i]);
		
	return result;
}



template <typename T, size_t N>
kipl::base::TImage<T,N> sqr(const kipl::base::TImage<T,N> img)
{
	kipl::base::TImage<T,N> res(img.Dims());
	

	T *pRes=res.GetDataPtr();
	memcpy(pRes,img.GetDataPtr(),sizeof(T)*img.Size());
	
	const size_t nData=img.Size(); 
	for (size_t i=0; i<nData; i++) {
		pRes[i]*=pRes[i];
	}
	
	return res;
}

template <typename T, size_t N>
kipl::base::TImage<T,N> sqrt(const kipl::base::TImage<T,N> img)
{
	kipl::base::TImage<T,N> res(img.Dims());
	
	T const * const pImg=img.GetDataPtr();
	T *pRes=res.GetDataPtr();
	
	const size_t nData=img.Size(); 
	for (size_t i=0; i<nData; i++) {
		pRes[i]=sqrt(pImg[i]);
	}
	
	return res;
}

template <typename T>
T Sigmoid(const T x, const T level, const T width)
{
	return static_cast<T>(1.0/(1.0+exp(-(x-level)/width)));
}

template <typename T, size_t N>
kipl::base::TImage<T,N> SigmoidWeights(kipl::base::TImage<T,N> img, const T level, const T width)
{
	kipl::base::TImage<T,N> res(img.Dims());
	
	T const * const pImg=img.GetDataPtr();
	T *pRes=res.GetDataPtr();
	
	const size_t nData=img.Size();
	const T epsilon=1e-3;
	const T lo=level-width*log(1/epsilon-1);
	const T hi=level-width*log(1/(epsilon-1)-1);
	const T invwidth=1.0/width;
	
	for (size_t i=0; i<nData; i++) {
		T val=pImg[i];
		if (val<lo)
			pRes[i]=0.0;
		else if (hi<val)
			pRes[i]=1.0;
		else
			pRes[i]=1.0/(1.0+exp((level-val)*invwidth));
	}
	
	return res;	
}

template <typename T>
double Entropy(T const * const data, const size_t N)
{
// Compute data sum
	double sum=0.0;
	for (size_t i=0; i<N; i++) {
		sum+=static_cast<double>(data[i]);
	}

// Compute the entropy
	double entropy=0.0;
	double p=0.0;
	for (size_t i=0; i<N; i++) {
		if (0<data[i]) {
			p=data[i]/sum;
			entropy-=log(p)*p;
		}
	}

	return entropy;
}




}}

#endif

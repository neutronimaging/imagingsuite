#ifndef _MATHFUNCTIONS_HPP_
#define _MATHFUNCTIONS_HPP_

#include "../../base/timage.h"
#include <complex>
#include <cmath>
#include <type_traits>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace kipl { namespace math {

template <typename T, size_t N>
kipl::base::TImage<T,N> abs(kipl::base::TImage<T,N> img)
{
    kipl::base::TImage<T,N> result(img.dims());
	
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
    kipl::base::TImage<T,N> res(img.dims());
	

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
	kipl::base::TImage<T,N> res(img.dims());
	
	T const * const pImg=img.GetDataPtr();
	T *pRes=res.GetDataPtr();
	
	const size_t nData=img.Size(); 
	for (size_t i=0; i<nData; i++) {
		pRes[i]=std::sqrt(pImg[i]);
	}
	
	return res;
}

template <typename T>
T sigmoid(const T x, const T level, const T width)
{
	return static_cast<T>(1.0/(1.0+exp(-(x-level)/width)));
}

template <typename T, size_t N>
kipl::base::TImage<T,N> sigmoidWeights(kipl::base::TImage<T,N> img, const T level, const T width)
{
	kipl::base::TImage<T,N> res(img.Dims());
	
	T const * const pImg=img.GetDataPtr();
	T *pRes=res.GetDataPtr();
	
	const size_t nData=img.Size();
	const T epsilon=1e-3;
	const T lo=level-width*log(1/epsilon-1);
	const T hi=level-width*log(1/(epsilon-1)-1);
	const T invwidth=1.0/width;
	
	for (size_t i=0; i<nData; i++) 
	{
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

// \brief Computes a weighted sum based on the sigmoid function
// \param val   - input value
// \param a     - first value
// \param b     - second value
// \param level - value for center point in the sigmoid function
// \param width - width of the sigmoid function (for w=1, 99.9% of the sigmoid are within +/-2pi)
// \returns a weighted sum of a and b based on the sigmoid function 
template <typename T>
inline T sigmoidWeights(T val, T a, T b, const float level, const float width)
{
    float w=sigmoid(val,level,width);

    return a + (b-a)*w;

}

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type>
double entropy(T const * const data, const size_t N)
{
// Compute data sum
	double sum=0.0;
	for (size_t i=0; i<N; i++) 
	{
		sum+=static_cast<double>(data[i]);
	}

// Compute the entropy
	double entropy=0.0;
	double p=0.0;
	for (size_t i=0; i<N; i++) 
	{
		if (0<data[i]) 
		{
			p=data[i]/sum;
			entropy-=std::log2(p)*p;
		}
	}

	return entropy;
}

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type>
double entropy(const std::vector<T>& vec) {
    // Avoid zero elements
    std::vector<double> nonZeroVec;
    std::copy_if(vec.begin(), vec.end(), std::back_inserter(nonZeroVec), [](T val) {
        return val != static_cast<T>(0);
    });

	if (nonZeroVec.empty()) 
		throw std::runtime_error("Entropy: All elements are zero");

    // Normalize vector elements by sum
    double sum = std::accumulate(nonZeroVec.begin(), nonZeroVec.end(), 0.0);
    std::transform(nonZeroVec.begin(), nonZeroVec.end(), nonZeroVec.begin(), [sum](T val) {
        return static_cast<double>(val) / sum;
    });

    // Calculate entropy
    double entropy = -std::accumulate(nonZeroVec.begin(), nonZeroVec.end(), 
									 0.0, 
									 [](double result, double val) 
										{
											return result + (val * std::log2(val));
										}
									);

    return entropy;
}

template <typename T, size_t N>
void replaceInfNaN(kipl::base::TImage<T,N> &img, T val)
{
    std::transform(img.GetDataPtr(), img.GetDataPtr() + img.Size(), img.GetDataPtr(),
                   [val](T pixel) -> T {
                       return std::isfinite(pixel) ? pixel : val;
                   });
}

/// \brief Factorize a number into its prime factors
/// \param n the number to factorize
/// \returns a vector containing the prime factors of n
template <typename T>
std::vector<T> factorize(T n)
{
	std::vector<T> factors;
	for (T i=2; i<=n; ++i)
	{
		while (n%i==0)
		{
			factors.push_back(i);
			n/=i;
		}
	}

	if (n!=1)
		factors.push_back(n);

	return factors;
}

}}

#endif

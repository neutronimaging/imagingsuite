#ifndef _COMPLEXIMAGE_HPP_
#define _COMPLEXIMAGE_HPP_

#include "../../base/timage.h"
#include <complex>

namespace kipl { namespace complexmath {

template <typename T, size_t N>
kipl::base::TImage<T,N> abs(const kipl::base::TImage<T,N> img)
{
	kipl::base::TImage<T,N> result(img.Dims());
	
	const size_t n=img.Size();
	const T * pImg=img.GetDataPtr();
	T * pResult=result.GetDataPtr();
	
	for (size_t i=0; i<n; i++)
		pResult[i]=abs(pImg[i]);
		
	return result;
}

template <typename T, size_t N>
kipl::base::TImage<T,N> abs(const kipl::base::TImage<std::complex<T>,N> cimg)
{
	kipl::base::TImage<T,N> result(cimg.Dims());
	
	const size_t n=cimg.Size();
	const std::complex<T> * pImg=cimg.GetDataPtr();
	T * pResult=result.GetDataPtr();
	
	for (size_t i=0; i<n; i++)
		pResult[i]=abs(pImg[i]);
		
	return result;
}

template <typename T, size_t N>
kipl::base::TImage<T,N> arg(const kipl::base::TImage<std::complex<T>,N> cimg)
{
	kipl::base::TImage<T,N> result(cimg.Dims());

	const size_t n=cimg.Size();
	const std::complex<T> * pImg=cimg.GetDataPtr();
	T * pResult=result.GetDataPtr();

	for (size_t i=0; i<n; i++)
		pResult[i]=arg(pImg[i]);

	return result;
}


template <typename T, size_t N>
kipl::base::TImage<T,N> real(const kipl::base::TImage<std::complex<T>,N> cimg)
{
	kipl::base::TImage<T,N> result(cimg.Dims());
	
	const size_t n=cimg.Size();
	const std::complex<T> * pImg=cimg.GetDataPtr();
	T * pResult=result.GetDataPtr();
	
	for (size_t i=0; i<n; i++)
		pResult[i]=pImg[i].real();
		
	return result;
}

template <typename T, size_t N>
kipl::base::TImage<T,N> imag(const kipl::base::TImage<std::complex<T>,N> cimg)
{
	kipl::base::TImage<T,N> result(cimg.Dims());
	
	const size_t n=cimg.Size();
	const std::complex<T> * pImg=cimg.GetDataPtr();
	T * pResult=result.GetDataPtr();
	
	for (size_t i=0; i<n; i++)
		pResult[i]=pImg[i].imag();
		
	return result;
}


template <typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> conj(const kipl::base::TImage<std::complex<T>,N> cimg)
{
	kipl::base::TImage<std::complex<T>,N> result=cimg;
	result.Clone();
	const size_t ndata=result.Size();
	
	std::complex<T> *pResult=result.GetDataPtr();
	
	for (size_t i=0; i<ndata; i++)
		pResult[i]._M_imag=-pResult[i]._M_imag;
		
    return result;
}

template <typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> complex(const kipl::base::TImage<T,N> &re, const kipl::base::TImage<T,N> &im)
{
	kipl::base::TImage<std::complex<T>, N> res(re.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	T const * const  pReal=re.GetDataPtr();
	T const * const  pImag=im.GetDataPtr();
	
	const size_t ndata=re.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=std::complex<T>(pReal[i],pImag[i]);
	}
	
	return res;
}

///////////////////////////////////////////////////////////
// Complex valued arithmetics
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator+(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB)
{
	kipl::base::TImage<std::complex<T>, N> res(imgA.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	T const * const  pA=imgA.GetDataPtr();
	std::complex<T> const * const  pB=imgB.GetDataPtr();
	
	const size_t ndata=imgA.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=pA[i]+pB[i];
	}
	
	return res;
}

template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator+(	const kipl::base::TImage<std::complex<T>,N> &imgA,
													const kipl::base::TImage<T,N> &imgB)
{
	return imgB+imgA;
}


template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator+(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB)
{
	kipl::base::TImage<std::complex<T>, N> res(imgA.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	std::complex<T> const * const  pA=imgA.GetDataPtr();
	std::complex<T> const * const  pB=imgB.GetDataPtr();
	
	const size_t ndata=imgA.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=pA[i]+pB[i];
	}
	
	return res;
}

template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator-(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB)
{
	kipl::base::TImage<std::complex<T>, N> res(imgA.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	T const * const  pA=imgA.GetDataPtr();
	std::complex<T> const * const  pB=imgB.GetDataPtr();
	
	const size_t ndata=imgA.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=pA[i]-pB[i];
	}
	
	return res;
}

template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator-(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB)
{
	kipl::base::TImage<std::complex<T>, N> res(imgA.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	T const * const  pB=imgB.GetDataPtr();
	std::complex<T> const * const  pA=imgA.GetDataPtr();
	
	const size_t ndata=imgA.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=pA[i]-pB[i];
	}
	
	return res;
}
													
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator-(	const kipl::base::TImage<std::complex<T>,N> &imgA,
													const kipl::base::TImage<std::complex<T>,N> &imgB)
{
	kipl::base::TImage<std::complex<T>, N> res(imgA.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	std::complex<T> const * const  pA=imgA.GetDataPtr();
	std::complex<T> const * const  pB=imgB.GetDataPtr();
	
	const size_t ndata=imgA.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=pA[i]-pB[i];
	}
	
	return res;
}


template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator*(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB)
{
	kipl::base::TImage<std::complex<T>, N> res(imgA.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	T const * const  pA=imgA.GetDataPtr();
	std::complex<T> const * const  pB=imgB.GetDataPtr();
	
	const size_t ndata=imgA.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=pA[i]*pB[i];
	}
	
	return res;
}

template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator*(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB)
{
	return imgB*imgA;
}

template<typename T, size_t N>
kipl::base::TImage<std::complex<T>, N> operator*(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB)
{
	kipl::base::TImage<std::complex<T>, N> res(imgA.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	std::complex<T> const * const  pA=imgA.GetDataPtr();
	std::complex<T> const * const  pB=imgB.GetDataPtr();
	
	const size_t ndata=imgA.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=pA[i]*pB[i];
	}
	
	return res;
}
													

template<typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> operator/(	const kipl::base::TImage<T,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB)
{
	kipl::base::TImage<std::complex<T>, N> res(imgA.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	T const * const  pA=imgA.GetDataPtr();
	std::complex<T> const * const  pB=imgB.GetDataPtr();
	
	const size_t ndata=imgA.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=pA[i]/pB[i];
	}
	
	return res;
}
													

template<typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> operator/(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<T,N> &imgB)
{
	kipl::base::TImage<std::complex<T>, N> res(imgA.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	T const * const  pB=imgB.GetDataPtr();
	std::complex<T> const * const  pA=imgA.GetDataPtr();
	
	const size_t ndata=imgA.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=pA[i]/pB[i];
	}
	
	return res;
}
template<typename T, size_t N>
kipl::base::TImage<std::complex<T>,N> operator/(	const kipl::base::TImage<std::complex<T>,N> &imgA, 
													const kipl::base::TImage<std::complex<T>,N> &imgB)
{
	kipl::base::TImage<std::complex<T>, N> res(imgA.Dims());
	
	std::complex<T> * pRes=res.GetDataPtr();
	std::complex<T> const * const  pA=imgA.GetDataPtr();
	std::complex<T> const * const  pB=imgB.GetDataPtr();
	
	const size_t ndata=imgA.Size();
	
	for (size_t i=0; i<ndata; i++) {
		pRes[i]=pA[i]/pB[i];
	}
	
	return res;
}

}}

#endif

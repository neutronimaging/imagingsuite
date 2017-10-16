//<LICENCE>

#ifndef TIMAGE_HPP_
#define TIMAGE_HPP_

#include <cstring>
#include <typeinfo>
#include <iomanip>
#ifdef _OPENMP
    #include <omp.h>
#else
    typedef int omp_int_t;
    inline omp_int_t omp_get_thread_num() { return 0;}
    inline omp_int_t omp_get_max_threads() { return 1;}
#endif

#include "imagearithmetics.h"
#include "../KiplException.h"

using namespace std;
namespace kipl { namespace base {

template<typename T, size_t N>
TImage<T,N>::TImage() : m_NData(0), m_buffer(0)
{
	memset(m_Dims,0,N*sizeof(size_t));
}
	
template<typename T, size_t N>
TImage<T,N>::TImage(const TImage<T,N> &img) : m_NData(img.m_NData), m_buffer(img.m_buffer)
{
	info=img.info;
	memcpy(m_Dims,img.m_Dims,N*sizeof(size_t));
}

template<typename T, size_t N>
TImage<T,N>::TImage(size_t const * const dims) : m_NData(_ComputeNElements(dims)), m_buffer(m_NData) 
{
	memcpy(m_Dims,dims,N*sizeof(size_t));
	memset(m_buffer.GetDataPtr(), 0, m_NData*sizeof(T));
}

template<typename T, size_t N>
TImage<T,N>::~TImage()
{
	
}
	
template<typename T, size_t N>
const TImage<T,N> & TImage<T,N>::operator=(const TImage<T,N> &img)
{
	info=img.info;
	m_buffer=img.m_buffer;
	m_NData=img.m_NData;
	memcpy(m_Dims, img.m_Dims, N*sizeof(size_t));
	
	return *this;
}

template<typename T, size_t N>
const TImage<T,N> & TImage<T,N>::operator=(const T value)
{
	if (value==static_cast<T>(0)) {
		memset(m_buffer.GetDataPtr(),0,m_buffer.Size()*sizeof(T));
	}
	else {
		T* pData=m_buffer.GetDataPtr();
		const size_t n=m_buffer.Size();
		for (size_t i=0; i<n; i++) {
			pData[i]=value;
		}
	}

	return *this;
}

template<typename T, size_t N>
void TImage<T,N>::Clone()
{
	m_buffer.Clone();
}

template<typename T, size_t N>
void TImage<T,N>::Clone(kipl::base::TImage<T,N> &img)
{
    this->Resize(img.Dims());
    std::copy(img.GetDataPtr(),img.GetDataPtr()+img.Size(),this->GetDataPtr());
}

template<typename T, size_t N>
size_t TImage<T,N>::_ComputeNElements(size_t const * const dims)
{
	size_t NData=dims[0];
	for (size_t i=1; i<N; i++)
		NData*=dims[i];
	
	return NData;
}

template<typename T, size_t N>
T & TImage<T,N>::operator()(size_t x, size_t y, size_t z)
{
    switch (N) {
        case 0: throw base::KiplException("TImage: Zero dimension images does not have any axes.",__FILE__,__LINE__); break;
        case 1: return m_buffer.GetDataPtr()[x]; break;
        case 2: return m_buffer.GetDataPtr()[x + y*m_Dims[0]]; break;
        case 3: return m_buffer.GetDataPtr()[x + y*m_Dims[0] + z*m_Dims[0]*m_Dims[1]]; break;
    default: throw base::KiplException("TImage: Image dimensions greater than 3 cannot be accessed with coordinates",__FILE__,__LINE__);
    }

    return *m_buffer.GetDataPtr();
}

template<typename T, size_t N>
size_t TImage<T,N>::Resize(size_t const * const dims) 
{
	m_buffer.Resize(_ComputeNElements(dims));
	for (size_t i=0; i<N; i++)
		m_Dims[i]=dims[i];

	m_NData=m_buffer.Size();
	return m_buffer.Size();
}

template<typename T, size_t N>
TImage<T,N> & TImage<T,N>::operator+=(TImage<T,N> &img)
{
	for (size_t i=0; i<N ; i++) 
		if (img.Size(i)!=Size(i)) 
			throw std::length_error("Image dimension mismatch for TImage<T,N>::operator+=");
	
	m_buffer.Clone();	
	
	//kipl::base::core::BasicAdd(&m_buffer[0], &img.m_buffer[0],m_buffer.Size());
	if (typeid(T)==typeid(float)) {
		kipl::base::core::SSE2Add(&m_buffer[0], &img.m_buffer[0],m_buffer.Size());
	}
	else {
		kipl::base::core::BasicAdd(&m_buffer[0], &img.m_buffer[0],m_buffer.Size());
	}
	return *this;
}

template<typename T, size_t N>
TImage<T,N> & TImage<T,N>::operator-=(TImage<T,N> &img)
{
	for (size_t i=0; i<N ; i++) 
		if (img.Size(i)!=m_Dims[i]) 
			throw std::length_error("Image dimension mismatch for TImage<T,N>::operator-=");
	
	m_buffer.Clone();	
	
	if (typeid(T)==typeid(float)) {
		kipl::base::core::SSE2Minus(&m_buffer[0], &img.m_buffer[0],m_buffer.Size());
	}
	else {
		kipl::base::core::BasicMinus(&m_buffer[0], &img.m_buffer[0],m_buffer.Size());
	}
	return *this;
}

template<typename T, size_t N>
TImage<T,N> & TImage<T,N>::operator*=(TImage<T,N> &img)
{
	for (size_t i=0; i<N ; i++) 
		if (img.Size(i)!=m_Dims[i]) 
			throw std::length_error("Image dimension mismatch for TImage<T,N>::operator*=");
	
	m_buffer.Clone();	
	
	if (typeid(T)==typeid(float)) {
		kipl::base::core::SSE2Mult(&m_buffer[0], &img.m_buffer[0],m_buffer.Size());
	}
	else {
		kipl::base::core::BasicMult(&m_buffer[0], &img.m_buffer[0],m_buffer.Size());
	}
	return *this;
}

template<typename T, size_t N>
TImage<T,N> & TImage<T,N>::operator/=(TImage<T,N> &img)
{
	for (size_t i=0; i<N ; i++) 
		if (img.Size(i)!=m_Dims[i]) 
			throw std::length_error("Image dimension mismatch for TImage<T,N>::operator/=");
	
	m_buffer.Clone();	
	
	if (typeid(T)==typeid(float)) {
		kipl::base::core::SSE2Div(&m_buffer[0], &img.m_buffer[0],m_buffer.Size());
	}
	else {
		kipl::base::core::BasicDiv(&m_buffer[0], &img.m_buffer[0],m_buffer.Size());
	}
	return *this;
}

template<typename T, size_t N>
TImage<T,N> & TImage<T,N>::operator+=(const T x)
{
	const size_t ndata=Size();
	if (x==static_cast<T>(0))
		return *this;

	m_buffer.Clone();	
	for (size_t i=0; i<ndata; i++)
		m_buffer[i]+=x;

	return *this;
}

template<typename T, size_t N>
TImage<T,N> & TImage<T,N>::operator-=(const T x)
{
	const size_t ndata=Size();
	if (x==static_cast<T>(0))
		return *this;

	m_buffer.Clone();	
	for (size_t i=0; i<ndata; i++)
		m_buffer[i]-=x;

	return *this;
}

template<typename T, size_t N>
TImage<T,N> & TImage<T,N>::operator*=(const T x)
{
	const size_t ndata=Size();
	if (x==static_cast<T>(1))
		return *this;

	if (x==static_cast<T>(0)){
		(*this)=T(0);
		return *this;
	}
	T * pData=m_buffer.GetDataPtr();
	for (size_t i=0; i<ndata; i++)
		pData[i]*=x;

	return *this;
}

template<typename T, size_t N>
TImage<T,N> & TImage<T,N>::operator/=(const T x)
{
	const ptrdiff_t ndata=Size();
	if (x==static_cast<T>(1))
		return *this;

	const T inv=1/x;
	if (inv==static_cast<T>(0))
		return *this;

	m_buffer.Clone();	

#pragma omp parallel for
	for (ptrdiff_t i=0; i<ndata; i++)
		m_buffer[i]*=inv;

	return *this;
}

template<typename T, size_t N>
TImage<T,N> operator+(TImage<T,N> &imgA, TImage<T,N> &imgB)
{
	for (size_t i=0; i<N ; i++) 
		if (imgA.Size(i)!=imgB.Size(i)) 
			throw std::length_error("Image dimension mismatch for operator+");
	
	TImage<T,N> result=imgA;
	
	result+=imgB;
	
	return result;
}

template<typename T, size_t N>
TImage<T,N> operator-(TImage<T,N> &imgA, TImage<T,N> &imgB)
{
	for (size_t i=0; i<N ; i++) 
		if (imgA.Size(i)!=imgB.Size(i)) 
			throw std::length_error("Image dimension mismatch for operator-");
	
	TImage<T,N> result=imgA;
	
	result-=imgB;
	
	return result;
}

template<typename T, size_t N>
TImage<T,N> operator*(TImage<T,N> &imgA, TImage<T,N> &imgB)
{
	for (size_t i=0; i<N ; i++) 
		if (imgA.Size(i)!=imgB.Size(i)) 
			throw std::length_error("Image dimension mismatch for operator*");
	
	TImage<T,N> result=imgA;
	
	result*=imgB;
	
	return result;
}

template<typename T, size_t N>
TImage<T,N> operator/(TImage<T,N> &imgA, TImage<T,N> &imgB)
{
	for (size_t i=0; i<N ; i++) 
		if (imgA.Size(i)!=imgB.Size(i)) 
			throw std::length_error("Image dimension mismatch for operator/");
	
	TImage<T,N> result=imgA;
	
	result/=imgB;
	
	return result;
}

template<typename T, size_t N>
std::ostream & operator<<(std::ostream &s, const TImage<T,N> &img)
{
	s<<N<<"-dim image, dimensions=["<<img.Size(0);
	for (size_t i=1; i<N; i++)
		s<<"x"<<img.Size(i);
	s<<"]";
    if ((2u<=N) && (img.Size(0)<=10u) && (img.Size(1)<=10u)) {
        for (size_t y=0; y<img.Size(1); y++) {
            s<<std::endl;
            for (size_t x=0; x<img.Size(1); x++) {
                s<<setw(6)<<img[x+img.Size(0)*y]<<" ";
            }
            s<<std::endl;
        }
    }
	
	return s;
}
template<typename T1, typename T2, size_t N>
bool CheckEqualSize(TImage<T1,N> &img1, TImage<T2,N> &img2)
{
	bool res=img1.Size(0)==img2.Size(0);

	for (size_t i=1; i<N; i++)
		res = res && (img1.Size(i)==img2.Size(i));

	return res;
}

}}
#endif /*TIMAGE_HPP_*/

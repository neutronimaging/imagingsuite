/*
 * LinearAlgebra.hpp
 *
 *  Created on: Apr 11, 2012
 *      Author: kaestner
 */

#ifndef LINEARALGEBRA_HPP_
#define LINEARALGEBRA_HPP_
#include <iomanip>
#include "../../base/KiplException.h"
#include <cstring>

namespace kipl {
namespace math {

template <typename T>
TMatrix<T>::TMatrix() :
m_nRows(0),
m_nCols(0),
m_nElements(0),
m_pData(NULL)
{}

template <typename T>
TMatrix<T>::TMatrix(int rows, int cols, T *data) :
m_nRows(0),
m_nCols(0),
m_nElements(0),
m_pData(NULL)
{
	Resize(rows,cols);
	if (data!=NULL)
		memcpy(m_pData,data,m_nElements*sizeof(T));
	else
		memset(m_pData,0,sizeof(T)*m_nElements);
}

template <typename T>
TMatrix<T>::~TMatrix()
{
	if (m_pData!=NULL)
		delete [] m_pData;
}

template <typename T>
TMatrix<T>::TMatrix(const TMatrix<T> &m) :
m_nRows(0),
m_nCols(0),
m_nElements(0),
m_pData(NULL)
{
	Resize(m.Rows(),m.Cols());

	memcpy(m_pData,m.m_pData,sizeof(T)*m_nElements);
}

template <typename T>
const TMatrix<T> & TMatrix<T>::operator=(const TMatrix<T> &m)
{
	Resize(m.Rows(),m.Cols());

	memcpy(m_pData,m.m_pData,sizeof(T)*m_nElements);
	return *this;
}

template <typename T>
const TMatrix<T> & TMatrix<T>::operator=(const T val)
{
	for (int i=0; i<m_nElements; i++) {
		m_pData[i]=val;
	}

	return *this;
}

template <typename T>
void TMatrix<T>::Resize(int rows, int cols)
{
	int nElements=rows*cols;

	if (nElements!=m_nElements) {
		if (m_pData!=NULL)
			delete [] m_pData;

		m_pData=new T[nElements];
		m_nElements=nElements;
		m_nRows=rows;
		m_nCols=cols;
	}
}

template <typename T>
TMatrix<T> TMatrix<T>::operator+(const TMatrix<T> &b)
{
	if ((Rows()!=b.Rows()) || (Cols()!=b.Cols()))
		throw kipl::base::KiplException("Matrix size mismatch",__FILE__,__LINE__);

	TMatrix<T> msum=*this;

	T * pSum=msum.ElementPtr();
	T const * const pB=b.ElementPtr();
	for (int i=0; i<msum.Elements(); i++)
		pSum[i]+=pB[i];

	return msum;
}

template <typename T>
TMatrix<T> TMatrix<T>::operator+(const T val)
{
	TMatrix<T> msum=*this;

	for (int i=0; i<msum.Elements(); i++)
		msum[0][i]+=val;

	return msum;
}

template <typename T>
TMatrix<T> TMatrix<T>::operator-(const TMatrix<T> &m)
{
	if ((m.Rows()!=m_nRows) || (m.Cols()!=m_nCols))
		throw kipl::base::KiplException("Matrix size mismatch",__FILE__,__LINE__);

	TMatrix<T> mdiff(*this);

	for (int i=0; i<mdiff.Elements(); i++)
		mdiff.m_pData[i]-=m.m_pData[i];

	return mdiff;
}

template <typename T>
TMatrix<T> TMatrix<T>::operator-(const T val)
{
	TMatrix<T> mdiff(*this);

	for (int i=0; i<mdiff.Elements(); i++)
		mdiff.m_pData[i]-=val;

	return mdiff;
}

template <typename T>
TMatrix<T> TMatrix<T>::operator*(const TMatrix<T> &m)
{
	if ((m_nCols!=m.Rows()))
		throw kipl::base::KiplException("Matrix size mismatch (cols!=rows for multiplication)",__FILE__,__LINE__);

	TMatrix<T> mprod(m_nRows,m.Cols());
	mprod=0.0f;

	for (int i=0; i<m_nRows; i++) {
		T *row=m_pData+i*m_nCols;

		T *prodrow=mprod.m_pData+i*m_nCols;
		for (int j=0; j<m_nCols; j++) { // Rows in matrix m
			for (int k=0; k<m.Cols(); k++) {
				prodrow[k]+=m[j][k]*row[j];
			}
		}
	}

	return mprod;
}

template <typename T>
TMatrix<T> TMatrix<T>::operator*(T val)
{
	TMatrix<T> mprod(*this);

	T * pElem=mprod.m_pData;
	for (int i=0; i<mprod.Elements(); i++)
		pElem[i]*=val;

	return mprod;

}

template <typename T>
TMatrix<T> TMatrix<T>::Transpose()
{
	TMatrix<T> t;

	return t;
}

template <typename T>
TMatrix<T> TMatrix<T>::Inverse()
{
	TMatrix<T> inv;

	return inv;
}

template <typename T>
T TMatrix<T>::Determinant()
{
	TMatrix<T> L,U;

	LUdecomposition(*this,L,U);

	T det=static_cast<T>(1.0);

	for (int i=0; i<m_nRows; i++) {
		det*=L[i][i];
	}

	for (int i=0; i<m_nRows; i++) {
		det*=U[i][i];
	}

	return det;
}


template<typename T>
TMatrix<T> Solve(TMatrix<T> &a, TMatrix<T> &b)
{
	TMatrix<T> x;
	return x;
}

template<typename T>
void LUdecomposition(const TMatrix<T> &a, TMatrix<T> &L,TMatrix<T> &U)
{
	if (a.Rows()!=a.Cols())
		throw kipl::base::KiplException("LUdecomp: Matrix is not square",__FILE__, __LINE__);

	U=a;


}

template<typename T>
TMatrix<T> DiagMatrix(int N, T val)
{
	TMatrix<T> d(N,N);
	d=0;
	for (int i=0; i<N; i++)
		d[i][i]=val;

	return d;
}


}}

template <typename T>
std::ostream & operator<<(std::ostream & s, kipl::math::TMatrix<T> &m)
{
	for (int r=0; r<m.Rows(); r++) {
		for (int c=0; c<m.Cols(); c++) {
			s<<std::setw(8)<<m[r][c];
		}
		s<<std::endl;
	}

	return s;
}


#endif /* LINEARALGEBRA_HPP_ */

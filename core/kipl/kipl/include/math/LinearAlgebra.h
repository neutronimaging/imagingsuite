//<LICENCE>

#ifndef LINEARALGEBRA_H_
#define LINEARALGEBRA_H_

namespace kipl {
namespace math {

template <typename T>
class TMatrix {
public:
	TMatrix();
    TMatrix(int rows, int cols, T *data=nullptr);

	~TMatrix();
	TMatrix(const TMatrix<T> &m);
	const TMatrix<T> & operator=(const TMatrix<T> &m);
	const TMatrix<T> & operator=(const T val);
	void Resize(int rows, int cols);

	T * ElementPtr() { return m_pData;}
	T const * const ElementPtr() const { return m_pData;}

	T * operator[](int row) { return m_pData+row*m_nCols;}
	T const * const operator[](int row) const { return m_pData+row*m_nCols;}

	int Rows() const {return m_nRows;}
	int Cols() const {return m_nCols;}
	int Elements() const {return m_nElements;}

	TMatrix<T> operator+(const TMatrix<T> &m);
	TMatrix<T> operator+(const T val);
	TMatrix<T> operator-(const TMatrix<T> &m);
	TMatrix<T> operator-(const T val);
	TMatrix<T> operator*(const TMatrix<T> &m);
	TMatrix<T> operator*(const T val);

	TMatrix<T> Transpose();
	TMatrix<T> Inverse();
	T Determinant();

protected:
	int m_nRows;
	int m_nCols;
	int m_nElements;

	T *m_pData;

};



template<typename T>
TMatrix<T> Solve(TMatrix<T> &a, TMatrix<T> &b);

template<typename T>
void LUdecomposition(const TMatrix<T> &a, TMatrix<T> &L,TMatrix<T> &U);

template<typename T>
TMatrix<T> DiagMatrix(int N, T val);
}}

template <typename T>
std::ostream & operator<<(std::ostream & s, kipl::math::TMatrix<T> &m);

#include "core/LinearAlgebra.hpp"
#endif /* LINEARALGEBRA_H_ */

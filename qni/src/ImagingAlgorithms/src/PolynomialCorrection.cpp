//<LICENSE>

#include <cstring>
#include <cstddef>
#include <sstream>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "../include/PolynomialCorrection.h"
#include "../include/ImagingException.h"

namespace ImagingAlgorithms {

PolynomialCorrection::PolynomialCorrection() : logger("ImagingAlgorithms::PolynomialCorrection"),
		m_nDegree(1)
{
	memset(m_fCoef,0,sizeof(float)*10);
	m_fCoef[1]=1.0f;
}

PolynomialCorrection::~PolynomialCorrection()
{
}

int PolynomialCorrection::Setup(float *coef, int order)
{
	m_nDegree = order;
	if ((m_nDegree<1) || (9<m_nDegree))
		throw ImagingException("Polynomial order is greater than 9 in PolynomialCorrection",__FILE__,__LINE__);

	memcpy(m_fCoef,coef, (m_nDegree+1)*sizeof(float));

	return 0;
}

void PolynomialCorrection::Process(float *x, float *y, size_t N, float minX, float maxX)
{
	float k=(maxX-minX)/static_cast<float>(N);

	for (size_t i=0; i<N; i++) {
		y[i]=x[i]=minX+k*static_cast<float>(i);
	}

	Process(y,N);

}

void PolynomialCorrection::Process(float *data, size_t N)
{
	//ptrdiff_t i;
	ptrdiff_t uN=static_cast<ptrdiff_t>(N);

	switch (m_nDegree)
	{
	case 1:
		#pragma omp parallel for
		for (ptrdiff_t i=0; i<uN; i++) {
			float &x=data[i];
			x=m_fCoef[1]*x+m_fCoef[0];
		}
		break;
	case 2:
		#pragma omp parallel for
		for (ptrdiff_t i=0; i<uN; i++) {
			float &x=data[i];
			x=(m_fCoef[2]*x+m_fCoef[1])*x+m_fCoef[0];
		}
		break;
	case 3:
		#pragma omp parallel for
		for (ptrdiff_t i=0; i<uN; i++) {
			float &x=data[i];
			x=((m_fCoef[3]*x+m_fCoef[2])*x
				+m_fCoef[1])*x
				+m_fCoef[0];
		}
		break;

	case 4:
		#pragma omp parallel for
		for (ptrdiff_t i=0; i<uN; i++) {
			float &x=data[i];
			x=(((m_fCoef[4]*x+m_fCoef[3])*x
				+m_fCoef[2])*x
				+m_fCoef[1])*x
				+m_fCoef[0];
		}
		break;
	case 5:
		#pragma omp parallel for
		for (ptrdiff_t i=0; i<uN; i++) {
			float &x=data[i];
			x=((((m_fCoef[5]*x+m_fCoef[4])*x
				+m_fCoef[3])*x
				+m_fCoef[2])*x
				+m_fCoef[1])*x
				+m_fCoef[0];
		}
		break;
	case 6:
		#pragma omp parallel for
		for (ptrdiff_t i=0; i<uN; i++) {
			float &x=data[i];
			x=(((((m_fCoef[6]*x+m_fCoef[5])*x
				+m_fCoef[4])*x
				+m_fCoef[3])*x
				+m_fCoef[2])*x
				+m_fCoef[1])*x
				+m_fCoef[0];
		}
		break;
	case 7:
		#pragma omp parallel for
		for (ptrdiff_t i=0; i<uN; i++) {
			float &x=data[i];
			x=((((((m_fCoef[7]*x+m_fCoef[6])*x
				+m_fCoef[5])*x
				+m_fCoef[4])*x
				+m_fCoef[3])*x
				+m_fCoef[2])*x
				+m_fCoef[1])*x
				+m_fCoef[0];
		}
		break;

	case 8:
		#pragma omp parallel for
		for (ptrdiff_t i=0; i<uN; i++) {
			float &x=data[i];
			x=(((((((m_fCoef[8]*x+m_fCoef[7])*x
				+m_fCoef[6])*x
				+m_fCoef[5])*x
				+m_fCoef[4])*x
				+m_fCoef[3])*x
				+m_fCoef[2])*x
				+m_fCoef[1])*x
				+m_fCoef[0];
		}
		break;
	case 9:
		#pragma omp parallel for
		for (ptrdiff_t i=0; i<uN; i++) {
			float &x=data[i];
			x=((((((((m_fCoef[9]*x+m_fCoef[8])*x
					+m_fCoef[7])*x
					+m_fCoef[6])*x
					+m_fCoef[5])*x
					+m_fCoef[4])*x
					+m_fCoef[3])*x
					+m_fCoef[2])*x
					+m_fCoef[1])*x
					+m_fCoef[0];
		}
		break;
	}
}
} // end namespace imagingalgorithms

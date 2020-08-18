//<LICENSE>

#include <cstring>
#include <cstddef>
#include <sstream>
#include <algorithm>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "../include/PolynomialCorrection.h"
#include "../include/ImagingException.h"

namespace ImagingAlgorithms {

PolynomialCorrection::PolynomialCorrection() : logger("ImagingAlgorithms::PolynomialCorrection"),
        m_fCoef{0.0f, 0.879f, 0.0966f, 0.0998f},
        m_nDegree(3)
{
}

PolynomialCorrection::~PolynomialCorrection()
{
}

int PolynomialCorrection::setup(float *coef, int order)
{
	m_nDegree = order;
	if ((m_nDegree<1) || (9<m_nDegree))
		throw ImagingException("Polynomial order is greater than 9 in PolynomialCorrection",__FILE__,__LINE__);

    m_fCoef.resize(order+1UL);
    std::copy_n(coef, m_nDegree+1,m_fCoef.begin());

	return 0;
}

int PolynomialCorrection::setup(const std::vector<float> &coeff)
{
    if ((coeff.size()<2) || (10<coeff.size()))
        throw ImagingException("The number of polynomial coefficients must be 2<=N<=10",__FILE__,__LINE__);
    m_fCoef=coeff;
    m_nDegree = static_cast<int>(m_fCoef.size())-1;

    return 0;
}

void PolynomialCorrection::process(float *x, float *y, size_t N, float minX, float maxX)
{
	float k=(maxX-minX)/static_cast<float>(N);

	for (size_t i=0; i<N; i++) {
		y[i]=x[i]=minX+k*static_cast<float>(i);
	}

    processInplace(y,N);

}

std::vector<float> PolynomialCorrection::process(const std::vector<float> &x)
{
    std::vector<float> y=x;

    for (auto &yval : y)
        yval=computePolynomial(yval);

    return y;
}

void PolynomialCorrection::processInplace(float *data, size_t N)
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

void PolynomialCorrection::processInplace(double *data,size_t N)
{
    for (size_t i=0; i<N; ++i)
        data[i]=static_cast<double>(computePolynomial(static_cast<float>(data[i])));
}

inline float PolynomialCorrection::computePolynomial(float x)
{
    float y=0;

    switch (m_nDegree)
    {
    case 1:
        y=m_fCoef[1]*x+m_fCoef[0];
        break;
    case 2:
        y=(m_fCoef[2]*x+m_fCoef[1])*x+m_fCoef[0];
        break;
    case 3:
        y=((m_fCoef[3]*x+m_fCoef[2])*x
                +m_fCoef[1])*x
                +m_fCoef[0];
        break;

    case 4:
        y=(((m_fCoef[4]*x+m_fCoef[3])*x
                +m_fCoef[2])*x
                +m_fCoef[1])*x
                +m_fCoef[0];
        break;
    case 5:
        y=((((m_fCoef[5]*x+m_fCoef[4])*x
                +m_fCoef[3])*x
                +m_fCoef[2])*x
                +m_fCoef[1])*x
                +m_fCoef[0];
        break;
    case 6:
        y=(((((m_fCoef[6]*x+m_fCoef[5])*x
                +m_fCoef[4])*x
                +m_fCoef[3])*x
                +m_fCoef[2])*x
                +m_fCoef[1])*x
                +m_fCoef[0];
        break;
    case 7:
        y=((((((m_fCoef[7]*x+m_fCoef[6])*x
                +m_fCoef[5])*x
                +m_fCoef[4])*x
                +m_fCoef[3])*x
                +m_fCoef[2])*x
                +m_fCoef[1])*x
                +m_fCoef[0];
        break;

    case 8:
        y=(((((((m_fCoef[8]*x+m_fCoef[7])*x
                +m_fCoef[6])*x
                +m_fCoef[5])*x
                +m_fCoef[4])*x
                +m_fCoef[3])*x
                +m_fCoef[2])*x
                +m_fCoef[1])*x
                +m_fCoef[0];

        break;
    case 9:
        y=((((((((m_fCoef[9]*x+m_fCoef[8])*x
                    +m_fCoef[7])*x
                    +m_fCoef[6])*x
                    +m_fCoef[5])*x
                    +m_fCoef[4])*x
                    +m_fCoef[3])*x
                    +m_fCoef[2])*x
                    +m_fCoef[1])*x
                    +m_fCoef[0];
        break;
    }

    return y;
}

std::vector<float> PolynomialCorrection::coefficients()
{
    return m_fCoef;
}

} // end namespace imagingalgorithms



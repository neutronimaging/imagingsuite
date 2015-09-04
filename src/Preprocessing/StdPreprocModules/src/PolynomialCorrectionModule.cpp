/*
 * PolynomialCorrection.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */
//#include "stdafx.h"
#include "../include/StdPreprocModules_global.h"
#include "../include/PolynomialCorrectionModule.h"
#include <ParameterHandling.h>
#include <ReconException.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>

PolynomialCorrection::PolynomialCorrection() :
PreprocModuleBase("PolynomialCorrection"),
    m_nDegree(1)
{
	memset(m_fCoef,0,sizeof(float)*10);
	m_fCoef[1]=1.0f;
}

PolynomialCorrection::~PolynomialCorrection() {
}


int PolynomialCorrection::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	m_nDegree = GetIntParameter(parameters,"order");
	if ((m_nDegree<1) || (9<m_nDegree))
		throw ReconException("Polynomial order is greater than 9 in PolynomialCorrection module",__FILE__,__LINE__);

	memset(m_fCoef,0,sizeof(float)*10);

	GetFloatParameterVector(parameters,"coefficents",m_fCoef,m_nDegree+1);
	
	return 0;
}

std::map<std::string, std::string> PolynomialCorrection::GetParameters()
{
	std::map<std::string, std::string> parameters;


    parameters["order"]=kipl::strings::value2string(m_nDegree);

    parameters["coefficents"]=kipl::strings::Array2String(m_fCoef,m_nDegree+1);

	return parameters;
}

bool PolynomialCorrection::SetROI(size_t *roi)
{
	return false;
}

int PolynomialCorrection::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
    ComputePolynomial(img.GetDataPtr(),img.Size());

	return 0;
}

void PolynomialCorrection::PlotPolynomial(float *x, float *y, size_t N, float minX, float maxX)
{
	float k=(maxX-minX)/static_cast<float>(N);

	for (size_t i=0; i<N; i++) {
		y[i]=x[i]=minX+k*static_cast<float>(i);
	}

	ComputePolynomial(y,N);

}

int PolynomialCorrection::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	ComputePolynomial(img.GetDataPtr(),img.Size());

	return 0;
}

void PolynomialCorrection::ComputePolynomial(float *data, size_t N)
{
	ptrdiff_t i;
	ptrdiff_t uN=static_cast<ptrdiff_t>(N);
	switch (m_nDegree)
	{
	case 1: 
		for (i=0; i<uN; i++) {
			float &x=data[i];
			x=m_fCoef[1]*x+m_fCoef[0];
		}
		break;
	case 2: 
		for (i=0; i<uN; i++) {
			float &x=data[i];
			x=(m_fCoef[2]*x+m_fCoef[1])*x+m_fCoef[0];
		}
		break;
	case 3: 
		for (i=0; i<uN; i++) {
			float &x=data[i];
			x=((m_fCoef[3]*x+m_fCoef[2])*x
				+m_fCoef[1])*x
				+m_fCoef[0];
		}
		break;

	case 4: 
		for (i=0; i<uN; i++) {
			float &x=data[i];
			x=(((m_fCoef[4]*x+m_fCoef[3])*x
				+m_fCoef[2])*x
				+m_fCoef[1])*x
				+m_fCoef[0];
		}
		break;
	case 5: 
		for (i=0; i<uN; i++) {
			float &x=data[i];
			x=((((m_fCoef[5]*x+m_fCoef[4])*x
				+m_fCoef[3])*x
				+m_fCoef[2])*x
				+m_fCoef[1])*x
				+m_fCoef[0];
		}
		break;
	case 6: 
		for (i=0; i<uN; i++) {
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
		for (i=0; i<uN; i++) {
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
		for (i=0; i<uN; i++) {
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
		for (i=0; i<uN; i++) {
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

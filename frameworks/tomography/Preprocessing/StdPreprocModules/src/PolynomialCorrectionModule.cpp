//<LICENSE>

#include "../include/StdPreprocModules_global.h"
#include "../include/PolynomialCorrectionModule.h"
#include <ParameterHandling.h>
#include <ReconException.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>

PolynomialCorrection::PolynomialCorrection() :
    PreprocModuleBase("PolynomialCorrection")
{
}

PolynomialCorrection::~PolynomialCorrection() {
}


int PolynomialCorrection::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    int m_nDegree = GetIntParameter(parameters,"order");
	if ((m_nDegree<1) || (9<m_nDegree))
		throw ReconException("Polynomial order is greater than 9 in PolynomialCorrection module",__FILE__,__LINE__);

    float *c=new float[m_nDegree+1];

    GetFloatParameterVector(parameters,"coefficents",c,m_nDegree+1);

    pc.Setup(c,m_nDegree);
	
    delete [] c;

	return 0;
}

std::map<std::string, std::string> PolynomialCorrection::GetParameters()
{
	std::map<std::string, std::string> parameters;


    parameters["order"]=kipl::strings::value2string(pc.polynomialOrder());

    parameters["coefficents"]=kipl::strings::Vector2String(pc.coefficients());

	return parameters;
}

bool PolynomialCorrection::SetROI(size_t *roi)
{
	return false;
}

void PolynomialCorrection::PlotPolynomial(float *x, float *y, size_t N, float minX, float maxX)
{
	float k=(maxX-minX)/static_cast<float>(N);

	for (size_t i=0; i<N; i++) {
		y[i]=x[i]=minX+k*static_cast<float>(i);
	}

    pc.Process(y,N);

}

int PolynomialCorrection::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
    pc.Process(img.GetDataPtr(),img.Size());

    return 0;
}

int PolynomialCorrection::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    pc.Process(img.GetDataPtr(),img.Size());

	return 0;
}


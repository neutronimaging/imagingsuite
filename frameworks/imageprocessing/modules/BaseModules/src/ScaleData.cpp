//<LICENSE>

#include "stdafx.h"
#include "ScaleData.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <math/image_statistics.h>

ScaleData::ScaleData() :
KiplProcessModuleBase("ScaleData"),
	m_fSlope(1.0f),
	m_fIntercept(0.0f),
	m_bAutoScale(false)
{

}

ScaleData::~ScaleData() {
}


int ScaleData::Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters)
{
	m_fSlope = GetFloatParameter(parameters,"slope");
	m_fIntercept = GetFloatParameter(parameters,"intercept");
	m_bAutoScale = kipl::strings::string2bool(GetStringParameter(parameters,"autoscale"));
	
	return 0;
}

std::map<std::string, std::string> ScaleData::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["slope"]     = kipl::strings::value2string(m_fSlope);
	parameters["intercept"] = kipl::strings::value2string(m_fIntercept);
	parameters["autoscale"] = kipl::strings::bool2string(m_bAutoScale); 

	return parameters;
}

int ScaleData::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	float *pImg = img.GetDataPtr();
	ptrdiff_t N=static_cast<ptrdiff_t>(img.Size());

	if (m_bAutoScale) {
		std::pair<double,double> stats=kipl::math::statistics(img.GetDataPtr(),img.Size());		

		m_fIntercept=static_cast<float>(stats.first);
		m_fSlope=static_cast<float>(stats.second);

		float invSlope=1.0f/m_fSlope;

		#pragma omp parallel
		{
			#pragma omp for
			for (ptrdiff_t i=0; i<N; i++) {
				pImg[i]=invSlope*(pImg[i]-m_fIntercept);
			}
		}

	}
	else {
		#pragma omp parallel
		{
			float *pImg = img.GetDataPtr();
			ptrdiff_t N=static_cast<ptrdiff_t>(img.Size());
		
			#pragma omp for
			for (ptrdiff_t i=0; i<N; i++) {
				pImg[i]=m_fSlope*pImg[i]+m_fIntercept;
			}
		}
	}

	return 0;
}

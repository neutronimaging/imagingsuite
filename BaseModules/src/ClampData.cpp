/*
 * DataScaler.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */
#include "stdafx.h"
#include "ClampData.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <math/image_statistics.h>

ClampData::ClampData() :
KiplProcessModuleBase("ClampData"),
	m_fMin(0.0f),
	m_fMax(1.0f)
{

}

ClampData::~ClampData() {
}


int ClampData::Configure(std::map<std::string, std::string> parameters)
{
	m_fMin = GetFloatParameter(parameters,"min");
	m_fMax = GetFloatParameter(parameters,"max");
	
	return 0;
}

std::map<std::string, std::string> ClampData::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["min"] = kipl::strings::value2string(m_fMin);
	parameters["max"] = kipl::strings::value2string(m_fMax);

	return parameters;
}

int ClampData::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	float *pImg = img.GetDataPtr();
	ptrdiff_t N=static_cast<ptrdiff_t>(img.Size());

	#pragma omp parallel
	{
		float *pImg = img.GetDataPtr();
		ptrdiff_t N=static_cast<ptrdiff_t>(img.Size());
		bool lo=false;
		bool hi=false;

		#pragma omp for
		for (ptrdiff_t i=0; i<N; i++) {
			lo=pImg[i]<m_fMin;
			hi=m_fMax<pImg[i];
			pImg[i]=lo*m_fMin + hi*m_fMax + pImg[i]*!(lo || hi);
		}
	}

	return 0;
}

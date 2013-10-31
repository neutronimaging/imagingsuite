/*
 * DataScaler.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */
#include "stdafx.h"
#include "BasicThreshold.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <ParameterHandling.h>

BasicThreshold::BasicThreshold() :
KiplProcessModuleBase("BasicThreshold"),
	m_fThreshold(0.0f)
{

}

BasicThreshold::~BasicThreshold() {
}


int BasicThreshold::Configure(std::map<std::string, std::string> parameters)
{
	m_fThreshold = GetFloatParameter(parameters,"threshold");

	return 0;
}

std::map<std::string, std::string> BasicThreshold::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["threshold"]="0.0f";

	return parameters;
}

int BasicThreshold::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	#pragma omp parallel
	{
		float *pImg = img.GetDataPtr();
		ptrdiff_t N=static_cast<ptrdiff_t>(img.Size());
	
		#pragma omp for
		for (ptrdiff_t i=0; i<N; i++) {
			pImg[i]=m_fThreshold<pImg[i];
		}
	}

	return 0;
}

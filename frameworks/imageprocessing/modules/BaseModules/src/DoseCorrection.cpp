//<LICENSE>

#include "stdafx.h"
#include "DoseCorrection.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <math/image_statistics.h>

DoseCorrection::DoseCorrection() :
KiplProcessModuleBase("DoseCorrection"),
	m_fSlope(1.0f),
	m_fIntercept(0.0f)
{
	m_nROI[0]=0;
	m_nROI[1]=0;
	m_nROI[2]=10;
	m_nROI[3]=10;
}

DoseCorrection::~DoseCorrection()
{
}

int DoseCorrection::Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters)
{
	m_fSlope = GetFloatParameter(parameters,"slope");
	m_fIntercept = GetFloatParameter(parameters,"intercept");

	GetIntParameterVector(parameters,"doseroi",m_nROI,4);
	
	return 0;
}

std::map<std::string, std::string> DoseCorrection::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["slope"]     = kipl::strings::value2string(m_fSlope);
	parameters["intercept"] = kipl::strings::value2string(m_fIntercept);
	std::ostringstream arraystring;
	arraystring<<m_nROI[0]<<" "<<m_nROI[1]<<" "<<m_nROI[2]<<" "<<m_nROI[3];
	parameters["doseroi"]   = arraystring.str(); 

	return parameters;
}

int DoseCorrection::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	ptrdiff_t N=static_cast<ptrdiff_t>(img.Size(0)*img.Size(1));
	ptrdiff_t slices=static_cast<ptrdiff_t>(img.Size(2));
	size_t height = m_nROI[3]-m_nROI[1];
	size_t width  = m_nROI[2]-m_nROI[0];
	size_t stride = img.Size(0);
	std::ostringstream msg;

	float d0=kipl::math::RegionMean(img.GetLinePtr(m_nROI[1])+m_nROI[0],width,height,stride);
	msg<<"d0="<<d0<<" from roi = ["<<m_nROI[0]<<", "<<m_nROI[1]<<", "<<m_nROI[2]<<", "<<m_nROI[3]<<"]";
	logger(kipl::logging::Logger::LogMessage,msg.str());

	//#pragma omp parallel 
	{
        float *pSlice=nullptr;
		float dose=1.0f;
	//	#pragma omp for
		for (ptrdiff_t slice=0; slice<slices; slice++) 
		{

			pSlice=img.GetLinePtr(0,slice);

			dose=kipl::math::RegionMean(pSlice+stride*m_nROI[1]+m_nROI[0],width,height,stride);	
			dose=m_fSlope*d0/dose;
			msg.str("");
			msg<<"Slice "<<slice<<" dose="<<dose;
			logger(kipl::logging::Logger::LogMessage,msg.str());

			for (ptrdiff_t i=0; i<N; i++) {
				pSlice[i]=dose*pSlice[i]+m_fIntercept;
			}
		}
	}
	
	return 0;
}

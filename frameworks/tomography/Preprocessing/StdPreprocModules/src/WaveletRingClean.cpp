/*
 * WaveletRingClean.cpp
 *
 *  Created on: Aug 9, 2011
 *      Author: anders
 */
//#include "stdafx.h"
#include "../include/StdPreprocModules_global.h"
#include "../include/WaveletRingClean.h"

#include <ParameterHandling.h>
#include <StripeFilter.h>
#include <ReconException.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <math/mathconstants.h>

WaveletRingClean::WaveletRingClean(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("WaveletRingClean",interactor),
    m_sWName("daub15"),
    m_fSigma(0.05f),
    m_nDecNum(2),
	m_bParallelProcessing(false),
	m_eCleanMethod(ImagingAlgorithms::VerticalComponentFFT)
{

}

WaveletRingClean::~WaveletRingClean() 
{
}

int WaveletRingClean::Configure(ReconConfig UNUSED(config), std::map<std::string, std::string> parameters)
{
	m_sWName              = GetStringParameter(parameters,"wname");
	m_fSigma              = GetFloatParameter(parameters,"sigma");
	m_nDecNum             = GetIntParameter(parameters,"decnum");
	m_bParallelProcessing = kipl::strings::string2bool(GetStringParameter(parameters,"parallel"));
	string2enum(GetStringParameter(parameters,"method"),m_eCleanMethod);

	return 0;
}

std::map<std::string, std::string> WaveletRingClean::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["wname"]    = m_sWName;
	parameters["sigma"]    = kipl::strings::value2string(m_fSigma);
	parameters["decnum"]   = kipl::strings::value2string(m_nDecNum);
	parameters["parallel"] = m_bParallelProcessing ? "true" : "false";
	parameters["method"]   = enum2string(m_eCleanMethod);

	return parameters;
}

bool WaveletRingClean::SetROI(size_t * UNUSED(roi))
{
	return false;
}

int WaveletRingClean::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	int retval=0;
	if (m_bParallelProcessing)
		retval=ProcessParallel(img,coeff);
	else
		retval=ProcessSingle(img,coeff);

	return retval;
}

int WaveletRingClean::ProcessSingle(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & UNUSED(coeff))
{
	size_t dims[2]={img.Size(0), img.Size(2)};
	bool fail=false;
		kipl::base::TImage<float,2> sinogram;
		ImagingAlgorithms::StripeFilter *filter=NULL;
		try {
			filter=new ImagingAlgorithms::StripeFilter(dims,m_sWName,m_nDecNum, m_fSigma);
		}
		catch (kipl::base::KiplException &e) {
				logger(kipl::logging::Logger::LogError,e.what());
				fail=true;
		}
		if (!fail) {
            const size_t N=img.Size(1);
            for (size_t j=0; (j<N) && (UpdateStatus(float(j)/N,m_sModuleName)==false); ++j)
			{
				ExtractSinogram(img,sinogram,j);

				filter->Process(sinogram);

				InsertSinogram(sinogram,img,j);
			}
			delete filter;
		}

	if (fail) {
		throw ReconException("Failed to process the projections using stripe filter.",__FILE__,__LINE__);
	}
	return 0;


}

int WaveletRingClean::ProcessParallel(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & UNUSED(coeff))
{
	size_t dims[2]={img.Size(0), img.Size(2)};
	bool fail=false;
    int N=static_cast<int>(img.Size(1));
	#pragma omp parallel
	{
		kipl::base::TImage<float,2> sinogram;
		ImagingAlgorithms::StripeFilter *filter=NULL;
		try {
			filter=new ImagingAlgorithms::StripeFilter(dims,m_sWName,m_nDecNum, m_fSigma);
		}
		catch (kipl::base::KiplException &e) {
			#pragma omp critical
			{
				logger(kipl::logging::Logger::LogError,e.what());
				fail=true;
			}
		}
		if (!fail) {
			#pragma omp for
            for (int j=0; j<N; j++)
			{
				std::cout<<"Processing sinogram "<<j<<std::endl;
				ExtractSinogram(img,sinogram,j);

				filter->Process(sinogram);

				InsertSinogram(sinogram,img,j);
			}
			delete filter;
		}
	}

	if (fail) {
		throw ReconException("Failed to process the projections using stripe filter.",__FILE__,__LINE__);
	}
	return 0;
}





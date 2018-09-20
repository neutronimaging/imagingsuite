//<LICENSE>

#include "stdafx.h"
#include "../include/KiplProcessModuleBase.h"
#include "../include/KiplFrameworkException.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string.h>

#include <base/thistogram.h>


KiplProcessModuleBase::KiplProcessModuleBase(std::string name,bool bComputeHistogram, kipl::interactors::InteractionBase *interactor) :
    ProcessModuleBase(name,interactor),
    m_Histogram(1024),
    m_bComputeHistogram(bComputeHistogram)
{
}

KiplProcessModuleBase::~KiplProcessModuleBase(void)
{
	logger(kipl::logging::Logger::LogVerbose,"Destroying module base");
}

int KiplProcessModuleBase::Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{
	int retval=ProcessModuleBase::Process(img,parameters);

	if (m_bComputeHistogram) {
		ComputeHistogram(img,m_Histogram.Size());
	}

	return retval;
}

void KiplProcessModuleBase::ComputeHistogram(kipl::base::TImage<float,3> &img, size_t N)
{
	m_Histogram.Clear();
	kipl::base::Histogram(img.GetDataPtr(), img.Size(), m_Histogram.GetY(), m_Histogram.Size(), 0.0f, 0.0f, m_Histogram.GetX());
}


int KiplProcessModuleBase::Configure(std::map<std::string, std::string> parameters)
{
    return parameters.size();
}


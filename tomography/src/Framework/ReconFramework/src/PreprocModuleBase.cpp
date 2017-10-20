//<LICENSE>

//#include "stdafx.h"
#include "../include/ReconConfig.h"
#include "../include/PreprocModuleBase.h"
#include "../include/ReconException.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string.h>


PreprocModuleBase::PreprocModuleBase(std::string name, kipl::interactors::InteractionBase *interactor) :
    ProcessModuleBase(name,interactor)
{
}

PreprocModuleBase::~PreprocModuleBase(void)
{
	logger(kipl::logging::Logger::LogVerbose,"Destroying module base");
}


bool PreprocModuleBase::SetROI(size_t * roi)
{
    return false;
}

std::string PreprocModuleBase::Version()
{
    ostringstream s;
    //s<<"PreprocModuleBase ("<<std::max(kipl::strings::VersionNumber("$Rev$"), PreprocModuleBase::SourceVersion())<<"), "<<ProcessModuleBase::Version();

    return s.str();
}

int PreprocModuleBase::Configure(std::map<std::string, std::string> parameters)
{
    return parameters.size();
}

int PreprocModuleBase::ExtractSinogram(kipl::base::TImage<float,3> &projections, kipl::base::TImage<float,2> &sinogram, size_t idx)
{
	size_t dims[2]={projections.Size(0), projections.Size(2)};

	sinogram.Resize(dims);

	for (size_t i=0; i<dims[1]; i++)
	{
		memcpy(sinogram.GetLinePtr(i),projections.GetLinePtr(idx,i),dims[0]*sizeof(float));
	}

	return 0;
}

int PreprocModuleBase::InsertSinogram(kipl::base::TImage<float,2> &sinogram, kipl::base::TImage<float,3> &projections, size_t idx)
{
	size_t const * const dims=sinogram.Dims();

	for (size_t i=0; i<dims[1]; i++)
	{
		memcpy(projections.GetLinePtr(idx,i),sinogram.GetLinePtr(i),dims[0]*sizeof(float));
	}

	return 0;
}

int PreprocModuleBase::SourceVersion()
{
	return kipl::strings::VersionNumber("$Rev$");
}


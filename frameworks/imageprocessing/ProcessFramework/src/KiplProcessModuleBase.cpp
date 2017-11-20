//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $File$
// $Date: 2011-08-17 16:22:51 +0200 (Mi, 17 Aug 2011) $
// $Rev: 1020 $
// $Id: KiplProcessModuleBase.cpp 1020 2011-08-17 14:22:51Z kaestner $
//

#include "stdafx.h"
#include "../include/KiplProcessModuleBase.h"
#include "../include/KiplFrameworkException.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string.h>

#include <base/thistogram.h>


KiplProcessModuleBase::KiplProcessModuleBase(std::string name,bool bComputeHistogram) :
	ProcessModuleBase(name),
	m_bComputeHistogram(bComputeHistogram), 
	m_Histogram(1024)
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

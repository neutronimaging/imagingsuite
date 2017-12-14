//
// This file is part of the kiptool library by Anders Kaestner
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
#include "PoreSizeMapModule.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <math/image_statistics.h>
#include <morphology/morphdist.h>
#include <porespace/poresize.h>

PoreSizeMapModule::PoreSizeMapModule() :
KiplProcessModuleBase("PoreSizeMapModule", true),
	m_bComplement(false),
	m_fMaxRadius(std::numeric_limits<float>::max())
{

}

PoreSizeMapModule::~PoreSizeMapModule()
{
}


int PoreSizeMapModule::Configure(std::map<std::string, std::string> parameters)
{
	m_bComplement = kipl::strings::string2bool(GetStringParameter(parameters,"complement"));
	m_fMaxRadius  = GetFloatParameter(parameters,"maxradius");
	return 0;
}

std::map<std::string, std::string> PoreSizeMapModule::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["complement"]    = (m_bComplement);
	parameters["maxradius"]     = kipl::strings::value2string(m_fMaxRadius);

	return parameters;
}

int PoreSizeMapModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	kipl::base::TImage<float,3> pore;

	kipl::porespace::PoreSizeMap(img, pore);

	return 0;
}

//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//

#include "stdafx.h"
#include "ModuleB.h"
#include <iostream>

ModuleB::ModuleB(void) :
	PreprocModuleBase("MoreDummies.ModuleB")
{
}

ModuleB::~ModuleB(void)
{
	logger(kipl::logging::Logger::LogVerbose,"Destroying");
}

int ModuleB::ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,float> &parameters)
{
	logger(kipl::logging::Logger::LogVerbose,"Processing");
	return 42;
}

int ModuleB::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{

	return 0;
}


std::map<std::string, std::string> ModuleB::GetParameters()
{
	std::map<std::string, std::string> parameters;

	return parameters;

}

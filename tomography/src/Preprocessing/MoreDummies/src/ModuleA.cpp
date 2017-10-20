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
#include "ModuleA.h"
#include <iostream>

ModuleA::ModuleA(void) :
	PreprocModuleBase("MoreDummies.ModuleA")
{
}

ModuleA::~ModuleA(void)
{
	logger(kipl::logging::Logger::LogVerbose,"Destroying");
}

int ModuleA::ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,float> &parameters)
{
	logger(kipl::logging::Logger::LogVerbose,"Processing");

	return 24;
}

int ModuleA::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{

	return 0;
}

std::map<std::string, std::string> ModuleA::GetParameters()
{
	std::map<std::string, std::string> parameters;

	return parameters;

}

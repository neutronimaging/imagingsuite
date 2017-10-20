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
#include <sstream>
#include <strings/miscstring.h>
#include <ReconException.h>

ModuleA::ModuleA(void) :
	PreprocModuleBase("ModuleA"),
		parA(0.0f),
		parB(0)
{
}

ModuleA::~ModuleA(void)
{
	logger(kipl::logging::Logger::LogVerbose,"Destroying");
}

int ModuleA::ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,float> &parameters)
{
	logger(kipl::logging::Logger::LogVerbose,"Processing");

	std::cout<<"ParA="<<parA<<", ParB="<<parB<<std::endl;

	return 1;
}

int ModuleA::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	std::ostringstream msg;
	try {
		parA=GetFloatParameter(parameters,"ParameterA");
		parB=GetIntParameter(parameters,"ParameterB");
	} 
	catch (ReconException &e) {
		msg<<"Failed to configure module\n"<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}

	return 0;
}

std::map<std::string, std::string> ModuleA::GetParameters()
{
	std::map<std::string, std::string> parameters;
	
	parameters["ParameterA"]=kipl::strings::value2string(parA);
	parameters["ParameterB"]=kipl::strings::value2string(parB);

	return parameters;
}

/*
 * SpotCleanModule.cpp
 *
 *  Created on: Jan 22, 2013
 *      Author: kaestner
 */
#include "stdafx.h"
#include "SpotCleanModule.h"

SpotCleanModule::SpotCleanModule() : logger("SpotCleanModule")
{
	// TODO Auto-generated constructor stub

}

SpotCleanModule::~SpotCleanModule() {
	// TODO Auto-generated destructor stub
}

int SpotCleanModule::Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{

	return 0;
}

int SpotCleanModule::Configure(std::map<std::string, std::string> parameters)
{
	return 0;
}

std::map<std::string, std::string> SpotCleanModule::GetParameters()
{
	std::map<std::string, std::string>  parameters;

	return parameters;
}

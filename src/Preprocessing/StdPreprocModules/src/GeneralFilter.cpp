/*
 * GeneralFilter.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */
#include "../include/StdPreprocModules_global.h"
#include "../include/GeneralFilter.h"

GeneralFilter::GeneralFilter() {

}

GeneralFilter::~GeneralFilter() {

}


int GeneralFilter::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	return 0;
}

std::map<std::string, std::string> GeneralFilter::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["type"]="box";
	parameters["size"]="3";

	return parameters;
}

bool GeneralFilter::SetROI(size_t *roi)
{
	return false;
}

int GeneralFilter::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{

	return 0;
}

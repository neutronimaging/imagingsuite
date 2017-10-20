/*
 * ReferenceImageCorrectionModule.cpp
 *
 *  Created on: Jan 22, 2013
 *      Author: kaestner
 */

#include "ReferenceImageCorrectionModule.h"

ReferenceImageCorrectionModule::ReferenceImageCorrectionModule() : logger("ReferenceImageCorrectionModule"){
	// TODO Auto-generated constructor stub

}

ReferenceImageCorrectionModule::~ReferenceImageCorrectionModule() {
	// TODO Auto-generated destructor stub
}

int ReferenceImageCorrectionModule::Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{
	return 0;
}

int ReferenceImageCorrectionModule::Configure(std::map<std::string, std::string> parameters)
{
	return 0;
}

std::map<std::string, std::string> ReferenceImageCorrectionModule::GetParameters()
{
	std::map<std::string, std::string> parameters;

	return parameters;
}

int ReferenceImageCorrectionModule::Process(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters)
{
	return 0;
}

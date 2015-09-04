/*
 * DataScaler.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */
//#include "stdafx.h"
#include "../include/DataScaler.h"
#include <ParameterHandling.h>

DataScaler::DataScaler() :
PreprocModuleBase("DataScaler"),
	fOffset(0.0f),
	fSlope(1.0f)
{

}

DataScaler::~DataScaler() {
}


int DataScaler::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	fOffset = GetFloatParameter(parameters,"offset");
	fSlope = GetFloatParameter(parameters,"slope");

	return 0;
}

std::map<std::string, std::string> DataScaler::GetParameters()
{
	std::map<std::string, std::string> parameters;

    parameters["offset"]=fOffset;
    parameters["slope"]=fSlope;

	return parameters;
}

bool DataScaler::SetROI(size_t *roi)
{
	return false;
}

int DataScaler::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
	float *pImg = img.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++) {
		pImg[i]=fSlope*pImg[i]+fOffset;
	}

	return 0;
}

int DataScaler::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	float *pImg = img.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++) {
		pImg[i]=fSlope*pImg[i]+fOffset;
	}

	return 0;
}

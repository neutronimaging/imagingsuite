/*
 * PolynomialCorrectionModule.cpp
 *
 *  Created on: Jan 22, 2013
 *      Author: kaestner
 */
#include "stdafx.h"
#include "PolynomialCorrectionModule.h"

PolynomialCorrectionModule::PolynomialCorrectionModule() : logger("PolynomialCorrectionModule")
{
	// TODO Auto-generated constructor stub

}

PolynomialCorrectionModule::~PolynomialCorrectionModule() {
	// TODO Auto-generated destructor stub
}

int PolynomialCorrectionModule::Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{

	return 0;
}

int PolynomialCorrectionModule::Configure(std::map<std::string, std::string> parameters)
{
	return 0;
}

std::map<std::string, std::string> PolynomialCorrectionModule::GetParameters()
{
	std::map<std::string, std::string>  parameters;

	return parameters;
}

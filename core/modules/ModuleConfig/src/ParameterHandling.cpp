//<LICENSE>

#include "stdafx.h"

#include <strings/string2array.h>
#include <cstdlib>
#include <map>
#include <string>
#include <sstream>
#include <cstdlib>
#include "../include/ModuleException.h"


std::string DLL_EXPORT GetStringParameter(std::map<std::string, std::string> parameters, std::string parameter)
{
	std::ostringstream msg;
	std::string str;
	str=parameters[parameter];
	if (str.empty()) {
		msg.str("");
		msg<<"Could not find parameter "<<parameter;
		throw ModuleException(msg.str(),__FILE__,__LINE__);
	}

	return str;
}

float DLL_EXPORT GetFloatParameter(std::map<std::string, std::string> parameters, std::string parameter)
{
	std::string str=GetStringParameter(parameters,parameter);

	
	return static_cast<float>(std::atof(str.c_str()));
}

int DLL_EXPORT GetIntParameter(std::map<std::string, std::string> parameters, std::string parameter)
{
	std::string str=GetStringParameter(parameters,parameter);

	return std::atoi(str.c_str());
}

int DLL_EXPORT GetFloatParameterVector(std::map<std::string, std::string> parameters, std::string parameter,float *arr, int N)
{
	std::string str=GetStringParameter(parameters,parameter);

	kipl::strings::String2Array(str,arr,N);

	return N;
}

int DLL_EXPORT GetIntParameterVector(std::map<std::string, std::string> parameters, std::string parameter,int *arr, int N)
{
	std::string str=GetStringParameter(parameters,parameter);

	kipl::strings::String2Array(str,arr,N);
	return N;
}

int DLL_EXPORT GetUIntParameterVector(std::map<std::string, std::string> parameters, std::string parameter,size_t *arr, int N)
{
	std::string str=GetStringParameter(parameters,parameter);

	kipl::strings::String2Array(str,arr,N);
	return N;
}

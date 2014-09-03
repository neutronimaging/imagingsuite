//
// This file is part of the ModuleConfig library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//

// ClassificationModules.cpp : Defines the exported functions for the DLL application.
//

#ifndef __PARAMETERHANDLING_H
#define __PARAMETERHANDLING_H

#include "ModuleConfig_global.h"

#include <map>
#include <string>

std::string MODULECONFIGSHARED_EXPORT GetStringParameter(std::map<std::string, std::string> parameters, std::string parameter);

float MODULECONFIGSHARED_EXPORT GetFloatParameter(std::map<std::string, std::string> parameters, std::string parameter);

int MODULECONFIGSHARED_EXPORT GetIntParameter(std::map<std::string, std::string> parameters, std::string parameter);

int MODULECONFIGSHARED_EXPORT GetFloatParameterVector(std::map<std::string, std::string> parameters, std::string parameter,float *arr, int N);

int MODULECONFIGSHARED_EXPORT GetIntParameterVector(std::map<std::string, std::string> parameters, std::string parameter,int *arr, int N);

int MODULECONFIGSHARED_EXPORT GetUIntParameterVector(std::map<std::string, std::string> parameters, std::string parameter,size_t *arr, int N);

#endif

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

#include "../src/stdafx.h"

#include <map>
#include <string>

std::string DLL_EXPORT GetStringParameter(std::map<std::string, std::string> parameters, std::string parameter);

float DLL_EXPORT GetFloatParameter(std::map<std::string, std::string> parameters, std::string parameter);

int DLL_EXPORT GetIntParameter(std::map<std::string, std::string> parameters, std::string parameter);

int DLL_EXPORT GetFloatParameterVector(std::map<std::string, std::string> parameters, std::string parameter,float *arr, int N);

int DLL_EXPORT GetIntParameterVector(std::map<std::string, std::string> parameters, std::string parameter,int *arr, int N);

int DLL_EXPORT GetUIntParameterVector(std::map<std::string, std::string> parameters, std::string parameter,size_t *arr, int N);

#endif

//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2011-08-09 19:55:32 +0200 (Di, 09 Aug 2011) $
// $Rev: 998 $
// $Id: StdPreprocModules.cpp 998 2011-08-09 17:55:32Z kaestner $
//

// ClassificationModules.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "QNImodules.h"

#include "SpotCleanModule.h"
#include "ReferenceImageCorrectionModule.h"
#include "PolynomialCorrectionModule.h"

#include "../include/KiplProcessModuleBase.h"

#include <cstdlib>
#include <map>
#include <list>
#include <string>
#include <cstring>


DLL_EXPORT void * GetModule(const char * application, const char * name)
{
	if (strcmp(application,"kiptool")!=0)
		return NULL;

	if (name!=NULL) {
		std::string sName=name;

		if (sName=="SpotClean")
			return new SpotCleanModule;

		if (sName=="ReferenceImageCorrection")
			return new ReferenceImageCorrectionModule;

		if (sName=="PolynomialCorrection")
			return new PolynomialCorrectionModule;

	}

	return NULL;
}

DLL_EXPORT int Destroy(const char * application, void *obj)
{
	if (strcmp(application,"kiptool")!=0)
		return -1;

	delete reinterpret_cast<KiplProcessModuleBase *>(obj);

	return 0;
}

DLL_EXPORT int LibVersion()
{
	return -1;
}

DLL_EXPORT int GetModuleList(const char * application, void *listptr)
{
	if (strcmp(application,"kiptool")!=0)
		return -1;

	std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

	SpotCleanModule scm;
	modulelist->operator []("SpotClean")=scm.GetParameters();

	ReferenceImageCorrectionModule ricm;
	modulelist->operator []("ReferenceImageCorrection")=ricm.GetParameters();

	PolynomialCorrectionModule pcm;
	modulelist->operator []("PolynomialCorrection")=pcm.GetParameters();

	return 0;
}


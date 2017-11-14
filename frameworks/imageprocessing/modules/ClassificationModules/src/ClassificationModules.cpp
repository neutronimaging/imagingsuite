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

#include "ClassificationModules_global.h"
#include <cstdlib>
#include <map>
#include <list>
#include <string>

#include "ClassificationModules.h"

#include "BasicThreshold.h"
#include "DoubleThreshold.h"
//#include "FuzzyCMeans.h"
//#include "KernelFuzzyCMeans.h"
#include "RemoveBackground.h"

#include "../include/KiplProcessModuleBase.h"




void * GetModule(const char * application, const char * name)
{
	if (strcmp(application,"kiptool")!=0)
		return NULL;

	if (name!=NULL) {
		std::string sName=name;

		if (sName=="BasicThreshold")
			return new BasicThreshold;

		if (sName=="DoubleThreshold")
			return new DoubleThreshold;

//		if (sName=="FuzzyCMeans")
//			return new FuzzyCMeans;

//		if (sName=="KernelFuzzyCMeans")
//			return new KernelFuzzyCMeans;

		if (sName=="RemoveBackground")
			return new RemoveBackground;


	}

	return NULL;
}

int Destroy(const char * application, void *obj)
{
	if (strcmp(application,"kiptool")!=0)
		return -1;

	delete reinterpret_cast<KiplProcessModuleBase *>(obj);

	return 0;
}

int LibVersion()
{
	return -1;
}

int GetModuleList(const char * application, void *listptr)
{
	if (strcmp(application,"kiptool")!=0)
		return -1;

	std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

	BasicThreshold bt;
	modulelist->operator []("BasicThreshold")=bt.GetParameters();

	BasicThreshold dt;
	modulelist->operator []("DoubleThreshold")=dt.GetParameters();

//	FuzzyCMeans fcm;
//	modulelist->operator []("FuzzyCMeans")=fcm.GetParameters();

//	KernelFuzzyCMeans kfcm;
//	modulelist->operator []("KernelFuzzyCMeans")=kfcm.GetParameters();

	RemoveBackground rb;
	modulelist->operator []("RemoveBackground")=rb.GetParameters();

	return 0;
}


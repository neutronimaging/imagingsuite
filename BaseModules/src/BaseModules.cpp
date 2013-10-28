// BaseModules.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BaseModules.h"
#include "ScaleData.h"
#include "DoseCorrection.h"
#include "ClampData.h"
#include "VolumeProject.h"

#include <KiplProcessModuleBase.h>

#include <cstdlib>
#include <map>
#include <list>
#include <string>


DLL_EXPORT void * GetModule(const char *application, const char * name)
{
	if (strcmp(application,"kiptool")!=0)
		return NULL;

	if (name!=NULL) {
		std::string sName=name;

		if (sName=="scaledata")
			return new ScaleData;
		if (sName=="DoseCorrection")
			return new DoseCorrection;
		if (sName=="ClampData")
			return new ClampData;
		if (sName=="VolumeProject")
			return new VolumeProject;
	}

	return NULL;
}

DLL_EXPORT int Destroy(const char *application, void *obj)
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

DLL_EXPORT int GetModuleList(const char *application, void *listptr)
{
	if (strcmp(application,"kiptool")!=0)
		return -1;

	std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

	ScaleData sd;
	modulelist->operator []("scaledata")=sd.GetParameters();

	DoseCorrection dc;
	modulelist->operator []("DoseCorrection")=dc.GetParameters();

	ClampData cd;
	modulelist->operator []("ClampData")=cd.GetParameters();

	VolumeProject vp;
	modulelist->operator []("VolumeProject")=vp.GetParameters();

	return 0;
}


// BaseModules.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ImageHistogram.h"

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

		if (sName=="ImageHistogram")
			return new ImageHistogram;
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

	ImageHistogram mih;
	modulelist->operator []("ImageHistogram")=mih.GetParameters();

	return 0;
}


// AdvancedFilterModules.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <sstream>
#include "PoreSizeMapModule.h"
#include <logging/logger.h>


DLL_EXPORT void * GetModule(const char *application, const char * name)
{
	if (strcmp(application,"kiptool")!=0)
		return NULL;

	if (name!=NULL) {
		std::string sName=name;

		if (sName=="PoreSizeMap")
			return new PoreSizeMapModule;
	}

	return NULL;
}

DLL_EXPORT int Destroy(const char *application, void *obj)
{

	if (strcmp(application,"kiptool")!=0)
		return -1;

	kipl::logging::Logger logger("PorespaceModules destroy");
	std::ostringstream msg;

	if (obj!=NULL) {
		KiplProcessModuleBase * module = reinterpret_cast<KiplProcessModuleBase *>(obj);
		msg<<"Destroying "<<module->ModuleName();
		logger(kipl::logging::Logger::LogMessage,msg.str());

		delete module;
	}

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

	PoreSizeMapModule psm;
	modulelist->operator []("PoreSizeMap")=psm.GetParameters();

	return 0;
}

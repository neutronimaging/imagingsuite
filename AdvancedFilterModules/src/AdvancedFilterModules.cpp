//
// This file is part of the i KIPL image processing tool by Anders Kaestner
// (c) 2008,2009,2010,2011,2012,2013 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2013-08-15 21:58:23 +0200 (Thu, 15 Aug 2013) $
// $Rev: 1481 $
//

// AdvancedFilterModules.cpp : Defines the exported functions for the DLL application.
//

#include "AdvancedFilterModules_global.h"
#include <KiplProcessModuleBase.h>
#include "AdvancedFilterModules.h"
#include <string>
#include <sstream>
#include "ISSfilterModule.h"
#include "NonLinDiffusion.h"
#include "nonlocalmeansmodule.h"
#include <logging/logger.h>


ADVANCEDFILTERMODULES_EXPORT void * GetModule(const char *application, const char * name)
{
	if (strcmp(application,"kiptool")!=0)
		return NULL;

	if (name!=NULL) {
		std::string sName=name;

		if (sName=="ISSfilter")
            return new ISSfilterModule;

        if (sName=="NonLinDiffusion")
            return new NonLinDiffusionModule;

        if (sName=="NonLocalMeansFilter")
            return new NonLocalMeansModule;

	}

	return NULL;
}

ADVANCEDFILTERMODULES_EXPORT int Destroy(const char *application, void *obj)
{

	if (strcmp(application,"kiptool")!=0)
		return -1;

	kipl::logging::Logger logger("AdvancedFilterModules destroy");
	std::ostringstream msg;

	if (obj!=NULL) {
		KiplProcessModuleBase * module = reinterpret_cast<KiplProcessModuleBase *>(obj);
		msg<<"Destroying "<<module->ModuleName();
		logger(kipl::logging::Logger::LogMessage,msg.str());

		delete module;
	}

	return 0;
}

ADVANCEDFILTERMODULES_EXPORT int LibVersion()
{
	return -1;
}

ADVANCEDFILTERMODULES_EXPORT int GetModuleList(const char *application, void *listptr)
{
	if (strcmp(application,"kiptool")!=0)
		return -1;

	std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

    ISSfilterModule iss;
	modulelist->operator []("ISSfilter")=iss.GetParameters();

    NonLinDiffusionModule nld;
    modulelist->operator []("NonLinDiffusion")=nld.GetParameters();

    NonLocalMeansModule nlm;
    modulelist->operator []("NonLocalMeansFilter")=nlm.GetParameters();
    return 0;
}

//
// This file is part of the backprojector modules of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2011-04-22 12:52:16 +0200 (Fri, 22 Apr 2011) $
// $Rev: 915 $
// $Id: StdPreprocModules.cpp 915 2011-04-22 10:52:16Z kaestner $
//
#include "stdafx.h"

#include <BackProjectorBase.h>
#include <InteractionBase.h>

#include <cstdlib>
#include <string>


DLL_EXPORT void * GetModule(const char *application, const char * name, void *vinteractor)
{
	if (strcmp(application,"muhrec"))
		return NULL;

	InteractionBase *interactor=reinterpret_cast<InteractionBase *>(vinteractor);
	if (name!=NULL) {
		std::string sName=name;

//		if (sName=="MultiProjBP")
//			return new MultiProjectionBP(interactor);

	}

	return NULL;
}

DLL_EXPORT int Destroy(const char *application,void *obj)
{
	kipl::logging::Logger logger("VectorBackProj::Destroy");
	if (strcmp(application,"muhrec"))
		return -1;

	std::ostringstream msg;
	if (obj!=NULL) {
		BackProjectorBase *module=reinterpret_cast<BackProjectorBase *>(obj);
		delete module;
	}

	return 0;
}

DLL_EXPORT int LibVersion()
{
	return -1;
}

DLL_EXPORT int GetBPModuleList(const char * application, void *listptr)
{
	if (strcmp(application,"muhrec"))
		return -1;

	std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

//	MultiProjectionBP mpbp;
//	modulelist->operator []("MultiProjBP")=mpbp.GetParameters();

	return 0;
}

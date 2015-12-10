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
//#include "stdafx.h"
#include "../include/StdBackProjectors_global.h"
#include "../include/StdBackProjectors.h"
#include <BackProjectorModuleBase.h>
#include <interactors/interactionbase.h>
#include "../include/MultiProjBP.h"
#include "../include/MultiProjBPparallel.h"
#include "../include/NNMultiProjBP.h"

#include <cstdlib>
#include <string>


STDBACKPROJECTORS_EXPORT void * GetModule(const char *application, const char * name, void *vinteractor)
{
    if (strcmp(application,"muhrecbp"))
		return NULL;

    kipl::interactors::InteractionBase *interactor=reinterpret_cast<kipl::interactors::InteractionBase *>(vinteractor);
	if (name!=NULL) {
		std::string sName=name;

		if (sName=="MultiProjBP")
			return new MultiProjectionBP(interactor);

		if (sName=="MultiProjBPparallel")
			return new MultiProjectionBPparallel(interactor);

		if (sName=="NearestNeighborBP")
			return new NearestNeighborBP(interactor);
		if (sName=="MultiProjBPparallel")
			return new MultiProjectionBPparallel(interactor);

	}

	return NULL;
}

STDBACKPROJECTORS_EXPORT int Destroy(const char *application,void *obj)
{
	kipl::logging::Logger logger("StdBackProj::Destroy");
    if (strcmp(application,"muhrecbp"))
		return -1;

	std::ostringstream msg;
	if (obj!=NULL) {
		BackProjectorModuleBase *module=reinterpret_cast<BackProjectorModuleBase *>(obj);
		delete module;
	}

	return 0;
}

STDBACKPROJECTORS_EXPORT int LibVersion()
{
	return -1;
}

STDBACKPROJECTORS_EXPORT int GetModuleList(const char * application, void *listptr)
{
    if (strcmp(application,"muhrecbp"))
		return -1;

	std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

	MultiProjectionBP mpbp;
	modulelist->operator []("MultiProjBP")=mpbp.GetParameters();

	MultiProjectionBPparallel mpbpp;
	modulelist->operator []("MultiProjBPparallel")=mpbpp.GetParameters();

	NearestNeighborBP nnbp;
	modulelist->operator []("NearestNeighborBP")=nnbp.GetParameters();

	return 0;
}

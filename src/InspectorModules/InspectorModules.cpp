//
// This file is part of the backprojector modules of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:13:06 +0200 (Mon, 21 May 2012) $
// $Rev: 1315 $
// $Id: InspectorModules.cpp 1315 2012-05-21 14:13:06Z kaestner $
//
#include "inspectormodules_global.h"
#include "InspectorModules.h"
#include "ProjectionInspector.h"
#include "SaveProjections.h"
#include "CountNANs.h"

#include <cstdlib>
#include <string>


INSPECTORMODULESSHARED_EXPORT void * GetModule(const char *application, const char * name)
{
	if (strcmp(application,"muhrec")!=0)
		return NULL;

	if (name!=NULL) {
		std::string sName=name;

		if (sName=="ProjectionInspector")
			return new ProjectionInspector;

		if (sName=="SaveProjections")
			return new SaveProjections;

		if (sName=="CountNANs")
			return new CountNANs;
	}

	return NULL;
}

INSPECTORMODULESSHARED_EXPORT int Destroy(const char *application, void *obj)
{
	if (strcmp(application,"muhrec")!=0)
		return -1;

	if (obj!=NULL)
		delete reinterpret_cast<PreprocModuleBase *>(obj);

	return 0;
}

INSPECTORMODULESSHARED_EXPORT int LibVersion()
{
	return -1;
}

INSPECTORMODULESSHARED_EXPORT int GetModuleList(const char *application, void *listptr)
{
	if (strcmp(application,"muhrec")!=0)
		return -1;

	std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

	ProjectionInspector insp;
	modulelist->operator []("ProjectionInspector")=insp.GetParameters();

	SaveProjections save;
	modulelist->operator []("SaveProjections")=save.GetParameters();

	CountNANs cn;
	modulelist->operator []("CountNANs")=cn.GetParameters();

	return 0;
}

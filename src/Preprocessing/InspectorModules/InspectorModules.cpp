//<LICENSE>

#include "inspectormodules_global.h"
#include "InspectorModules.h"
#include "ProjectionInspector.h"
#include "SaveProjections.h"
#include "CountNANs.h"
#include "getimagesize.h"

#include <cstdlib>
#include <string>


INSPECTORMODULESSHARED_EXPORT void * GetModule(const char *application, const char * name,void *vinteractor)
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

        if (sName=="GetImageSize")
            return new GetImageSize;
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

    GetImageSize gis;
    modulelist->operator []("GetImageSize")=gis.GetParameters();

	return 0;
}

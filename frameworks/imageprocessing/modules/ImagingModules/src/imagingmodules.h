//<LICENSE>
#ifndef IMAGINGMODULES_H
#define IMAGINGMODULES_H

#include "ImagingModules_global.h"


extern "C" {
IMAGINGMODULESSHARED_EXPORT void * GetModule(const char *application, const char * name, void *vinteractor);
IMAGINGMODULESSHARED_EXPORT int Destroy(const char *application, void *obj);
IMAGINGMODULESSHARED_EXPORT int LibVersion();
IMAGINGMODULESSHARED_EXPORT int GetModuleList(const char *application, void *listptr);

}

#endif

#ifndef PCAMODULES_H
#define PCAMODULES_H

#include "PCAModules_global.h"

extern "C" {
PCAMODULESSHARED_EXPORT void * GetModule(const char *application, const char * name, void * vinteractor);
PCAMODULESSHARED_EXPORT int Destroy(const char *application, void *obj);
PCAMODULESSHARED_EXPORT int LibVersion();
PCAMODULESSHARED_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif // PCAMODULES_H

//<LICENSE>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BASEMODULES_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BASEMODULES_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef BASEMODULES_H
#define BASEMODULES_H

#include "BaseModules_global.h"

extern "C" {
BASEMODULES_EXPORT void * GetModule(const char *application, const char * name, void *vinteractor);
BASEMODULES_EXPORT int Destroy(const char *application, void *obj);
BASEMODULES_EXPORT int LibVersion();
BASEMODULES_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif

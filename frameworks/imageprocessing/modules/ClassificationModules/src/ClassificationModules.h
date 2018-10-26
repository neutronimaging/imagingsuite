//<LICENSE>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CLASSIFICATIONMODULES_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CLASSIFICATIONMODULES_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef __CLASSIFICATIONMODULES_H
#define __CLASSIFICATIONMODULES_H

#include "ClassificationModules_global.h"

extern "C" {
CLASSIFICATIONMODULES_EXPORT void * GetModule(const char * application, const char * name, void *vinteractor);
CLASSIFICATIONMODULES_EXPORT int Destroy(const char * application, void *obj);
CLASSIFICATIONMODULES_EXPORT int LibVersion();
CLASSIFICATIONMODULES_EXPORT int GetModuleList(const char * application, void *listptr);
}

#endif

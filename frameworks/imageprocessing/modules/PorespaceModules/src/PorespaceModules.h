// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ADVANCEDFILTERMODULES_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ADVANCEDFILTERMODULES_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef _PORESPACEMODULES_H
#define _PORESPACEMODULES_H

#include "PorespaceModules_global.h"

extern "C" {
PORESPACEMODULES_EXPORT void * GetModule(const char *application, const char * name);
PORESPACEMODULES_EXPORT int Destroy(const char *application, void *obj);
PORESPACEMODULES_EXPORT int LibVersion();
PORESPACEMODULES_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif

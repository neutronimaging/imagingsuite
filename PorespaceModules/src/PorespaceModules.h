// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ADVANCEDFILTERMODULES_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ADVANCEDFILTERMODULES_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef _ADVANCEDFILTERMODULES_H
#define _ADVANCEDFILTERMODULES_H

#include "stdafx.h"

extern "C" {
DLL_EXPORT void * GetModule(const char *application, const char * name);
DLL_EXPORT int Destroy(const char *application, void *obj);
DLL_EXPORT int LibVersion();
DLL_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif

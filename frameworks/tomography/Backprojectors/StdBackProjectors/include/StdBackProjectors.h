// LICENSE

#ifndef _STDBACKPROJECTORS_H
#define _STDBACKPROJECTORS_H

#include "StdBackProjectors_global.h"
// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the MOREDUMMIES_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// MOREDUMMIES_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//#ifdef _MSC_VER
//#define DLL_EXPORT __declspec(dllexport)
//#include <windows.h>
//#else
//#define DLL_EXPORT
//#endif


extern "C" {
STDBACKPROJECTORS_EXPORT void * GetModule(const char * application, const char * name, void *vinteractor);
STDBACKPROJECTORS_EXPORT int Destroy(const char * application, void *obj);
STDBACKPROJECTORS_EXPORT int LibVersion();
STDBACKPROJECTORS_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif

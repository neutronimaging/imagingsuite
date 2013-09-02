// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the STDPREPROCMODULESGUI_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// STDPREPROCMODULESGUI_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef __STDPREPROCMODULESGUI_H
#define __STDPREPROCMODULESGUI_H

#ifdef _MSC_VER
#define DLL_EXPORT __declspec(dllexport)
#include <windows.h>
#else
#define DLL_EXPORT
#endif

extern "C" {
DLL_EXPORT void * GetGUIModule(const char *application, const char *name);
DLL_EXPORT int DestroyGUIModule(const char *application, void *obj);
}
#endif 

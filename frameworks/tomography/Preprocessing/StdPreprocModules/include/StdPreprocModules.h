//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//

#ifndef _STDPREPROCMODULES_H
#define _STDPREPROCMODULES_H
// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the MOREDUMMIES_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// MOREDUMMIES_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef _MSC_VER
#define DLL_EXPORT __declspec(dllexport)
#include <windows.h>
#else
#define DLL_EXPORT
#endif

extern "C" {
DLL_EXPORT void * GetModule(const char *application, const char * name,void *vinteractor);
DLL_EXPORT int Destroy(const char *application, void *obj);
DLL_EXPORT int LibVersion();
DLL_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif

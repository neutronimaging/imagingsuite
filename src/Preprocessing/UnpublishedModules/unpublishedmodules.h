#ifndef UNPUBLISHEDMODULES_H
#define UNPUBLISHEDMODULES_H

#include "unpublishedmodules_global.h"

//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-08 16:08:50 +0200 (Tue, 08 May 2012) $
// $Rev: 1113 $
// $Id: StdPreprocModules.h 1113 2012-05-08 14:08:50Z kaestner $
//
//#ifdef _MSC_VER
//#define DLL_EXPORT __declspec(dllexport)
//#include <windows.h>
//#else
//#define DLL_EXPORT
//#endif

extern "C" {
UNPUBLISHEDMODULESSHARED_EXPORT void * GetModule(const char *application, const char * name, void *vinteractor);
UNPUBLISHEDMODULESSHARED_EXPORT int Destroy(const char *application, void *obj);
UNPUBLISHEDMODULESSHARED_EXPORT int LibVersion();
UNPUBLISHEDMODULESSHARED_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif // UNPUBLISHEDMODULES_H

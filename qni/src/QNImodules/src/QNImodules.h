//
// This file is part of processing modules QNI processing library for kiptool by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2011-06-11 20:01:04 +0200 (Sa, 11 Jun 2011) $
// $Rev: 957 $
// $Id: StdPreprocModules.h 957 2011-06-11 18:01:04Z kaestner $
//

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CLASSIFICATIONMODULES_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CLASSIFICATIONMODULES_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef __QNIMODULES_H
#define __QNIMODULES_H

#include "stdafx.h"

extern "C" {
DLL_EXPORT void * GetModule(const char * application, const char * name);
DLL_EXPORT int Destroy(const char * application, void *obj);
DLL_EXPORT int LibVersion();
DLL_EXPORT int GetModuleList(const char * application, void *listptr);
}

#endif

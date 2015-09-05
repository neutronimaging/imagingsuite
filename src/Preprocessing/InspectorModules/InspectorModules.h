//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:13:06 +0200 (Mon, 21 May 2012) $
// $Rev: 1315 $
// $Id: InspectorModules.h 1315 2012-05-21 14:13:06Z kaestner $
//

#ifndef _INSPECTORMODULES_H
#define _INSPECTORMODULES_H

#include "stdafx.h"

extern "C" {
DLL_EXPORT void * GetModule(const char * application, const char * name);
DLL_EXPORT int Destroy(const char * application, void *obj);
DLL_EXPORT int LibVersion();
DLL_EXPORT int GetModuleList(const char * application, void *listptr);
}

#endif

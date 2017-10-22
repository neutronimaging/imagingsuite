//<LICENSE>

#ifndef _INSPECTORMODULES_H
#define _INSPECTORMODULES_H

#include "stdafx.h"

extern "C" {
DLL_EXPORT void * GetModule(const char * application, const char * name,void *vinteractor);
DLL_EXPORT int Destroy(const char * application, void *obj);
DLL_EXPORT int LibVersion();
DLL_EXPORT int GetModuleList(const char * application, void *listptr);
}

#endif

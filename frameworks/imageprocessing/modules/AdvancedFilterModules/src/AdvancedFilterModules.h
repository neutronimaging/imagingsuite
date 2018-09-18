//<LICENSE>

#ifndef ADVANCEDFILTERMODULES_H
#define ADVANCEDFILTERMODULES_H

#include "AdvancedFilterModules_global.h"

extern "C" {
ADVANCEDFILTERMODULES_EXPORT void * GetModule(const char *application, const char * name,void *vinteractor);
ADVANCEDFILTERMODULES_EXPORT int Destroy(const char *application, void *obj);
ADVANCEDFILTERMODULES_EXPORT int LibVersion();
ADVANCEDFILTERMODULES_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif

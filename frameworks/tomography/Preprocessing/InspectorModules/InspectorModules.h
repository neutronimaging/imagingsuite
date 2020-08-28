//<LICENSE>

#ifndef INSPECTORMODULES_H
#define INSPECTORMODULES_H

#include "inspectormodules_global.h"

extern "C" {
INSPECTORMODULESSHARED_EXPORT void * GetModule(const char * application, const char * name,void *vinteractor);
INSPECTORMODULESSHARED_EXPORT int Destroy(const char * application, void *obj);
INSPECTORMODULESSHARED_EXPORT int LibVersion();
INSPECTORMODULESSHARED_EXPORT int GetModuleList(const char * application, void *listptr);
}

#endif

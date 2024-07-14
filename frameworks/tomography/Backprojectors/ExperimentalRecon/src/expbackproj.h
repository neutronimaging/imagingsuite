//<LICENSE>

#ifndef EXPBACKPROJ_H
#define EXPBACKPROJ_H

#include "expbackproj_global.h"

extern "C" {
EXPBACKPROJSHARED_EXPORT void * GetModule(const char * application, const char * name, void *vinteractor);
EXPBACKPROJSHARED_EXPORT int Destroy(const char * application, void *obj);
EXPBACKPROJSHARED_EXPORT int LibVersion();
EXPBACKPROJSHARED_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif // EXPBACKPROJ_H

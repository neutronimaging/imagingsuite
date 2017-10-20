//<LICENSE>

#ifndef FDKBACKPROJ_H
#define FDKBACKPROJ_H

#include "fdkbackproj_global.h"

extern "C" {
FDKBACKPROJSHARED_EXPORT void * GetModule(const char * application, const char * name, void *vinteractor);
FDKBACKPROJSHARED_EXPORT int Destroy(const char * application, void *obj);
FDKBACKPROJSHARED_EXPORT int LibVersion();
FDKBACKPROJSHARED_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif // FDKBACKPROJ_H

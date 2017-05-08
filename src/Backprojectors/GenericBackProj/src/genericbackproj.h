//<LICENSE>

#ifndef GENERICBACKPROJ_H
#define GENERICBACKPROJ_H

#include "genericbackproj_global.h"

extern "C" {
GENERICBACKPROJSHARED_EXPORT void * GetModule(const char * application, const char * name, void *vinteractor);
GENERICBACKPROJSHARED_EXPORT int Destroy(const char * application, void *obj);
GENERICBACKPROJSHARED_EXPORT int LibVersion();
GENERICBACKPROJSHARED_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif // GENERICBACKPROJ_H

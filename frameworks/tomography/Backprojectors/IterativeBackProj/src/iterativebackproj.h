#ifndef ITERATIVEBACKPROJ_H
#define ITERATIVEBACKPROJ_H

#include "iterativebackproj_global.h"

extern "C" {
ITERATIVEBACKPROJSHARED_EXPORT void * GetModule(const char * application, const char * name, void *vinteractor);
ITERATIVEBACKPROJSHARED_EXPORT int Destroy(const char * application, void *obj);
ITERATIVEBACKPROJSHARED_EXPORT int LibVersion();
ITERATIVEBACKPROJSHARED_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif // ITERATIVEBACKPROJ_H

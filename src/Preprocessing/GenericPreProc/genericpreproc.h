//<LICENSE>

#ifndef GENERICPREPROC_H
#define GENERICPREPROC_H

#include "genericpreproc_global.h"

extern "C" {
GENERICPREPROCSHARED_EXPORT void * GetModule(const char *application, const char * name,void *vinteractor);

GENERICPREPROCSHARED_EXPORT int Destroy(const char *application, void *obj);

GENERICPREPROCSHARED_EXPORT int LibVersion();

GENERICPREPROCSHARED_EXPORT int GetModuleList(const char *application, void *listptr);

}

#endif // GENERICPREPROC_H

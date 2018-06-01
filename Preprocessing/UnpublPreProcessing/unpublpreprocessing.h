#ifndef UNPUBLPREPROCESSING_H
#define UNPUBLPREPROCESSING_H

#include "unpublpreprocessing_global.h"

extern "C" {
    UNPUBLPREPROCESSINGSHARED_EXPORT void * GetModule(const char *application, const char * name,void *vinteractor);
    UNPUBLPREPROCESSINGSHARED_EXPORT int Destroy(const char *application, void *obj);
    UNPUBLPREPROCESSINGSHARED_EXPORT int LibVersion();
    UNPUBLPREPROCESSINGSHARED_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif // UNPUBLPREPROCESSING_H

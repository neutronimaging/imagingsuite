#ifndef UNPUBLISHEDMODULESGUI_H
#define UNPUBLISHEDMODULESGUI_H

#include "unpublishedmodulesgui_global.h"

extern "C" {
void UNPUBLISHEDMODULESGUISHARED_EXPORT * GetGUIModule(const char *application, const char *name);
int UNPUBLISHEDMODULESGUISHARED_EXPORT DestroyGUIModule(const char *application, void *obj);
}


#endif // UNPUBLISHEDMODULESGUI_H

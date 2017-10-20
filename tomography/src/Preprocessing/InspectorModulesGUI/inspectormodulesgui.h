//<LICENSE>

#ifndef INSPECTORMODULESGUI_H
#define INSPECTORMODULESGUI_H

#include "inspectormodulesgui_global.h"

extern "C" {
void INSPECTORMODULESGUISHARED_EXPORT * GetGUIModule(const char *application, const char *name,void *);
int INSPECTORMODULESGUISHARED_EXPORT DestroyGUIModule(const char *application, void *obj);
}

#endif // INSPECTORMODULESGUI_H

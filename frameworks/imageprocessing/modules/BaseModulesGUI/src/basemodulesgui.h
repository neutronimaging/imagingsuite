//<LICENSE>

#ifndef BASEMODULESGUI_H
#define BASEMODULESGUI_H

#include "basemodulesgui_global.h"

extern "C" {
void BASEMODULESGUISHARED_EXPORT * GetGUIModule(const char *application, const char *name,void *interactor);
int BASEMODULESGUISHARED_EXPORT DestroyGUIModule(const char *application, void *obj);
}

#endif // BASEMODULESGUI_H

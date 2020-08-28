//<LICENSE>

#ifndef IMAGINGMODULESGUI_H
#define IMAGINGMODULESGUI_H

#include "imagingmodulesgui_global.h"


extern "C" {
void IMAGINGMODULESGUISHARED_EXPORT * GetGUIModule(const char *application, const char *name,void *interactor);
int IMAGINGMODULESGUISHARED_EXPORT DestroyGUIModule(const char *application, void *obj);
}

#endif // IMAGINGMODULESGUI_H

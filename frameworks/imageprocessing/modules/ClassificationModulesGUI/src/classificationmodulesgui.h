#ifndef CLASSIFICATIONMODULESGUI_H
#define CLASSIFICATIONMODULESGUI_H

#include "classificationmodulesgui_global.h"

extern "C" {
void CLASSIFICATIONMODULESGUISHARED_EXPORT * GetGUIModule(const char *application, const char *name,void *interactor);
int CLASSIFICATIONMODULESGUISHARED_EXPORT DestroyGUIModule(const char *application, void *obj);
}
#endif // CLASSIFICATIONMODULESGUI_H

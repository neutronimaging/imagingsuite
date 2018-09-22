#ifndef ADVANCEDFILTERMODULESGUI_H
#define ADVANCEDFILTERMODULESGUI_H

#include "advancedfiltermodulesgui_global.h"

extern "C" {
void ADVANCEDFILTERMODULESGUISHARED_EXPORT * GetGUIModule(const char *application, const char *name,void *interactor);
int ADVANCEDFILTERMODULESGUISHARED_EXPORT DestroyGUIModule(const char *application, void *obj);
}

#endif // ADVANCEDFILTERMODULESGUI_H

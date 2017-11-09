#ifndef UNPUBLPREPROCESSINGGUI_H
#define UNPUBLPREPROCESSINGGUI_H

#include "unpublpreprocessinggui_global.h"

class UNPUBLPREPROCESSINGGUISHARED_EXPORT UnpublPreProcessingGUI
{

public:
    UnpublPreProcessingGUI();

};

extern "C" {
void UNPUBLPREPROCESSINGGUISHARED_EXPORT * GetGUIModule(const char *application, const char *name,void *interactor);
int UNPUBLPREPROCESSINGGUISHARED_EXPORT DestroyGUIModule(const char *application, void *obj);
}

#endif // UNPUBLPREPROCESSINGGUI_H

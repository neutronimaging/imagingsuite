//<LICENSE>

#include "inspectormodulesgui.h"


#include "inspectormodulesgui_global.h"
#include "saveprojectionsdlg.h"

class ConfiguratorDialogBase;

void INSPECTORMODULESGUISHARED_EXPORT * GetGUIModule(const char * application, const char * name, void *interactor)
{
    if (strcmp(application,"muhrec")!=0)
        return nullptr;

    if (name!=nullptr) {
        std::string sName=name;

        if (sName=="SaveProjections")
            return new SaveProjectionsDlg;

    }
    return nullptr;
}

INSPECTORMODULESGUISHARED_EXPORT int DestroyGUIModule(const char * application, void *obj)
{
    if (strcmp(application,"muhrec")!=0)
        return -1;

    if (obj!=nullptr) {
        ConfiguratorDialogBase *dlg=reinterpret_cast<ConfiguratorDialogBase *>(obj);
        delete dlg;
    }

    return 0;
}

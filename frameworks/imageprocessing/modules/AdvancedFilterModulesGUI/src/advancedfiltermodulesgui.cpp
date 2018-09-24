#include "advancedfiltermodulesgui.h"

#include <string>

#include "issfilterdlg.h"

class ConfiguratorDialogBase;

ADVANCEDFILTERMODULESGUISHARED_EXPORT void *GetGUIModule(const char *application, const char *name, void *interactor)
{
    if (strcmp(application,"kipltool")!=0)
        return nullptr;

    if (name!=nullptr) {
        std::string sName=name;

        if (sName=="ISSfilter")
            return new ISSFilterDlg;
    }
    return nullptr;
}

ADVANCEDFILTERMODULESGUISHARED_EXPORT int DestroyGUIModule(const char *application, void *obj)
{
    if (strcmp(application,"kipltool")!=0)
        return -1;

    if (obj!=nullptr) {
        ConfiguratorDialogBase *dlg=reinterpret_cast<ConfiguratorDialogBase *>(obj);
        delete dlg;
    }

    return 0;
}

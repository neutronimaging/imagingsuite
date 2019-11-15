//<LICENSE>
#include "advancedfiltermodulesgui.h"

#include <string>
#include <sstream>

#include "issfilterdlg.h"
#include "nonlindiffusiondlg.h"

class ConfiguratorDialogBase;

ADVANCEDFILTERMODULESGUISHARED_EXPORT void *GetGUIModule(const char *application, const char *name, void *interactor)
{
    (void)interactor;

    kipl::logging::Logger logger("GetGUIModule");
    std::ostringstream msg;

    logger.message("Fetching Advanced Filter GUI");
    if (strcmp(application,"kiptool")!=0)
        return nullptr;

    if (name!=nullptr) {
        std::string sName=name;
        logger.message(sName);

        if (sName=="ISSfilter")
            return new ISSFilterDlg;

        if (sName=="NonLinDiffusion")
            return new NonLinDiffusionDlg;
    }
    return nullptr;
}

ADVANCEDFILTERMODULESGUISHARED_EXPORT int DestroyGUIModule(const char *application, void *obj)
{
    if (strcmp(application,"kiptool")!=0)
        return -1;

    if (obj!=nullptr) {
        ConfiguratorDialogBase *dlg=reinterpret_cast<ConfiguratorDialogBase *>(obj);
        delete dlg;
    }

    return 0;
}

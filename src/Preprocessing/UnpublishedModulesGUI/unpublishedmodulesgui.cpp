#include "unpublishedmodulesgui_global.h"
#include "unpublishedmodulesgui.h"
#include <string>
#include <cstring>

//#include "morphspotcleandlg.h"

class ConfiguratorDialogBase;

void UNPUBLISHEDMODULESGUISHARED_EXPORT * GetGUIModule(const char * application, const char * name)
{
    if (strcmp(application,"muhrec")!=0)
        return nullptr;

    if (name!=nullptr) {
        std::string sName=name;

//        if (sName=="MorphSpotClean")
//            return new MorphSpotCleanDlg;
    }
    return nullptr;
}

UNPUBLISHEDMODULESGUISHARED_EXPORT int DestroyGUIModule(const char * application, void *obj)
{
    if (strcmp(application,"muhrec")!=0)
        return -1;

    if (obj!=nullptr) {
        ConfiguratorDialogBase *dlg=reinterpret_cast<ConfiguratorDialogBase *>(obj);
        delete dlg;
    }

    return 0;
}

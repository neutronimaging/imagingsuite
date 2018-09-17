


#include "imagingmodulesgui_global.h"
#include "imagingmodulesgui.h"

#include "bblognormdlg.h"

class ConfiguratorDialogBase;

IMAGINGMODULESGUISHARED_EXPORT void *GetGUIModule(const char *application, const char *name,void *interactor)
{
//    std::cout<< "getGUIModule" << std::endl;

    if (strcmp(application,"kiptool")!=0)
        return nullptr;

    if (name!=nullptr){
        std::string sName=name;

//         std::cout<< "module name: " << sName <<  std::endl;

        if (sName=="BBLogNorm")
            return new BBLogNormDlg;
    }

    return NULL;

}

IMAGINGMODULESGUISHARED_EXPORT int DestroyGUIModule(const char *application, void *obj)
{
    if (strcmp(application,"kiptool")!=0)
        return -1;

    if (obj!=nullptr) {
        ConfiguratorDialogBase *dlg=reinterpret_cast<ConfiguratorDialogBase *>(obj);
        delete dlg;
    }

    return 0;
}

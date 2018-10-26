
//#ifndef _WINDOWS_
//#include <Windows.h>
//#include <winnt.h>
//#endif

#include "imagingmodulesgui_global.h"
#include "imagingmodulesgui.h"

#include "bblognormdlg.h"
#include "WaveletRingCleanDlg.h"

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

        if (sName=="StripeFilter")
            return new WaveletRingCleanDlg;
    }

    return nullptr;

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

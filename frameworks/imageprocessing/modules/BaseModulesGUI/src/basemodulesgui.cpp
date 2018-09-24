#include "basemodulesgui.h"
#include <string>

class ConfiguratorDialogBase;

BASEMODULESGUISHARED_EXPORT void *GetGUIModule(const char *application, const char *name, void *interactor)
{
    if (strcmp(application,"kipltool")!=0)
        return nullptr;

    if (name!=nullptr) {
        std::string sName=name;

//        if (sName=="scaledata")
//			return new ScaleDataDlg;
//		if (sName=="DoseCorrection")
//			return new DoseCorrectionDlg;
//		if (sName=="ClampData")
//			return new ClampDataDlg;
//		if (sName=="VolumeProject")
//			return new VolumeProjectDlg;
    }
    return nullptr;
}

BASEMODULESGUISHARED_EXPORT int DestroyGUIModule(const char *application, void *obj)
{
    if (strcmp(application,"kipltool")!=0)
        return -1;

    if (obj!=nullptr) {
        ConfiguratorDialogBase *dlg=reinterpret_cast<ConfiguratorDialogBase *>(obj);
        delete dlg;
    }

    return 0;
}

//<LICENSE>
#include "basemodulesgui.h"
#include <string>
#include <sstream>

#include <logging/logger.h>


#include "scaledatadlg.h"


class ConfiguratorDialogBase;

BASEMODULESGUISHARED_EXPORT void *GetGUIModule(const char *application, const char *name, void *interactor)
{
    kipl::logging::Logger logger("GetGUIModule");
    std::ostringstream msg;
    if (strcmp(application,"kiptool")!=0)
        return nullptr;

    if (name!=nullptr) {
        std::string sName=name;
        msg<<"Fetching "<<sName;
        logger.message(msg.str());

        if (sName=="scaledata")
            return new ScaleDataDlg;
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

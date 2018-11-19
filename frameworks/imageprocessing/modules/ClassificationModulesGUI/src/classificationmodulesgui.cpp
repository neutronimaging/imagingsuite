//<LICENSE>

#include <QDebug>
#include <string>
#include <sstream>

#include "basicthresholddlg.h"
#include "doublethresholddlg.h"
#include "removebackgrounddlg.h"

#include <logging/logger.h>

class ConfiguratorDialogBase;

CLASSIFICATIONMODULESGUISHARED_EXPORT void * GetGUIModule(const char *application, const char *name, void *interactor)
{
    (void)interactor;

    kipl::logging::Logger logger("GetGUIModule");
    std::ostringstream msg;

    logger.message("Fetching Classification GUI");
    if (strcmp(application,"kiptool")!=0)
        return nullptr;

    if (name!=nullptr) {
        std::string sName=name;
        logger.message(sName);

        if (sName=="BasicThreshold")
            return new BasicThresholdDlg;

        if (sName=="DoubleThreshold")
            return new DoubleThresholdDlg;

//		if (sName=="FuzzyCMeans")
//			return new FuzzyCMeans;

//		if (sName=="KernelFuzzyCMeans")
//			return new KernelFuzzyCMeans;

//        if (sName=="RemoveBackground")
//            return new RemoveBackgroundDlg;
    }
    return nullptr;
}

CLASSIFICATIONMODULESGUISHARED_EXPORT int DestroyGUIModule(const char *application, void *obj)
{
    qDebug()<<"destroy module 1 "<<application;
    if (strcmp(application,"kiptool")!=0)
        return -1;

     qDebug()<<"destroy module 2";
    if (obj!=nullptr) {
        ConfiguratorDialogBase *dlg=reinterpret_cast<ConfiguratorDialogBase *>(obj);
        delete dlg;
    }

     qDebug()<<"destroy module 3";
    return 0;
}

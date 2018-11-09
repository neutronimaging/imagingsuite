//<LICENSE>

#include "ImagingModules_global.h"

#include <cstring>
#include <string>
#include <sstream>

#include <logging/logger.h>

#include <KiplProcessModuleBase.h>

#include "imagingmodules.h"
#include "translateprojectionmodule.h"
#include "stripefiltermodule.h"
#include "bblognorm.h"
#include "morphspotcleanmodule.h"



void * GetModule(const char *application, const char * name, void *vinteractor)
{
    if (strcmp(application,"kiptool")!=0)
        return nullptr;

    kipl::interactors::InteractionBase *interactor=reinterpret_cast<kipl::interactors::InteractionBase *>(vinteractor);

    if (name!=nullptr) {
        std::string sName=name;

//        if (sName=="TranslateProjections")
//            return new TranslateProjectionModule;

        if (sName=="StripeFilter")
            return new StripeFilterModule(interactor);

        if (sName=="BBLogNorm")
            return new BBLogNorm(interactor);

        if (sName=="MorphSpotClean")
            return new MorphSpotCleanModule(interactor);
    }

    return nullptr;
}

int Destroy(const char *application, void *obj)
{

    if (strcmp(application,"kiptool")!=0)
        return -1;

    kipl::logging::Logger logger("ImagingModules destroy");
    std::ostringstream msg;

    if (obj!=nullptr) {
        KiplProcessModuleBase * module = reinterpret_cast<KiplProcessModuleBase *>(obj);
        msg<<"Destroying "<<module->ModuleName();
        logger(kipl::logging::Logger::LogMessage,msg.str());

        delete module;
    }

    return 0;
}

int LibVersion()
{
    return -1;
}

int GetModuleList(const char *application, void *listptr)
{
    if (strcmp(application,"kiptool")!=0)
        return -1;

    std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

    TranslateProjectionModule tpm;
    modulelist->operator []("TranslateProjections")=tpm.GetParameters();

    StripeFilterModule sfm;
    modulelist->operator []("StripeFilter")=sfm.GetParameters();

    MorphSpotCleanModule msc;
    modulelist->operator []("MorphSpotClean")=msc.GetParameters();

    BBLogNorm bblognorm;
    modulelist->operator []("BBLogNorm")=bblognorm.GetParameters();

    return 0;
}


// PCAModules.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <sstream>
#include "PCAFilterModule.h"
#include <logging/logger.h>


DLL_EXPORT void * GetModule(const char *application, const char * name, void *vinteractor)
{
    if (strcmp(application,"kiptool")!=0)
        return nullptr;

        kipl::interactors::InteractionBase *interactor=reinterpret_cast<kipl::interactors::InteractionBase *>(vinteractor);
    if (name!=nullptr) {
        std::string sName=name;

        if (sName=="PCAFilter")
            return new PCAFilterModule;
    }

    return nullptr;
}

DLL_EXPORT int Destroy(const char *application, void *obj)
{

    if (strcmp(application,"kiptool")!=0)
        return -1;

    kipl::logging::Logger logger("PCAModules destroy");
    std::ostringstream msg;

    if (obj!=nullptr) {
        KiplProcessModuleBase * module = reinterpret_cast<KiplProcessModuleBase *>(obj);
        msg<<"Destroying "<<module->ModuleName();
        logger(kipl::logging::Logger::LogMessage,msg.str());

        delete module;
    }

    return 0;
}

DLL_EXPORT int LibVersion()
{
    return -1;
}

DLL_EXPORT int GetModuleList(const char *application, void *listptr)
{
    if (strcmp(application,"kiptool")!=0)
        return -1;

    std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

    PCAFilterModule pfm;
    modulelist->operator []("PCAFilter")=pfm.GetParameters();

    return 0;
}

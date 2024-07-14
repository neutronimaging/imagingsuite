//<LICENSE>

#include "expbackproj.h"

#include <cstdlib>
#include <string>

#include <BackProjectorModuleBase.h>
#include <interactors/interactionbase.h>
#include "expbp.h"



void * GetModule(const char *application, const char * name, void *vinteractor)
{
    if (strcmp(application,"muhrecbp"))
        return nullptr;

    kipl::interactors::InteractionBase *interactor=reinterpret_cast<kipl::interactors::InteractionBase *>(vinteractor);
    if (name!=nullptr) {
        std::string sName=name;

        if (sName=="ExperimentalBP")
            return new ExperimentalBP(interactor);
    }

    return nullptr;
}

int Destroy(const char *application,void *obj)
{
    kipl::logging::Logger logger("ExerimentalBackProj::Destroy");
    if (strcmp(application,"muhrecbp"))
        return -1;

    std::ostringstream msg;
    if (obj!=nullptr) {
        BackProjectorModuleBase *module=reinterpret_cast<BackProjectorModuleBase *>(obj);
        delete module;
    }

    return 0;
}

int LibVersion()
{
    return -1;
}

int GetModuleList(const char * application, void *listptr)
{
    if (strcmp(application,"muhrecbp"))
        return -1;

    std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

    ExperimentalBP expbp;
    modulelist->operator []("ExperimentalBP")=expbp.GetParameters();

    return 0;
}

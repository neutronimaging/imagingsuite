//<LICENSE>

#include "genericbackproj.h"

#include <cstdlib>
#include <string>

#include <backprojectorbase.h>
#include <interactors/interactionbase.h>
#include "genericbp.h"



void * GetModule(const char *application, const char * name, void *vinteractor)
{
    if (strcmp(application,"muhrecbp"))
        return NULL;

    kipl::interactors::InteractionBase *interactor=reinterpret_cast<kipl::interactors::InteractionBase *>(vinteractor);
    if (name!=NULL) {
        std::string sName=name;

        if (sName=="GenericBP")
            return new GenericBP(interactor);
    }

    return NULL;
}

int Destroy(const char *application,void *obj)
{
    kipl::logging::Logger logger("GenericBackProj::Destroy");
    if (strcmp(application,"muhrecbp"))
        return -1;

    std::ostringstream msg;
    if (obj!=NULL) {
        BackProjectorBase *module=reinterpret_cast<BackProjectorBase *>(obj);
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

    GenericBP mpbp;
    modulelist->operator []("GenericBP")=mpbp.GetParameters();

    return 0;
}

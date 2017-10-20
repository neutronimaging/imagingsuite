#include "iterativebackproj_global.h"
#include <cstdlib>
#include <string>

#include <BackProjectorModuleBase.h>
#include <interactors/interactionbase.h>

#include "genericbp.h"
#include "iterativebackproj.h"
#include "sirtbp.h"

void * GetModule(const char *application, const char * name, void *vinteractor)
{
    if (strcmp(application,"muhrecbp"))
        return NULL;

    kipl::interactors::InteractionBase *interactor=reinterpret_cast<kipl::interactors::InteractionBase *>(vinteractor);
    if (name!=NULL) {
        std::string sName=name;

        if (sName=="GenericBP")
            return new GenericBP(interactor);

        if (sName=="SIRTbp")
            return new SIRTbp(interactor);
    }

    return NULL;
}

int Destroy(const char *application,void *obj)
{
    kipl::logging::Logger logger("IterativeBackProj::Destroy");
    if (strcmp(application,"muhrecbp"))
        return -1;

    std::ostringstream msg;
    if (obj!=NULL) {
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

    std::map<std::string, std::map<std::string, std::string> > &modules = *modulelist;

    GenericBP mpbp;
    modules["GenericBP"]=mpbp.GetParameters();

//    SIRTbp sirtbp;
//    modules["SIRTbp"]=sirtbp.GetParameters();

    return 0;
}

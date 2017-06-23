//<LICENSE>

#include "fdkbackproj_global.h"

#include <cstdlib>
#include <string>

#include <BackProjectorModuleBase.h>
#include <interactors/interactionbase.h>

#include "genericbp.h"
#include "fdkbackproj.h"
#include "fdkbp.h"
#include "fdkbp_single.h"
#include "fdkreconbase.h"

FDKBACKPROJSHARED_EXPORT void * GetModule(const char *application, const char * name, void *vinteractor)
{
    if (strcmp(application,"muhrecbp"))
        return NULL;

    kipl::interactors::InteractionBase *interactor=reinterpret_cast<kipl::interactors::InteractionBase *>(vinteractor);
    if (name!=NULL) {
        std::string sName=name;

        if (sName=="FDKbp_single")
            return new FDKbp_single(interactor);

        if (sName=="FDKbp")
            return new FDKbp(interactor);

    }

    return NULL;
}

FDKBACKPROJSHARED_EXPORT int Destroy(const char *application,void *obj)
{
    kipl::logging::Logger logger("FdkBackProj::Destroy");
    if (strcmp(application,"muhrecbp"))
        return -1;

    std::ostringstream msg;
    if (obj!=NULL) {
        BackProjectorModuleBase *module=reinterpret_cast<BackProjectorModuleBase *>(obj);
        delete module;
    }

    return 0;
}

FDKBACKPROJSHARED_EXPORT int LibVersion()
{
    return -1;
}

FDKBACKPROJSHARED_EXPORT int GetModuleList(const char * application, void *listptr)
{
    if (strcmp(application,"muhrecbp"))
        return -1;

    std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

    std::map<std::string, std::map<std::string, std::string> > &modules = *modulelist;

    FDKbp_single fdkbps;
    modules["FDKbp_single"]=fdkbps.GetParameters();

    FDKbp fdkbp;
    modules["FDKbp"]=fdkbp.GetParameters();

    return 0;
}

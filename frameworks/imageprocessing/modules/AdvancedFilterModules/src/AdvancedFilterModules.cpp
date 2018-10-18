//<LICENSE>

#include "AdvancedFilterModules_global.h"
#include <KiplProcessModuleBase.h>
#include "AdvancedFilterModules.h"
#include <string>
#include <sstream>
#include "ISSfilterModule.h"
#include "NonLinDiffusion.h"
#include "nonlocalmeansmodule.h"
#include <logging/logger.h>


ADVANCEDFILTERMODULES_EXPORT void * GetModule(const char *application, const char * name, void *vinteractor)
{
	if (strcmp(application,"kiptool")!=0)
        return nullptr;

    kipl::interactors::InteractionBase *interactor=reinterpret_cast<kipl::interactors::InteractionBase *>(vinteractor);

    if (name!=nullptr) {
		std::string sName=name;

		if (sName=="ISSfilter")
            return new ISSfilterModule(interactor);

        if (sName=="NonLinDiffusion")
            return new NonLinDiffusionModule;

//        if (sName=="NonLocalMeansFilter")
//            return new NonLocalMeansModule;
	}

    return nullptr;
}

ADVANCEDFILTERMODULES_EXPORT int Destroy(const char *application, void *obj)
{

	if (strcmp(application,"kiptool")!=0)
		return -1;

	kipl::logging::Logger logger("AdvancedFilterModules destroy");
	std::ostringstream msg;

    if (obj!=nullptr) {
		KiplProcessModuleBase * module = reinterpret_cast<KiplProcessModuleBase *>(obj);
		msg<<"Destroying "<<module->ModuleName();
		logger(kipl::logging::Logger::LogMessage,msg.str());

		delete module;
	}

	return 0;
}

ADVANCEDFILTERMODULES_EXPORT int LibVersion()
{
	return -1;
}

ADVANCEDFILTERMODULES_EXPORT int GetModuleList(const char *application, void *listptr)
{
	if (strcmp(application,"kiptool")!=0)
		return -1;

	std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

    ISSfilterModule iss;
	modulelist->operator []("ISSfilter")=iss.GetParameters();

    NonLinDiffusionModule nld;
    modulelist->operator []("NonLinDiffusion")=nld.GetParameters();

    NonLocalMeansModule nlm;
    modulelist->operator []("NonLocalMeansFilter")=nlm.GetParameters();
    return 0;
}

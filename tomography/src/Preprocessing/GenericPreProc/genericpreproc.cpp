//<LICENSE>

#include "genericpreproc.h"

#include <base/KiplException.h>
#include <ModuleException.h>
#include <ReconException.h>

#include <PreprocModuleBase.h>

#include <logging/logger.h>

#include <cstdlib>
#include <list>
#include <string>
#include <sstream>
#include <map>

#include "PreprocModule.h"

void * GetModule(const char *application, const char * name,void *vinteractor)
{
    if (strcmp(application,"muhrec")!=0)
        return nullptr;

    if (name!=nullptr) {
        std::string sName=name;

        if (sName=="PreprocModule")
            return new PreprocModule;


    }

    return nullptr;
}

int Destroy(const char * application, void *obj)
{
    if (strcmp(application,"muhrec")!=0)
        return -1;

    kipl::logging::Logger logger("StdPreprocModules::Destroy");
    std::ostringstream msg;
    std::string name="No name";
    try {
        if (obj!=nullptr) {
            PreprocModuleBase *module=reinterpret_cast<PreprocModuleBase *>(obj);
            name=module->ModuleName();
            msg<<"Destroying "<<name;
            logger(kipl::logging::Logger::LogVerbose,msg.str());
            msg.str("");
            delete module;
        }
    }
    catch (std::exception & e) {
        msg<<"Failed to destroy "<<name<<" with STL exception"<<e.what();

        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (ModuleException & e) {
        msg<<"Failed to destroy "<<name<<" with Module exception"<<e.what();

        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (ReconException & e) {
        msg<<"Failed to destroy "<<name<<" with Recon exception"<<e.what();

        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (kipl::base::KiplException & e) {
        msg<<"Failed to destroy "<<name<<" with KIPL exception"<<e.what();

        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    return 0;
}

int LibVersion()
{
    return -1;
}

int GetModuleList(const char *application, void *listptr)
{
    if (strcmp(application,"muhrec")!=0)
        return -1;

    std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

    PreprocModule pm;
    modulelist->operator []("PreprocModule")=pm.GetParameters();

    return 0;
}

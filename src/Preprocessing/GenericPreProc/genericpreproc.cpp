#include "genericpreproc.h"

//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2015-06-16 08:21:46 +0200 (Die, 16 Jun 2015) $
// $Rev: 1231 $
// $Id: StdPreprocModules.cpp 1231 2015-06-16 06:21:46Z kaestner $
//

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

void * GetModule(const char *application, const char * name)
{
    if (strcmp(application,"muhrec")!=0)
        return NULL;

    if (name!=NULL) {
        std::string sName=name;

        if (sName=="FullLogNorm")
            return new FullLogNorm;


    }

    return NULL;
}

int Destroy(const char * application, void *obj)
{
    if (strcmp(application,"muhrec")!=0)
        return -1;

    kipl::logging::Logger logger("StdPreprocModules::Destroy");
    std::ostringstream msg;
    std::string name="No name";
    try {
        if (obj!=NULL) {
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

    FullLogNorm norm;
    modulelist->operator []("FullLogNorm")=norm.GetParameters();

    return 0;
}

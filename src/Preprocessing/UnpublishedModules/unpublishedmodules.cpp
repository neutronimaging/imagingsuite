//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: carminati_c $
// $Date: 2016-11-21 11:06:38 +0100 (Mon, 21 Nov 2016) $
// $Rev: 1340 $
// $Id: StdPreprocModules.cpp 1340 2016-11-21 10:06:38Z carminati_c $
//
#include "unpublishedmodules_global.h"
#include "unpublishedmodules.h"
#include <cstdlib>
#include <list>
#include <string>
#include <sstream>
#include <map>

//#include "../include/MorphSpotCleanModule.h"
//#include "../include/robustlognorm.h"

//#include <base/KiplException.h>
//#include <ModuleException.h>
//#include <ReconException.h>

//#include <PreprocModuleBase.h>
//#include <logging/logger.h>


//UNPUBLISHEDMODULESSHARED_EXPORT void * GetModule(const char *application, const char * name,void *vinteractor)
//{
//    if (strcmp(application,"muhrec")!=0)
//        return nullptr;

//    if (name!=nullptr) {
//        std::string sName=name;

//        if (sName=="RobustLogNorm")
//            return new RobustLogNorm;

//        if (sName=="MorphSpotClean")
//            return new MorphSpotCleanModule;
//    }

//    return nullptr;
//}

//UNPUBLISHEDMODULESSHARED_EXPORT int Destroy(const char * application, void *obj)
//{
//    if (strcmp(application,"muhrec")!=0)
//        return -1;

//    kipl::logging::Logger logger("StdPreprocModules::Destroy");
//    std::ostringstream msg;
//    std::string name="No name";
//    try {
//        if (obj!=nullptr) {
//            PreprocModuleBase *module=reinterpret_cast<PreprocModuleBase *>(obj);
//            name=module->ModuleName();
//            msg<<"Destroying "<<name;
//            logger(kipl::logging::Logger::LogVerbose,msg.str());
//            msg.str("");
//            delete module;
//        }
//    }
//    catch (std::exception & e) {
//        msg<<"Failed to destroy "<<name<<" with STL exception"<<e.what();

//        logger(kipl::logging::Logger::LogError,msg.str());
//        return -1;
//    }
//    catch (ModuleException & e) {
//        msg<<"Failed to destroy "<<name<<" with Module exception"<<e.what();

//        logger(kipl::logging::Logger::LogError,msg.str());
//        return -1;
//    }
//    catch (ReconException & e) {
//        msg<<"Failed to destroy "<<name<<" with Recon exception"<<e.what();

//        logger(kipl::logging::Logger::LogError,msg.str());
//        return -1;
//    }
//    catch (kipl::base::KiplException & e) {
//        msg<<"Failed to destroy "<<name<<" with KIPL exception"<<e.what();

//        logger(kipl::logging::Logger::LogError,msg.str());
//        return -1;
//    }
//    return 0;
//}

//UNPUBLISHEDMODULESSHARED_EXPORT int LibVersion()
//{
//    return -1;
//}

//UNPUBLISHEDMODULESSHARED_EXPORT int GetModuleList(const char *application, void *listptr)
//{
//    if (strcmp(application,"muhrec")!=0)
//        return -1;

//    std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

//    RobustLogNorm robnorm;
//    modulelist->operator[]("RobustLogNorm")=robnorm.GetParameters();

//    MorphSpotCleanModule mscm;
//    modulelist->operator []("MorphSpotClean")=mscm.GetParameters();

//    return 0;
//}

//
// This file is part of the i KIPL image processing tool by Anders Kaestner
// (c) 2008,2009,2010,2011,2012,2013 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2013-08-15 21:58:23 +0200 (Thu, 15 Aug 2013) $
// $Rev: 1481 $
//

#include "ImagingModules_global.h"

#include <cstring>
#include <string>
#include <sstream>

#include <logging/logger.h>

#include <KiplProcessModuleBase.h>

#include "imagingmodules.h"
#include "translateprojectionmodule.h"
#include "stripefiltermodule.h"



void * GetModule(const char *application, const char * name)
{
    if (strcmp(application,"kiptool")!=0)
        return NULL;

    if (name!=NULL) {
        std::string sName=name;

        if (sName=="TranslateProjections")
            return new TranslateProjectionModule;

        if (sName=="StripeFilter")
            return new StripeFilterModule;
    }

    return NULL;
}

int Destroy(const char *application, void *obj)
{

    if (strcmp(application,"kiptool")!=0)
        return -1;

    kipl::logging::Logger logger("ImagingModules destroy");
    std::ostringstream msg;

    if (obj!=NULL) {
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

    return 0;
}


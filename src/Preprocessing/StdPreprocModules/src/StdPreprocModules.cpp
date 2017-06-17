//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//
#include "stdafx.h"
#include "../include/StdPreprocModules_global.h"
#include "../include/NormPlugins.h"
#include "../include/SpotClean2.h"
#include "../include/StdPreprocModules.h"
#include "../include/PreprocEnums.h"
#include "../include/SpotClean.h"
#include "../include/TranslatedProjectionWeighting.h"
#include "../include/ProjectionFilter.h"
#include "../include/GeneralFilter.h"
#include "../include/AdaptiveFilter.h"
#include "../include/BasicRingClean.h"
#include "../include/DataScaler.h"
#include "../include/MedianMixRingClean.h"
#include "../include/ISSfilter.h"
#include "../include/WaveletRingClean.h"
#include "../include/PolynomialCorrectionModule.h"
#include "../include/SinoSpotClean.h"
#include "../include/TranslateProjectionsModule.h"
#include "../include/MorphSpotCleanModule.h"
#include "../include/GammaSpotCleanModule.h"
#include "../include/CameraStripeClean.h"
#include "../include/robustlognorm.h"

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

STDPREPROCMODULESSHARED_EXPORT void * GetModule(const char *application, const char * name, void *vinteractor)
{
	if (strcmp(application,"muhrec")!=0)
		return NULL;

    kipl::interactors::InteractionBase *interactor=reinterpret_cast<kipl::interactors::InteractionBase *>(vinteractor);

	if (name!=NULL) {
		std::string sName=name;

        if (sName=="FullLogNorm")
            return new FullLogNorm;

        if (sName=="FullNorm")
            return new FullNorm;
#ifdef USE_UNPUBLISHED
        if (sName=="RobustLogNorm")
            return new RobustLogNorm;
#endif

		if (sName=="LogProjection")
			return new LogProjection;

		if (sName=="ProjectionFilterSingle")
			return new ProjectionFilterSingle;

		if (sName=="SpotClean")
			return new SpotClean;

		if (sName=="SpotClean2")
			return new SpotClean2;

//		if (sName=="SpotRingClean")
//			return new SpotRingClean;

		if (sName=="TranslatedProjectionWeighting")
			return new TranslatedProjectionWeighting;

		if (sName=="GeneralFilter")
			return new GeneralFilter;

        if (sName=="AdaptiveFilter")
            return new AdaptiveFilter(interactor);

		if (sName=="BasicRingClean")
			return new BasicRingClean;

		if (sName=="MedianMixRingClean")
			return new MedianMixRingClean;

		if (sName=="DataScaler")
			return new DataScaler;

		if (sName=="ISSfilter")
			return new ISSfilter;

		if (sName=="WaveletRingClean")
            return new WaveletRingClean(interactor);
		
		if (sName=="PolynomialCorrection")
			return new PolynomialCorrection;

		if (sName=="SinoSpotClean")
			return new SinoSpotClean;

        if (sName=="TranslateProjections")
            return new TranslateProjectionsModule;

        if (sName=="MorphSpotClean")
            return new MorphSpotCleanModule(interactor);

        if (sName=="GammaSpotClean")
            return new GammaSpotCleanModule;

        if (sName=="CameraStripeClean")
            return new CameraStripeClean;
	}

	return NULL;
}

STDPREPROCMODULESSHARED_EXPORT int Destroy(const char * application, void *obj)
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

STDPREPROCMODULESSHARED_EXPORT int LibVersion()
{
	return -1;
}

STDPREPROCMODULESSHARED_EXPORT int GetModuleList(const char *application, void *listptr)
{
	if (strcmp(application,"muhrec")!=0)
		return -1;

	std::map<std::string, std::map<std::string, std::string> > *modulelist=reinterpret_cast<std::map<std::string, std::map<std::string, std::string> > *>(listptr);

    FullLogNorm flnorm;
    modulelist->operator []("FullLogNorm")=flnorm.GetParameters();

    FullNorm fnorm;
    modulelist->operator []("FullNorm")=fnorm.GetParameters();

#ifdef USE_UNPUBLISHED
    RobustLogNorm robnorm;
    modulelist->operator[]("RobustLogNorm")=robnorm.GetParameters();
#endif

	LogProjection lproj;
	modulelist->operator []("LogProjection")=lproj.GetParameters();

	ProjectionFilterSingle filter;
	modulelist->operator []("ProjectionFilterSingle")=filter.GetParameters();
	
	SpotClean2 sc2;
	modulelist->operator []("SpotClean2")=sc2.GetParameters();

//	SpotRingClean srcl;
//	modulelist->operator []("SpotRingClean")=srcl.GetParameters();

	TranslatedProjectionWeighting tpw;
	modulelist->operator []("TranslatedProjectionWeighting")=tpw.GetParameters();

	GeneralFilter gf;
	modulelist->operator []("GeneralFilter")=gf.GetParameters();

	AdaptiveFilter af;
	modulelist->operator []("AdaptiveFilter")=af.GetParameters();

	BasicRingClean brc;
	modulelist->operator []("BasicRingClean")=brc.GetParameters();

	MedianMixRingClean mmrc;
	modulelist->operator []("MedianMixRingClean")=mmrc.GetParameters();

	DataScaler ds;
	modulelist->operator []("DataScaler")=ds.GetParameters();

	ISSfilter iss;
	modulelist->operator []("ISSfilter")=iss.GetParameters();

	WaveletRingClean wrc;
	modulelist->operator []("WaveletRingClean")=wrc.GetParameters();

	PolynomialCorrection pc;
	modulelist->operator []("PolynomialCorrection")=pc.GetParameters();

	SinoSpotClean ssc;
	modulelist->operator []("SinoSpotClean")=ssc.GetParameters();

    TranslateProjectionsModule tpm;
    modulelist->operator []("TranslateProjections")=tpm.GetParameters();

    MorphSpotCleanModule mscm;
    modulelist->operator []("MorphSpotClean")=mscm.GetParameters();

    GammaSpotCleanModule gscm;
    modulelist->operator []("GammaSpotClean")=gscm.GetParameters();

    CameraStripeClean csc;
    modulelist->operator []("CameraStripeClean")=csc.GetParameters();

	return 0;
}

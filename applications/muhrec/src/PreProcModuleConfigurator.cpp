/*
 * PreProcModuleConfigurator.cpp
 *
 *  Created on: Jan 21, 2012
 *      Author: anders
 */
//#include "stdafx.h"
#include "PreProcModuleConfigurator.h"
#include <sstream>
#include <ConfigBase.h>
#include <ReconException.h>
#include <ModuleException.h>
#include <base/KiplException.h>
#include <ReconFactory.h>
#include <ReconEngine.h>


PreProcModuleConfigurator::PreProcModuleConfigurator(ReconConfig *config)
{
	ModuleConfigurator::m_Config=dynamic_cast<ConfigBase *>(config);
}

PreProcModuleConfigurator::~PreProcModuleConfigurator() {
}

int PreProcModuleConfigurator::GetImage(std::string sSelectedModule)
{
	ReconEngine *engine=NULL;
	ReconFactory factory;

	ReconConfig * config=dynamic_cast<ReconConfig *>(m_Config);
	std::ostringstream msg;
	try {
		engine=factory.BuildEngine(*dynamic_cast<ReconConfig *>(m_Config),NULL);
	}
	catch (ReconException &e) {
		msg<<"Failed to build the configuration engine with a ReconException: "<<e.what();
	}
	catch (ModuleException &e){
		msg<<"Failed to build the configuration engine with a ModuleException: "<<e.what();
	}
	catch (kipl::base::KiplException &e) {
		msg<<"Failed to build the configuration engine with a KiplException: "<<e.what();
	}

	catch (std::exception &e) {
		msg<<"Failed to build the configuration engine with a STL Exception: "<<e.what();
	}
	catch (...) {
		msg<<"Failed to build the configuration engine with an unhandled ";
	}
	if (!msg.str().empty()) {
		throw ModuleException(msg.str(),__FILE__,__LINE__);
	}

	logger(kipl::logging::Logger::LogMessage,"Engine successfully built");
	size_t *r=config->ProjectionInfo.roi;
	size_t roi[4]={r[0],r[1],r[2],r[3]};
	m_Image=engine->RunPreproc(roi,sSelectedModule);

	if (engine!=NULL)
		delete engine;
	return 0;
}

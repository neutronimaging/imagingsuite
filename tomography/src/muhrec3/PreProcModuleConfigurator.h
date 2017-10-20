/*
 * PreProcModuleConfigurator.h
 *
 *  Created on: Jan 21, 2012
 *      Author: anders
 */

#ifndef PREPROCMODULECONFIGURATOR_H_
#define PREPROCMODULECONFIGURATOR_H_

#include <ModuleConfigurator.h>
#include <ReconConfig.h>

class PreProcModuleConfigurator: public ModuleConfigurator {
public:
	PreProcModuleConfigurator(ReconConfig *config);
	virtual ~PreProcModuleConfigurator();
protected:
	virtual int GetImage(std::string sSelectedModule);
};

#endif /* PREPROCMODULECONFIGURATOR_H_ */

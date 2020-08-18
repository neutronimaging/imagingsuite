//<LICENSE>

#ifndef PREPROCMODULECONFIGURATOR_H_
#define PREPROCMODULECONFIGURATOR_H_

#include <ModuleConfigurator.h>
#include <ReconConfig.h>
#include <interactors/interactionbase.h>

class PreProcModuleConfigurator: public ModuleConfigurator {
public:
    PreProcModuleConfigurator(ReconConfig *config, kipl::interactors::InteractionBase *interactor=nullptr);
	virtual ~PreProcModuleConfigurator();
protected:
    virtual int GetImage(std::string sSelectedModule, kipl::interactors::InteractionBase *interactor=nullptr);
};

#endif /* PREPROCMODULECONFIGURATOR_H_ */

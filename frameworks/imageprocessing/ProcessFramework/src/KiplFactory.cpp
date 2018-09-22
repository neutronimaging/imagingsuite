//<LICENSE>

#include "stdafx.h"
#include "../include/KiplFactory.h"
#include "../include/KiplFrameworkException.h"
#include "../include/KiplModuleItem.h"
#include <ModuleException.h>


KiplFactory::KiplFactory(void)
{
}

KiplFactory::~KiplFactory(void)
{
}


KiplEngine * KiplFactory::BuildEngine(KiplProcessConfig &config, kipl::interactors::InteractionBase *interactor)
{
    KiplEngine * engine=new KiplEngine("KiplEngine",interactor);

	engine->SetConfig(config);
	
	std::list<ModuleConfig>::iterator it;

	// Setting up the processing modules
	for (it=config.modules.begin(); it!=config.modules.end(); it++) {
		if (it->m_bActive==true) {
            KiplModuleItem *module=nullptr;
			try {
                module=new KiplModuleItem(it->m_sSharedObject,it->m_sModule,interactor);

				module->GetModule()->Configure(it->parameters);
				engine->AddProcessingModule(module);
			}
			catch (ModuleException &e) {
				throw KiplFrameworkException(e.what(),__FILE__,__LINE__);
			}
			catch (KiplFrameworkException &e) {
				throw KiplFrameworkException(e.what(),__FILE__,__LINE__);
			}
		}
	} 

	return engine;
}


//<LICENSE>

#include "stdafx.h"
#include "../include/KiplFactory.h"
#include "../include/KiplFrameworkException.h"
#include "../include/KiplModuleItem.h"
#include <ModuleException.h>
#include "../include/KiplProcessModuleBase.h"


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
	
	// Setting up the processing modules
    for (auto &module : config.modules) {
        if (module.m_bActive==true) {
            KiplModuleItem *pModule=nullptr;
			try {
                pModule=new KiplModuleItem(module.m_sSharedObject,module.m_sModule,interactor);

                pModule->GetModule()->Configure(config, module.parameters);
                engine->AddProcessingModule(pModule);
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


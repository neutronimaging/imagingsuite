//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2008-11-11 21:09:49 +0100 (Di, 11 Nov 2008) $
// $Rev: 13 $
//

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


KiplEngine * KiplFactory::BuildEngine(KiplProcessConfig &config)
{
	KiplEngine * engine=new KiplEngine;

	engine->SetConfig(config);
	
	std::list<ModuleConfig>::iterator it;

	// Setting up the processing modules
	for (it=config.modules.begin(); it!=config.modules.end(); it++) {
		if (it->m_bActive==true) {
			KiplModuleItem *module=NULL;
			try {
				module=new KiplModuleItem(it->m_sSharedObject,it->m_sModule);

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


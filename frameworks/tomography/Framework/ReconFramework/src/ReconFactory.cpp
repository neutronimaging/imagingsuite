//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $File$
// $Date$
// $Rev$
// $Id$
//
#include "stdafx.h"
#include "../include/ReconFactory.h"
#include "../include/ReconConfig.h"
#include "../include/PreprocModuleBase.h"
#include "../include/ReconException.h"
#include <iostream>

ReconFactory::ReconFactory(void) :
	logger("ReconFactory")
{

}

ReconFactory::~ReconFactory(void)
{

}

ReconEngine * ReconFactory::BuildEngine(ReconConfig &config, kipl::interactors::InteractionBase *interactor)
{
	ReconEngine * engine=new ReconEngine("ReconEngine",interactor);

	engine->SetConfig(config);
	

	std::list<ModuleConfig>::iterator it;

	// Setting up the preprocessing modules
    int i=0;
    for (it=config.modules.begin(); it!=config.modules.end(); it++, i++) {
		if (it->m_bActive==true) {
            ModuleItem *module=nullptr;
			try {
                module=new ModuleItem("muhrec",it->m_sSharedObject,it->m_sModule,interactor);

				module->GetModule()->Configure(config,it->parameters);
				engine->AddPreProcModule(module);
			}
			catch (ReconException &e) {
				throw ReconException(e.what(),__FILE__,__LINE__);
			}
		}
	}

    SetBackProjector(config,engine,interactor);
	return engine;

}

void ReconFactory::SetBackProjector(ReconConfig &config, ReconEngine * engine, kipl::interactors::InteractionBase *interactor)
{
    if (config.backprojector.m_bActive==true) {
        BackProjItem *module=nullptr;
        try {
            module=new BackProjItem("muhrecbp",config.backprojector.m_sSharedObject,config.backprojector.m_sModule,interactor);

            module->GetModule()->Configure(config,config.backprojector.parameters);
            engine->SetBackProjector(module);
        }
        catch (ReconException &e) {
            throw ReconException(e.what(),__FILE__,__LINE__);
        }
    }
}


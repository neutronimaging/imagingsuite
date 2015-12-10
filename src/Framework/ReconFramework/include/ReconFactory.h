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
#ifndef __RECONFACTORY_H
#define __RECONFACTORY_H
#include "ReconFramework_global.h"

#include <list>
#include <string>

#include <logging/logger.h>
#include <interactors/interactionbase.h>
#include "ReconEngine.h"
#include "PreprocModuleBase.h"
#include "ReconConfig.h"

class RECONFRAMEWORKSHARED_EXPORT ReconFactory
{
private:
	kipl::logging::Logger logger;
public:
	ReconFactory(void);
    ReconEngine * BuildEngine(ReconConfig &config, kipl::interactors::InteractionBase *interactor=nullptr);
    void SetBackProjector(ReconConfig &config, ReconEngine * engine, kipl::interactors::InteractionBase *interactor=nullptr);
	virtual ~ReconFactory(void);

private:
};


#endif

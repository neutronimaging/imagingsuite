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
//#include "stdafx.h"
#include <logging/logger.h>
#include "ReconEngine.h"
#include "PreprocModuleBase.h"
#include "ReconConfig.h"
#include "InteractionBase.h"
#include <list>
#include <string>

class RECONFRAMEWORKSHARED_EXPORT ReconFactory
{
private:
	kipl::logging::Logger logger;
public:
	ReconFactory(void);
	ReconEngine * BuildEngine(ReconConfig &config, InteractionBase *interactor=NULL);
    void SetBackProjector(ReconConfig &config, ReconEngine * engine, InteractionBase *interactor=NULL);
	virtual ~ReconFactory(void);

private:
};


#endif

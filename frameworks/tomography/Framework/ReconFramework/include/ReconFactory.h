//<LICENSE>
#ifndef RECONFACTORY_H
#define RECONFACTORY_H
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

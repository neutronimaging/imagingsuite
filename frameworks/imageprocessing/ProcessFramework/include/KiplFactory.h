//<LICENSE>

#ifndef __KIPLFACTORY_H
#define __KIPLFACTORY_H

#include <interactors/interactionbase.h>
#include "ProcessFramework_global.h"
#include "KiplEngine.h"
#include "KiplProcessConfig.h"

class PROCESSFRAMEWORKSHARED_EXPORT KiplFactory
{
public:
	KiplFactory(void);
	~KiplFactory(void);

    KiplEngine * BuildEngine(KiplProcessConfig &config, kipl::interactors::InteractionBase *interactor=nullptr);

};

#endif

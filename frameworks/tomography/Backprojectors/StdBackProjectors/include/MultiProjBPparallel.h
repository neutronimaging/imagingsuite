//<LICENSE>

#ifndef _MULTIPROJBPPARALLEL_H_
#define _MULTIPROJBPPARALLEL_H_
#include "StdBackProjectors_global.h"

#include <interactors/interactionbase.h>
#include "../include/StdBackProjectorBase.h"

namespace reconstructor{ namespace UnitTests {
    class testBasicReconstructor;
}}
class STDBACKPROJECTORS_EXPORT MultiProjectionBPparallel:
	public StdBackProjectorBase
{
public:
    MultiProjectionBPparallel(kipl::interactors::InteractionBase *interactor=nullptr);
	virtual ~MultiProjectionBPparallel(void);
protected:
	virtual void BackProject();
};

#endif

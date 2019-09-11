//<LICENSE>

#ifndef _MULTIPROJBP_H_
#define _MULTIPROJBP_H_

#include <interactors/interactionbase.h>
#include "../include/StdBackProjectorBase.h"

namespace reconstructor{ namespace UnitTests {
	class testBasicReconstructor;
}}
class MultiProjectionBP:
	public StdBackProjectorBase
{
public:
    MultiProjectionBP(kipl::interactors::InteractionBase *interactor=nullptr);
	virtual ~MultiProjectionBP(void);
protected:
	virtual void BackProject();
};

#endif

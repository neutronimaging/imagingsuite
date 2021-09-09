//<LICENSE>

#ifndef _NNBP_H_
#define _NNBP_H_

#include <interactors/interactionbase.h>
#include "../include/StdBackProjectorBase.h"

namespace reconstructor{ namespace UnitTests {
	class STDBACKPROJECTORS_EXPORT testBasicReconstructor;
}}
class STDBACKPROJECTORS_EXPORT NearestNeighborBP:
	public StdBackProjectorBase
{
public:
    NearestNeighborBP(kipl::interactors::InteractionBase *interactor=nullptr);
	virtual ~NearestNeighborBP(void);
protected:
	virtual void BackProject();
};

#endif

//<LICENSE>

#ifndef DOUBLETHRESHOLD_H_
#define DOUBLETHRESHOLD_H_

#include "ClassificationModules_global.h"
#include <KiplProcessModuleBase.h>
#include <segmentation/thresholds.h>
#include <KiplProcessConfig.h>

class CLASSIFICATIONMODULES_EXPORT DoubleThreshold: public KiplProcessModuleBase {
public:
    DoubleThreshold(kipl::interactors::InteractionBase *interactor=nullptr);
	virtual ~DoubleThreshold();

    virtual int Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();

protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float m_fHighThreshold;
	float m_fLowThreshold;

    kipl::segmentation::CmpType m_Compare;

};

#endif /* DOUBLETHRESHOLD_H_ */

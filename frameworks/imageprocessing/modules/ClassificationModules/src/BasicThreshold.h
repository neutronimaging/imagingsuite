/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef BASICTHRESHOLD_H_
#define BASICTHRESHOLD_H_

#include "ClassificationModules_global.h"
#include <KiplProcessModuleBase.h>
#include <KiplProcessConfig.h>

class CLASSIFICATIONMODULES_EXPORT BasicThreshold: public KiplProcessModuleBase {
public:
    BasicThreshold(kipl::interactors::InteractionBase *interactor=nullptr);
    virtual ~BasicThreshold();
	
    virtual int Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float m_fThreshold;
    KiplProcessConfig m_Config;

private:
    bool updateStatus(float val, std::string msg);

};

#endif /* DATASCALER_H_ */

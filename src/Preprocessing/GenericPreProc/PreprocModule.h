/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef PREPROCMODULE_H_
#define PREPROCMODULE_H_

#include "genericpreproc_global.h"
#include <PreprocModuleBase.h>

class GENERICPREPROCSHARED_EXPORT PreprocModule: public PreprocModuleBase {
public:
    PreprocModule();
    virtual ~PreprocModule();
	
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
	virtual bool SetROI(size_t *roi);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float fOffset;
	float fSlope;
};

#endif /* PREPROCMODULE_H_ */

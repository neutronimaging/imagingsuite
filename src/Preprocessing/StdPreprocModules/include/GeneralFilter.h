/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef GENERALFILTER_H_
#define GENERALFILTER_H_

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>

class STDPREPROCMODULESSHARED_EXPORT GeneralFilter: public PreprocModuleBase {
public:
	GeneralFilter();
	virtual ~GeneralFilter();
	
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
	virtual bool SetROI(size_t *roi);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);

};

#endif /* GENERALFILTER_H_ */

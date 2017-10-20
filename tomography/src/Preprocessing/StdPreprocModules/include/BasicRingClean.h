/*
 * BasicRingClean.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef BASICRINGCLEAN_H_
#define BASICRINGCLEAN_H_

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>

class STDPREPROCMODULESSHARED_EXPORT BasicRingClean: public PreprocModuleBase {
public:
	BasicRingClean();
	virtual ~BasicRingClean();
	
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
	virtual bool SetROI(size_t *roi);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);


};

#endif /* GENERALFILTER_H_ */

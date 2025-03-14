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
	using PreprocModuleBase::Configure;
	BasicRingClean();
	virtual ~BasicRingClean();
	
	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
	std::map<std::string, std::string> GetParameters() override;
	bool SetROI(const std::vector<size_t> & roi) override;
protected:
	using PreprocModuleBase::ProcessCore;
	int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) override;


};

#endif /* GENERALFILTER_H_ */

/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef DATASCALER_H_
#define DATASCALER_H_

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>

class STDPREPROCMODULESSHARED_EXPORT DataScaler: public PreprocModuleBase {
public:
	using PreprocModuleBase::Configure;

	DataScaler();
	virtual ~DataScaler();
	
	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
	std::map<std::string, std::string> GetParameters() override;
	bool SetROI(const std::vector<size_t> &roi) override;
protected:
	int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
	int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) override;

	float fOffset;
	float fSlope;
};

#endif /* DATASCALER_H_ */

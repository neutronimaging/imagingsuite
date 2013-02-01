/*
 * SpotCleanModule.h
 *
 *  Created on: Jan 22, 2013
 *      Author: kaestner
 */

#ifndef SPOTCLEANMODULE_H_
#define SPOTCLEANMODULE_H_

#include "stdafx.h"

#include <KiplProcessModuleBase.h>
#include <logging/logger.h>

class DLL_EXPORT SpotCleanModule: public KiplProcessModuleBase {
protected:
	kipl::logging::Logger logger;

public:
	SpotCleanModule();
	virtual ~SpotCleanModule();

	virtual int Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int Process(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters) {return 0;}

};

#endif /* SPOTCLEANMODULE_H_ */

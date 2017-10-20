/*
 * ReferenceImageCorrectionModule.h
 *
 *  Created on: Jan 22, 2013
 *      Author: kaestner
 */

#ifndef REFERENCEIMAGECORRECTIONMODULE_H_
#define REFERENCEIMAGECORRECTIONMODULE_H_

#include "stdafx.h"

#include <KiplProcessModuleBase.h>
#include <ReferenceImageCorrection.h>
#include <logging/logger.h>

class DLL_EXPORT ReferenceImageCorrectionModule :  public KiplProcessModuleBase {
protected:
	kipl::logging::Logger logger;

public:
	ReferenceImageCorrectionModule();
	virtual ~ReferenceImageCorrectionModule();


	virtual int Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();

protected:
	virtual int Process(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters);

};

#endif /* REFERENCEIMAGECORRECTIONMODULE_H_ */

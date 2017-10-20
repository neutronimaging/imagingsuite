/*
 * PolynomialCorrectionModule.h
 *
 *  Created on: Jan 22, 2013
 *      Author: kaestner
 */

#ifndef PolynomialCorrectionModule_H_
#define PolynomialCorrectionModule_H_

#include "stdafx.h"

#include <KiplProcessModuleBase.h>
#include <logging/logger.h>

class DLL_EXPORT PolynomialCorrectionModule: public KiplProcessModuleBase {
protected:
	kipl::logging::Logger logger;

public:
	PolynomialCorrectionModule();
	virtual ~PolynomialCorrectionModule();

	virtual int Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int Process(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters) {return 0;}

};

#endif /* PolynomialCorrectionModule_H_ */

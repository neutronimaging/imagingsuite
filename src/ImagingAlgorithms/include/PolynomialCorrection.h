/*
 * PolynomialCorrection.h
 *
 *  Created on: Jan 28, 2013
 *      Author: kaestner
 */

#ifndef POLYNOMIALCORRECTION_H_
#define POLYNOMIALCORRECTION_H_
#include "../src/stdafx.h"
#include <logging/logger.h>

namespace imagingalgorithms {

class PolynomialCorrection {
protected:
	kipl::logging::Logger logger;
public:
	PolynomialCorrection();
	virtual ~PolynomialCorrection();
};

} /* namespace imagingalgorithms */
#endif /* POLYNOMIALCORRECTION_H_ */

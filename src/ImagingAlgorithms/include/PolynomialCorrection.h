/*
 * PolynomialCorrection.h
 *
 *  Created on: Jan 28, 2013
 *      Author: kaestner
 */

#ifndef IMGALGPOLYNOMIALCORRECTION_H_
#define IMGALGPOLYNOMIALCORRECTION_H_
#include "../src/stdafx.h"
#include <logging/logger.h>

namespace ImagingAlgorithms {

class DLL_EXPORT PolynomialCorrection {
protected:
	kipl::logging::Logger logger;
public:
	PolynomialCorrection();
	virtual ~PolynomialCorrection();
	int Setup(float *coef, int degree);
	void Process(float *x, float *y, size_t N, float minX, float maxX);
	void Process(float *data, size_t N);
protected:
	float m_fCoef[16];
	int m_nDegree;
};

} /* namespace imagingalgorithms */
#endif /* POLYNOMIALCORRECTION_H_ */

/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef POLYNOMIALCORRECTION_H_
#define POLYNOMIALCORRECTION_H_

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>

class STDPREPROCMODULESSHARED_EXPORT PolynomialCorrection: public PreprocModuleBase {
public:
	PolynomialCorrection();
	virtual ~PolynomialCorrection();
	
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
	virtual bool SetROI(size_t *roi);
	void PlotPolynomial(float *x, float *y, size_t N, float minX, float maxX);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	void ComputePolynomial(float *data, size_t N);
	int m_nDegree;
	float m_fCoef[10];
};

#endif /* DATASCALER_H_ */

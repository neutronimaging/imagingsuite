//<LICENSE>

#ifndef POLYNOMIALCORRECTION_H
#define POLYNOMIALCORRECTION_H

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <PolynomialCorrection.h>


class STDPREPROCMODULESSHARED_EXPORT PolynomialCorrection: public PreprocModuleBase {
public:
	PolynomialCorrection();
	virtual ~PolynomialCorrection();
	
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
    virtual bool SetROI(const std::vector<size_t> &roi);
	void PlotPolynomial(float *x, float *y, size_t N, float minX, float maxX);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

    ImagingAlgorithms::PolynomialCorrection pc;
};

#endif /* DATASCALER_H_ */

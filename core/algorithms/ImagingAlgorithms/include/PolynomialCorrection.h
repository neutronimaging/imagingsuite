//<LICENSE>

#ifndef IMGALGPOLYNOMIALCORRECTION_H_
#define IMGALGPOLYNOMIALCORRECTION_H_

#include "ImagingAlgorithms_global.h"
#include <vector>

#include <logging/logger.h>

namespace ImagingAlgorithms {

class IMAGINGALGORITHMSSHARED_EXPORT PolynomialCorrection {
protected:
	kipl::logging::Logger logger;
public:
	PolynomialCorrection();
	virtual ~PolynomialCorrection();
	int Setup(float *coef, int degree);
	void Process(float *x, float *y, size_t N, float minX, float maxX);
	void Process(float *data, size_t N);
    std::vector<float> coefficients();
    int polynomialOrder() {return m_fCoef.size()-1;}
protected:
    std::vector<float> m_fCoef;
    int m_nDegree;
};


} /* namespace imagingalgorithms */
#endif /* POLYNOMIALCORRECTION_H_ */

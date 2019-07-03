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
    int setup(float *coef, int degree);
    int setup(const std::vector<float> &coeff);
    void process(float *x, float *y, size_t N, float minX, float maxX);
    std::vector<float> process(const std::vector<float> &x);
    void processInplace(float *data, size_t N);
    void processInplace(double *data,size_t N);
    std::vector<float> coefficients();
    int polynomialOrder() {return static_cast<int>(m_fCoef.size())-1;}
protected:
    float computePolynomial(float x);
    std::vector<float> m_fCoef;
    int m_nDegree;
};


} /* namespace imagingalgorithms */
#endif /* POLYNOMIALCORRECTION_H_ */

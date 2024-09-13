#ifndef NLCSC_H
#define NLCSC_H

#include "ImagingAlgorithms_global.h"

#include <vector>
#include <functional>
#include <utility>
#include <numeric>
#include <algorithm>



class IMAGINGALGORITHMSSHARED_EXPORT NLCSC {
public:
    // Constructor that takes base lag rates, initial lag coefficients, and the exposure curve function
    NLCSC(const std::vector<float>& baseLagRates, const std::vector<float>& initialLagCoeffs,
          std::function<std::vector<float>(float, const std::vector<float>&)> exposureCurve,
          float frameRate);

    // Main function to apply lag correction
    std::vector<float> deconvolveLag(const std::vector<float>& y, const std::vector<float>& exposure);

#ifdef QT_TESTLIB_LIB
public:
#else
private:
#endif
    std::vector<float> baseLagRates_;
    std::vector<float> initialLagCoeffs_;
    std::function<std::vector<float>(float, const std::vector<float>&)> exposureCurve_;
    float frameRate_;

    // Helper functions
    std::pair<std::vector<float>, std::vector<float>> getExposureDependentParams(float exposure);
    std::pair<float, std::vector<float>> deconvolutionStep(float y_k, const std::vector<float>& bn_x,
                                                           const std::vector<float>& an_x, const std::vector<float>& Sn_k);
    std::pair<std::vector<float>, std::vector<float>> updateStoredCharge(const std::vector<float>& S_star_k,
                                                                         float x_k, const std::vector<float>& bn_x,
                                                                         const std::vector<float>& an_x);
    float getS_star(float Sn_k, float bn_x, float an_x);
    float getStoredCharge(float Sn_k, float bn_x, float an_x);
    std::vector<float> getExposureDependentRates(float exposure);

    float dotProduct(const std::vector<float>& a, const std::vector<float>& b);
    float sum(const std::vector<float>& v);
};

std::vector<float> IMAGINGALGORITHMSSHARED_EXPORT exampleExposureCurve(float exposure, const std::vector<float>& initialLagCoeffs);

#endif  // NLCSC_H

//<LICENSE>

#ifndef RESOLUTIONESTIMATORS_H
#define RESOLUTIONESTIMATORS_H
#include "imagingqaalgorithms_global.h"

#include <vector>
#include <string>
#include <iostream>
#include <armadillo>

#include <base/timage.h>
#include <math/nonlinfit.h>
#include <logging/logger.h>

namespace ImagingQAAlgorithms {
class  IMAGINGQAALGORITHMSSHARED_EXPORT ResolutionEstimator
{
    kipl::logging::Logger logger;
public:

    ResolutionEstimator();
    ~ResolutionEstimator();

    Nonlinear::eProfileFunction profileFunction;

    void   setPixelSize(double s);
    double pixelSize();
    size_t size();
    void   setProfile(float *p, int N);
    void   setProfile(double *p, int N);
    void   setProfile(const std::vector<double> &p);
    void   setProfile(const std::vector<float> &p);
    void   setProfile(arma::vec &p);
    const std::vector<double> &profile();
    void   clear();

    double FWHM();
    double MTFresolution(float level=0.1f);
    void edgeDerivative(std::vector<double> &x, std::vector<double> &y, bool returnFit=false, float smooth=0.0f);
    void MTF(std::vector<double> &w, std::vector<double> &a);
    Nonlinear::SumOfGaussians fitFunction();

protected:
    void createAllocation(int N);
    void removeAllocation();
    void prepareXAxis();
    void analysis();
    void analyzeLineSpread();
    void analyzeMTF();
    void diffProfile();
    int profileSize;
    double mPixelSize;
    std::vector<double> mProfile;
    std::vector<double> mDiffProfile;
    std::vector<double> mXaxis;
    double mfwhm;
    Nonlinear::SumOfGaussians mFitFunction;
};
}



#endif // RESOLUTIONESTIMATORS_H

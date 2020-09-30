//<LICENSE>

#ifndef RESOLUTIONESTIMATORS_H
#define RESOLUTIONESTIMATORS_H
#include "imagingqaalgorithms_global.h"

#include <vector>
#include <string>
#include <iostream>
//#include <tnt.h>


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
    void   setProfile(arma::vec & p);
    const std::vector<double> &profile();
    void   clear();

    double FWHM();
    double MTFresolution(double level=0.1);
    void   edgeDerivative(std::vector<double> &x, std::vector<double> &y, bool returnFit=false, float smooth=0.0f);
    const  vector<double> &MTFAmplitude();
    const  vector<double> &MTFFrequency();
    Nonlinear::SumOfGaussians fitFunction();

protected:
    void createAllocation(int N);
    void removeAllocation();
    void prepareXAxis();
    void analysis();
    void analyzeLineSpread();
    void analyzeMTF();
    void diffProfile();
    size_t profileSize;
    double mPixelSize;
    std::vector<double> mProfile;
    std::vector<double> mDiffProfile;
    std::vector<double> mXaxis;
    double mfwhm;
    Nonlinear::SumOfGaussians mFitFunction;
    std::vector<double> mMTFamplitude;
    std::vector<double> mMTFfrequency;
};
}



#endif // RESOLUTIONESTIMATORS_H

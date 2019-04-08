#ifndef RESOLUTIONESTIMATORS_H
#define RESOLUTIONESTIMATORS_H
#include "imagingqaalgorithms_global.h"

#include <vector>
#include <string>
#include <iostream>
#include <tnt.h>


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
    int    size();
    void   setProfile(float *p, int N);
    void   setProfile(double *p, int N);
    void   setProfile(std::vector<double> &p);
    void   setProfile(std::vector<float> &p);
    void   setProfile(TNT::Array1D<double> &p);
    void   profile(double *p, int &N);
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
    double *mProfile;
    double *mDiffProfile;
    double *mXaxis;
    double mfwhm;
    Nonlinear::SumOfGaussians mFitFunction;
};
}



#endif // RESOLUTIONESTIMATORS_H

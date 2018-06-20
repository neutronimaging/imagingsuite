#ifndef RESOLUTIONESTIMATORS_H
#define RESOLUTIONESTIMATORS_H
#include "imagingqaalgorithms_global.h"

#include <vector>
#include <string>
#include <iostream>
#include <tnt.h>


#include <base/timage.h>
#include <math/nonlinfit.h>

namespace ImagingQAAlgorithms {
class  IMAGINGQAALGORITHMSSHARED_EXPORT ResolutionEstimator
{
public:

    ResolutionEstimator();
    ~ResolutionEstimator();

    Nonlinear::eProfileFunction profileFunction;

    void   setPixelSize(double s);
    double getPixelSize();
    void   setProfile(float *p, int N, double d=1.0f);
    void   setProfile(double *p, int N, double d=1.0f);
    void   setProfile(std::vector<double> &p, double d=1.0);
    void   setProfile(std::vector<float> &p, double d=1.0);
    void   setProfile(TNT::Array1D<double> &p, double d=1.0);

    float getFWHM();
    float getMTFresolution(float level=0.1f);
    void getEdgeDerivative(std::vector<double> &x, std::vector<double> &y, bool returnFit=false, float smooth=0.0f);
    void getMTF(std::vector<double> &w, std::vector<double> &a);

protected:
    void createAllocation(int N);
    void removeAllocation();
    void prepareXAxis();
    void analysis();
    void analyzeLineSpread();
    void analyzeMTF();
    void diffProfile();
    int profileSize;
    double pixelSize;
    double *profile;
    double *dprofile;
    double *xaxis;
    double fwhm;
    Nonlinear::FitFunctionBase &fn;
};
}



#endif // RESOLUTIONESTIMATORS_H

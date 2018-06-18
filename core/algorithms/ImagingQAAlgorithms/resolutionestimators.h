#ifndef RESOLUTIONESTIMATORS_H
#define RESOLUTIONESTIMATORS_H
#include "imagingqaalgorithms_global.h"

#include <vector>
#include <string>
#include <iostream>
#include <tnt.h>


#include <base/timage.h>

namespace ImagingQAAlgorithms {
class  IMAGINGQAALGORITHMSSHARED_EXPORT ResolutionEstimator
{
public:
    enum eProfileFunction {
        BestProfile,
        GaussProfile,
        LorenzProfile,
        VoightProfile
    };

    ResolutionEstimator();
    ~ResolutionEstimator();

    eProfileFunction profileFunction;

    void   setPixelSize(double s);
    double getPixelSize();
    void   setProfile(float *p, int N, double d=1.0f);
    void   setProfile(double *p, int N, double d=1.0f);
    void   setProfile(std::vector<double> &p, double d=1.0);
    void   setProfile(TNT::Array1D<double> &p, double d=1.0);

    float getFWHM();
    float getMTFresolution(float level=0.1f);
    void getEdgeDerivative(std::vector<double> &x, std::vector<double> &y, float smooth=0.0f);
    void getMTF(std::vector<double> &w, std::vector<double> &a);

protected:
    void createAllocation(int N);
    void removeAllocation();
    int profileSize;
    double pixelSize;
    double *profile;
};
}

void string2enum(std::string &str, ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction &e);
std::string enum2string(ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction e);

std::ostream & operator<<(std::ostream &s, ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction e);

#endif // RESOLUTIONESTIMATORS_H

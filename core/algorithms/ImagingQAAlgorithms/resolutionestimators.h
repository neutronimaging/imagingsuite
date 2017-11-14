#ifndef RESOLUTIONESTIMATORS_H
#define RESOLUTIONESTIMATORS_H
#include "imagingqaalgorithms_global.h"

#include <vector>

#include <base/timage.h>

namespace ImagingQAAlgorithms {
class  ResolutionEstimators
{
public:
    enum eProfileFunction {
        GaussProfile,
        LorenzProfile,
        VoightProfile
    };

    ResolutionEstimators();
    ~ResolutionEstimators();

    eProfileFunction profileFunction;
    float pixelSize;

    void setProfile(float *p, int N, float d=1.0f);

    float getFWHM();
    float getMTFresolution(float level=0.1f);
    vector<pair<float,float> > getEdgeDerivative(float smooth=0.0f);
    vector<pair<float,float> > getMTF();

protected:
    void createAllocation(int N);
    void removeAllocation();
    int profileSize;
    float dx;
    float *profile;
};

#endif // RESOLUTIONESTIMATORS_H

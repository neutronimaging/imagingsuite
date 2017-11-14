#include "resolutionestimators.h"

namespace ImagingQAAlgorithms {

ResolutionEstimators::ResolutionEstimators() :
    profileFunction(LorenzProfile),
    pixelSize(0.1f),
    profileSize(0),
    dx(1.0f),
    profile(nullptr)
{

}

ResolutionEstimators::~ResolutionEstimators()
{
    removeAllocation();
}


void ResolutionEstimators::setProfile(float *p, int N, float d)
{

}

float ResolutionEstimators::getFWHM()
{
    return -1.0f;
}

float ResolutionEstimators::getMTFresolution(float level)
{
    return -1.0f;
}

vector<pair<float,float> > ResolutionEstimators::getEdgeDerivative(float smooth)
{
    vector<pair<float,float> > d;

    return d;
}

vector<pair<float,float> > ResolutionEstimators::getMTF()
{
    vector<pair<float,float> > d;

    return d;
}

void ResolutionEstimators::createAllocation(int N)
{
    removeAllocation();

    profile=new float[N];
    profileSize=N;
}

void ResolutionEstimators::removeAllocation()
{
    if (profile!=nullptr) {
        delete [] profile;
        N=0;
    }
}
}

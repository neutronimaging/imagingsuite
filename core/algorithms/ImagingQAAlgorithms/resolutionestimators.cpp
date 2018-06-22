//<LICENSE>

#include <map>
#include <string>

#include <QDebug>

#include "resolutionestimators.h"

#include <tnt.h>
#include <strings/miscstring.h>
#include <math/nonlinfit.h>

namespace ImagingQAAlgorithms {

ResolutionEstimator::ResolutionEstimator() :
    profileFunction(Nonlinear::fnSumOfGaussians),
    profileSize(0),
    pixelSize(0.1),
    profile(nullptr),
    dprofile(nullptr),
    xaxis(nullptr),
    fwhm(-1.0),
    fn(Nonlinear::SumOfGaussians)
{

}

ResolutionEstimator::~ResolutionEstimator()
{
    removeAllocation();
}

void ResolutionEstimator::setPixelSize(double s)
{
    pixelSize=fabs(s);
}

double ResolutionEstimator::getPixelSize()
{
    return pixelSize;
}

void ResolutionEstimator::setProfile(float *p, int N, double d)
{
    (void)d;
    createAllocation(N);
    std::copy_n(p,N,profile);
    analysis();
}

void ResolutionEstimator::setProfile(double *p, int N, double d)
{
    (void)d;
    createAllocation(N);
    std::copy_n(p,N,profile);
    analysis();
}

void ResolutionEstimator::setProfile(std::vector<double> &p, double d)
{
    (void)d;
    size_t N=p.size();
    createAllocation(N);
    for (size_t i=0; i<N; ++i)
        profile[i]=p[i];
    analysis();
}

void ResolutionEstimator::setProfile(std::vector<float> &p, double d)
{
    (void)d;
    size_t N=p.size();
    createAllocation(N);
    for (size_t i=0; i<N; ++i)
        profile[i]=p[i];
    analysis();
}

void ResolutionEstimator::setProfile(TNT::Array1D<double> &p, double d)
{
    (void)d;
    size_t N=p.dim1();
    createAllocation(N);
    for (size_t i=0; i<N; ++i)
        profile[i]=p[i];
    analysis();
}

float ResolutionEstimator::getFWHM()
{
    return fwhm;
}

float ResolutionEstimator::getMTFresolution(float level)
{
    (void)level;

    return -1.0f;
}

void ResolutionEstimator::getEdgeDerivative(std::vector<double> &x, std::vector<double> &y, bool returnFit, float smooth)
{
    (void) smooth;
    x.clear();
    y.clear();
    if (returnFit==true) {
        for (int i=0; i<profileSize; ++i) {
            x.push_back(i*pixelSize);
            y.push_back(fn(x[i]));
        }
    }
    else {
        for (int i=0; i<profileSize; ++i) {
            x.push_back(i*pixelSize);
            y.push_back(dprofile[i]);
        }
    }
}

void ResolutionEstimator::getMTF(std::vector<double> &w, std::vector<double> &a)
{

}

void ResolutionEstimator::createAllocation(int N)
{
    removeAllocation();

    profile=new double[N];
    dprofile=new double[N];
    xaxis=new double[N];
    profileSize=N;
}

void ResolutionEstimator::removeAllocation()
{
    if (profile!=nullptr) {
        delete [] profile;
        profile=nullptr;
        profileSize=0;
    }

    if (dprofile!=nullptr) {
        delete [] dprofile;
        dprofile=nullptr;
    }

    if (xaxis!=nullptr) {
        delete [] xaxis;
        xaxis=nullptr;
    }
}

void ResolutionEstimator::prepareXAxis()
{
    for (int i=0; i<profileSize; ++i) {
        xaxis[i]=pixelSize*i;
    }
}

void ResolutionEstimator::analysis()
{
   diffProfile();

   analyzeLineSpread();

   analyzeMTF();
}

void ResolutionEstimator::analyzeLineSpread()
{
    Nonlinear::LevenbergMarquardt mrq;

    switch (profileFunction) {
    case Nonlinear::fnSumOfGaussians :
        fn=Nonlinear::SumOfGaussians;
        fn[0]=profileSize/2.0;
        fn[1]=*std::max_element(dprofile,dprofile+profileSize);
        fn[2]=profileSize/6.0;
        break;
    case Nonlinear::fnLorenzian :
        fn=Nonlinear::Lorenzian;
        break;
    case Nonlinear::fnVoight :
        fn=Nonlinear::Voight;
        break;
    default :
        throw kipl::base::KiplException("Unsupported profile fit function in resolution estimator",__FILE__,__LINE__);
    }

    TNT::Array1D<double> x(profileSize);
    TNT::Array1D<double> y(profileSize);
    TNT::Array1D<double> sig(profileSize);

    for (int i=0; i<x.dim1(); ++i) {
        x[i]=xaxis[i];
        y[i]=dprofile[i];
        sig[i]=1.0;
    }
    mrq.fit(x,y,sig,fn);


}

void ResolutionEstimator::analyzeMTF()
{

}

void ResolutionEstimator::diffProfile()
{
    for (int i=0; i<profileSize-1; ++i)
        dprofile[i]=profile[i+1]-profile[i];
    dprofile[profileSize-1]=dprofile[profileSize-2];
}

}



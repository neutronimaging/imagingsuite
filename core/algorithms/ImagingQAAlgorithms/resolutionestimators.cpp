//<LICENSE>

#include <map>
#include <string>

#include <QDebug>

#include "resolutionestimators.h"

#include <tnt.h>
#include <strings/miscstring.h>
#include <math/nonlinfit.h>

namespace ImagingQAAlgorithms {
Nonlinear::SumOfGaussians sog_dummy(1);

ResolutionEstimator::ResolutionEstimator() :
    logger("ResolutionEstimator"),
    profileFunction(Nonlinear::fnSumOfGaussians),
    profileSize(0),
    pixelSize(0.1),
    profile(nullptr),
    dprofile(nullptr),
    xaxis(nullptr),
    fwhm(-1.0),
    fitFunction(1)
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
            y.push_back(fitFunction(x[i]));
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

Nonlinear::SumOfGaussians ResolutionEstimator::getFitFunction()
{
    return fitFunction;
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

 //  analyzeMTF();
}

void ResolutionEstimator::analyzeLineSpread()
{
    std::ostringstream msg;

    TNT::Array1D<double> dataX(profileSize);
    TNT::Array1D<double> dataY(profileSize);
    TNT::Array1D<double> dataSig(profileSize);

    for (int i=0; i<dataX.dim1(); ++i) {
        dataX[i]=xaxis[i];
        dataY[i]=dprofile[i];
        dataSig[i]=1.0;
    }

    double maxval=-std::numeric_limits<double>::max();
    double minval=std::numeric_limits<double>::max();

    int maxpos=0;
    int minpos=0;
    int idx=0;
    for (int i=0; i<dataY.dim1() ; ++i) {
        if (maxval<dataY[i]) {
            maxval=dataY[i];
            maxpos=idx;
        }
        if (dataY[i]< minval) {
            minval=dataY[i];
            minpos=idx;
        }
        idx++;
    }

    double halfmax=(maxval-minval)/2+minval;
    int HWHM=maxpos;

    for (; HWHM<dataY.dim1(); ++HWHM) {
        if (dataY[HWHM]<halfmax)
            break;
    }

    fitFunction[0]=maxval;
    fitFunction[1]=dataX[maxpos];
    fitFunction[2]=(dataX[HWHM]-dataX[maxpos])*2;
    double d=dataX[1]-dataX[0];
    if (fitFunction[2]<d) {
        logger.warning("Could not find FWHM, using constant 3*dx");
        fitFunction[2]=3*d;
    }

    Nonlinear::LevenbergMarquardt mrqfit(0.001,5000);
    try {


        mrqfit.fit(dataX,dataY,dataSig,fitFunction);

    }
    catch (kipl::base::KiplException &e) {
        logger.error(e.what());
        return ;
    }
    catch (std::exception &e) {
        logger.message(msg.str());
        return ;
    }
    qDebug() << "post fit";
    msg.str("");
    msg<<"Fitted data to "<<fitFunction[0]<<", "<<fitFunction[1]<<", "<<fitFunction[2];

    logger.message(msg.str());
}

void ResolutionEstimator::analyzeMTF()
{
    // TODO Implement MTF
}

void ResolutionEstimator::diffProfile()
{
    for (int i=0; i<profileSize-1; ++i)
        dprofile[i]=profile[i+1]-profile[i];
    dprofile[profileSize-1]=dprofile[profileSize-2];
}

}



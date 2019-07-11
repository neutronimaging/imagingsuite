//<LICENSE>

#include <map>
#include <string>

//#include <QDebug>

#include "resolutionestimators.h"

#include <tnt.h>
#include <strings/miscstring.h>
#include <math/nonlinfit.h>

namespace ImagingQAAlgorithms {
//Nonlinear::SumOfGaussians sog_dummy(1);

ResolutionEstimator::ResolutionEstimator() :
    logger("ResolutionEstimator"),
    profileFunction(Nonlinear::fnSumOfGaussians),
    profileSize(0),
    mPixelSize(0.1),
    mProfile(nullptr),
    mDiffProfile(nullptr),
    mXaxis(nullptr),
    mfwhm(-1.0),
    mFitFunction(1)
{

}

ResolutionEstimator::~ResolutionEstimator()
{
    removeAllocation();
}

void ResolutionEstimator::setPixelSize(double s)
{
    mPixelSize=fabs(s);
}

double ResolutionEstimator::pixelSize()
{
    return mPixelSize;
}

int ResolutionEstimator::size()
{
    return profileSize;
}

void ResolutionEstimator::setProfile(float *p, int N)
{
    createAllocation(N);
    std::copy_n(p,N,mProfile);
    analysis();
}

void ResolutionEstimator::setProfile(double *p, int N)
{
    createAllocation(N);
    std::copy_n(p,N,mProfile);
    analysis();
}

void ResolutionEstimator::setProfile(std::vector<double> &p)
{
    int N=static_cast<int>(p.size());
    createAllocation(N);
    std::copy_n(p.begin(),N,mProfile);

    analysis();
}

void ResolutionEstimator::setProfile(std::vector<float> &p)
{
    size_t N=p.size();
    createAllocation(static_cast<int>(N));
    for (size_t i=0; i<N; ++i)
        mProfile[i]=static_cast<double>(p[i]);
    analysis();
}

void ResolutionEstimator::setProfile(TNT::Array1D<double> &p)
{
    int N=p.dim1();
    createAllocation(N);
    for (int i=0; i<N; ++i)
        mProfile[i]=p[i];


    analysis();
}

void ResolutionEstimator::profile(double *p, int &N)
{
    N=profileSize;
    std::copy_n(mProfile,N,p);
}

void ResolutionEstimator::clear()
{
    removeAllocation();

}

double ResolutionEstimator::FWHM()
{
    return mfwhm;
}

double ResolutionEstimator::MTFresolution(float level)
{
    (void)level;

    return -1.0;
}

void ResolutionEstimator::edgeDerivative(std::vector<double> &x, std::vector<double> &y, bool returnFit, float smooth)
{
    (void) smooth;
    x.clear();
    y.clear();
    if (returnFit==true) {
        for (int i=0; i<profileSize; ++i) {
            x.push_back(i*mPixelSize);
            y.push_back(mFitFunction(i*mPixelSize));
        }
    }
    else {
        for (int i=0; i<profileSize; ++i) {
            x.push_back(i*mPixelSize);
            y.push_back(mDiffProfile[i]);
        }
    }
}

void ResolutionEstimator::MTF(std::vector<double> &w, std::vector<double> &a)
{

}

Nonlinear::SumOfGaussians ResolutionEstimator::fitFunction()
{
    return mFitFunction;
}

void ResolutionEstimator::createAllocation(int N)
{
    removeAllocation();

    mProfile=new double[N];
    mDiffProfile=new double[N];
    mXaxis=new double[N];
    profileSize=N;
}

void ResolutionEstimator::removeAllocation()
{
    if (mProfile!=nullptr)
    {
        delete [] mProfile;
        mProfile=nullptr;
    }

    if (mDiffProfile!=nullptr)
    {
        delete [] mDiffProfile;
        mDiffProfile=nullptr;
    }

    if (mXaxis!=nullptr)
    {
        delete [] mXaxis;
        mXaxis=nullptr;
    }

    profileSize=0;
}

void ResolutionEstimator::prepareXAxis()
{
    for (int i=0; i<profileSize; ++i) {
        mXaxis[i]=mPixelSize*i;
    }
}

void ResolutionEstimator::analysis()
{
   prepareXAxis();
   diffProfile();

   analyzeLineSpread();
   analyzeMTF();
}

void ResolutionEstimator::analyzeLineSpread()
{
    std::ostringstream msg;

    TNT::Array1D<double> dataX(profileSize);
    TNT::Array1D<double> dataY(profileSize);
    TNT::Array1D<double> dataSig(profileSize);

    for (int i=0; i<dataX.dim1(); ++i) {
        dataX[i]=mXaxis[i];
        dataY[i]=mDiffProfile[i];
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

    if ((dataY.dim1()<=maxpos) || (dataY.dim1()<=minpos))
        throw kipl::base::KiplException("Min/max search out of bounds",__FILE__,__LINE__);

    double halfmax=(maxval-minval)/2+minval;
    int HWHM=maxpos;

    for (; HWHM<dataY.dim1(); ++HWHM) {
        if (dataY[HWHM]<halfmax)
            break;
    }

    mFitFunction[0]=maxval;
    mFitFunction[1]=dataX[maxpos];
    mFitFunction[2]=(dataX[HWHM]-dataX[maxpos])*2;

    double d=dataX[1]-dataX[0];
    if (mFitFunction[2]<mPixelSize) {
        logger.warning("Could not find FWHM, using constant 3*dx");
        mFitFunction[2]=3*mPixelSize;
    }

    Nonlinear::LevenbergMarquardt mrqfit(0.001,5000);
    try {
        mrqfit.fit(dataX,dataY,dataSig,mFitFunction);
    }
    catch (kipl::base::KiplException &e) {
        logger.error(e.what());
        return ;
    }
    catch (std::exception &e) {
        logger.error(e.what());
        return ;
    }

    msg.str(""); msg<<"Fitted data to "<<mFitFunction[0]<<", "<<mFitFunction[1]<<", "<<mFitFunction[2];

    logger.message(msg.str());
    mfwhm=2*sqrt(log(2))*mFitFunction[2];
}

void ResolutionEstimator::analyzeMTF()
{
    // TODO Implement MTF
}

void ResolutionEstimator::diffProfile()
{
    for (int i=0; i<profileSize-1; ++i)
        mDiffProfile[i]=0.5*(mProfile[i+1]-mProfile[i]);
    mDiffProfile[profileSize-1]=mDiffProfile[profileSize-2];
}

}



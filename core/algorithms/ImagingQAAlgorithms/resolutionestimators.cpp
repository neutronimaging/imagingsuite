//<LICENSE>

#include <map>
#include <string>

//#include <QDebug>

#include "resolutionestimators.h"

#include <tnt.h>
#include <strings/miscstring.h>
#include <math/nonlinfit.h>
#include <fft/fftbase.h>
#include <fft/zeropadding.h>

namespace ImagingQAAlgorithms {
//Nonlinear::SumOfGaussians sog_dummy(1);

ResolutionEstimator::ResolutionEstimator() :
    logger("ResolutionEstimator"),
    profileFunction(Nonlinear::fnSumOfGaussians),
    profileSize(0),
    mPixelSize(1),
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
    mProfile.resize(N);
    std::copy_n(p,N,mProfile.begin());
    analysis();
}

void ResolutionEstimator::setProfile(double *p, int N)
{
    profileSize = N;
    mProfile.resize(N);
    std::copy_n(p,N,mProfile.begin());
    analysis();
}

void ResolutionEstimator::setProfile(const std::vector<double> &p)
{
    profileSize = p.size();
    mProfile = p;

    analysis();
}

void ResolutionEstimator::setProfile(const std::vector<float> &p)
{
    profileSize = p.size();
    mProfile.resize(p.size());
    std::copy(p.begin(),p.end(),mProfile.begin());
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

void ResolutionEstimator::profile(double *p, size_t &N)
{
    N=profileSize;
    std::copy(mProfile.begin(),mProfile.end(),p);
}

void ResolutionEstimator::clear()
{
    removeAllocation();
}

double ResolutionEstimator::FWHM()
{
    return mfwhm;
}

double ResolutionEstimator::MTFresolution(double level)
{
    // Basic implementation, more precise using polynomial fit
    size_t idx=0;
    for (const auto &amplitude : mMTFamplitude)
    {
        if (amplitude<=level)
            break;

        ++idx;
    }

    return mMTFfrequency[idx];
}

void ResolutionEstimator::edgeDerivative(std::vector<double> &x, std::vector<double> &y, bool returnFit, float smooth)
{
    (void) smooth;
    x.clear();
    y.clear();
    if (returnFit==true) {
        for (size_t i=0; i<profileSize; ++i) {
            x.push_back(i*mPixelSize);
            y.push_back(mFitFunction(i*mPixelSize));
        }
    }
    else {
        for (size_t i=0; i<profileSize; ++i) {
            x.push_back(i*mPixelSize);
            y.push_back(mDiffProfile[i]);
        }
    }
}

const vector<double> & ResolutionEstimator::MTFAmplitude()
{
    return mMTFamplitude;
}

const vector<double> & ResolutionEstimator::MTFFrequency()
{
    return mMTFfrequency;
}

Nonlinear::SumOfGaussians ResolutionEstimator::fitFunction()
{
    return mFitFunction;
}

void ResolutionEstimator::createAllocation(int N)
{
    removeAllocation();

    profileSize=N;
}

void ResolutionEstimator::removeAllocation()
{

    profileSize=0;
}

void ResolutionEstimator::prepareXAxis()
{
    mXaxis.resize(profileSize);
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

//    qDebug() <<dataX[HWHM]<<dataX[maxpos] << mFitFunction[0] << mFitFunction[1] << mFitFunction[2];

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
    size_t dims = kipl::math::fft::NextPower2(profileSize);
    kipl::math::fft::FFTBase transform(&dims,1);

    double *data=new double[dims];
    std::fill_n(data,dims,0.0);
    std::copy(mDiffProfile.begin(),mDiffProfile.end(),data);

    complex<double> *spec = new complex<double>[dims];
    std::fill_n(spec,dims,0.0);
    transform(data,spec);

    mMTFamplitude.resize(dims);
    mMTFfrequency.resize(dims);

    double dW = 0.5/mPixelSize/dims;
    for (size_t i=0; i<dims; ++i)
    {
        mMTFamplitude[i] = std::abs(spec[i])/std::abs(spec[0]);
        mMTFfrequency[i] = i*dW;
    }

}

void ResolutionEstimator::diffProfile()
{
    for (int i=0; i<profileSize-1; ++i)
        mDiffProfile[i]=0.5*(mProfile[i+1]-mProfile[i]);
    mDiffProfile[profileSize-1]=mDiffProfile[profileSize-2];
}

}



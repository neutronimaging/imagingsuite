//<LICENSE>

#include <map>
#include <string>

#include <QDebug>

#include "resolutionestimators.h"

#include <armadillo>
#include <strings/miscstring.h>
#include <math/nonlinfit.h>

namespace ImagingQAAlgorithms {
//Nonlinear::SumOfGaussians sog_dummy(1);

ResolutionEstimator::ResolutionEstimator() :
    logger("ResolutionEstimator"),
    profileFunction(Nonlinear::fnSumOfGaussians),
    profileSize(0),
    mPixelSize(0.1),
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

size_t ResolutionEstimator::size()
{
    return mProfile.size();
}

void ResolutionEstimator::setProfile(float *p, int N)
{
    createAllocation(N);
    std::copy_n(p,N,mProfile.begin());
    analysis();
}

void ResolutionEstimator::setProfile(double *p, int N)
{
    createAllocation(N);
    std::copy_n(p,N,mProfile.begin());
    analysis();
}

void ResolutionEstimator::setProfile(const std::vector<double> &p)
{
    int N=static_cast<int>(p.size());
    createAllocation(N);
    mProfile = p;
    std::copy_n(p.begin(),N,mProfile.begin());

    analysis();
}

void ResolutionEstimator::setProfile(const std::vector<float> &p)
{
    createAllocation(p.size());
    std::copy(p.begin(),p.end(),mProfile.begin());

    analysis();
}

const std::vector<double> & ResolutionEstimator::profile()
{
    return mProfile;
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
    mProfile.resize(N);
    mDiffProfile.resize(N);
    mXaxis.resize(N);
    profileSize=N;
}

void ResolutionEstimator::removeAllocation()
{
    mProfile.clear();
    mDiffProfile.clear();
    mXaxis.clear();
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

    arma::vec dataX(profileSize);
    arma::vec dataY(profileSize);
    arma::vec dataSig(profileSize);

    std::copy(mXaxis.begin(),mXaxis.end(),dataX.begin());
    std::copy(mDiffProfile.begin(),mDiffProfile.end(),dataY.begin());
    std::fill(dataSig.begin(),dataSig.end(),1.0);


    double maxval=-std::numeric_limits<double>::max();
    double minval=std::numeric_limits<double>::max();

    arma::uword maxpos=0;
    arma::uword minpos=0;
    int idx=0;
    for (auto i=0; i<dataY.n_elem ; ++i)
    {
        if (maxval<dataY[i])
        {
            maxval=dataY[i];
            maxpos=idx;
        }
        if (dataY[i]< minval)
        {
            minval=dataY[i];
            minpos=idx;
        }
        idx++;
    }

    if ((dataY.n_elem<=maxpos) || (dataY.n_elem<=minpos))
        throw kipl::base::KiplException("Min/max search out of bounds",__FILE__,__LINE__);

    double halfmax=(maxval-minval)/2+minval;
    int HWHM=maxpos;

    for (; HWHM < dataY.n_elem; ++HWHM)
    {
        if (dataY[HWHM]<halfmax)
            break;
    }

    mFitFunction[0]=maxval;
    mFitFunction[1]=dataX[maxpos];
    mFitFunction[2]=(dataX[HWHM]-dataX[maxpos])*2;

    double d=dataX[1]-dataX[0];
    if (mFitFunction[2]<mPixelSize)
    {
        logger.warning("Could not find FWHM, using constant 3*dx");
        mFitFunction[2]=3*mPixelSize;
    }

    Nonlinear::LevenbergMarquardt mrqfit(0.001,5000);
    try
    {
        mrqfit.fit(dataX,dataY,dataSig,mFitFunction);
    }
    catch (kipl::base::KiplException &e)
    {
        logger.error(e.what());
        return ;
    }
    catch (std::exception &e)
    {
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



//<LICENSE>

#include <map>
#include <string>

#include <QDebug>

#include "resolutionestimators.h"

#include <strings/miscstring.h>

namespace ImagingQAAlgorithms {

ResolutionEstimator::ResolutionEstimator() :
    profileFunction(ResolutionEstimator::LorenzProfile),
    pixelSize(0.1),
    profileSize(0),
    profile(nullptr)
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
    return getPixelSize();
}

void ResolutionEstimator::setProfile(float *p, int N, double d)
{
    (void)d;
    createAllocation(N);
    std::copy_n(p,N,profile);
}

void ResolutionEstimator::setProfile(double *p, int N, double d)
{
    (void)d;
    createAllocation(N);
    std::copy_n(p,N,profile);
}

void ResolutionEstimator::setProfile(std::vector<double> &p, double d)
{
    (void)d;
    size_t N=p.size();
    createAllocation(N);
    for (size_t i=0; i<N; ++i)
        profile[i]=p[i];
}

void ResolutionEstimator::setProfile(TNT::Array1D<double> &p, double d)
{
    (void)d;
    size_t N=p.dim1();
    createAllocation(N);
    for (size_t i=0; i<N; ++i)
        profile[i]=p[i];
}

float ResolutionEstimator::getFWHM()
{
    return -1.0f;
}

float ResolutionEstimator::getMTFresolution(float level)
{
    (void)level;

    return -1.0f;
}

void ResolutionEstimator::getEdgeDerivative(std::vector<double> &x, std::vector<double> &y, float smooth)
{

}

void ResolutionEstimator::getMTF(std::vector<double> &w, std::vector<double> &a)
{

}

void ResolutionEstimator::createAllocation(int N)
{
    removeAllocation();

    profile=new double[N];
    profileSize=N;
}

void ResolutionEstimator::removeAllocation()
{
    if (profile!=nullptr) {
        delete [] profile;
        profileSize=0;
    }
}
}

void string2enum(string &str, ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction &e)
{
    std::string lowstr=kipl::strings::toLower(str);

    std::map<std::string,ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction> convmap;

    convmap["bestprofile"]=ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction::BestProfile;
    convmap["gaussprofile"]=ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction::GaussProfile;
    convmap["lorenzprofile"]=ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction::LorenzProfile;
    convmap["voightprofile"]=ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction::VoightProfile;

    auto it=convmap.find(lowstr);

    if (it==convmap.end())
        throw kipl::base::KiplException("Profile function does not exist",__FILE__,__LINE__);

    e=it->second;


}

std::string enum2string(ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction e)
{
    switch(e) {
    case ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction::BestProfile:
        return "BestProfile";
        break;
    case ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction::GaussProfile:
        return "GaussProfile";
        break;
    case ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction::LorenzProfile:
        return "LorenzProfile";
        break;
    case ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction::VoightProfile:
        return "VoightProfile";
        break;
    }

    return "GaussProfile";
}

ostream &operator<<(ostream &s, ImagingQAAlgorithms::ResolutionEstimator::eProfileFunction e)
{
    std::string str;
    str=enum2string(e);

    s<<str;

    return s;
}

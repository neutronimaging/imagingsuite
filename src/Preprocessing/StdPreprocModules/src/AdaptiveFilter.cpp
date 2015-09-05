//
// AdaptiveFilter.cpp
//
//  Created on: May 25, 2011
//      Author: anders
//
//  Revision information
//    Checked in by $author$
//    Check-in date $date$
//    svn Revision  $revision$
//

//#include "stdafx.h"
#include "../include/AdaptiveFilter.h"

#include <ReconException.h>
#include <ParameterHandling.h>

#include <filters/filter.h>
#include <base/tprofile.h>
#include <math/mathfunctions.h>


AdaptiveFilter::AdaptiveFilter() :
	PreprocModuleBase("AdaptiveFilter"),
	pLUT(NULL),
    m_nFilterSize(7),
	m_fLambda(0.1f),
	m_fSigma(0.01f),
    m_fEccentricityMin(0.3f),
    m_fEccentricityMax(0.7f),
    m_fFilterStrength(0.5f),
    m_fFmax(0.03f)
{}

AdaptiveFilter::~AdaptiveFilter()
{
	if (pLUT!=NULL)
		delete pLUT;
}

int AdaptiveFilter::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	mConfig=config;
    m_fLambda          = GetFloatParameter(parameters,"lambda");
    m_fSigma           = GetFloatParameter(parameters,"sigma");
    m_nFilterSize      = GetIntParameter(parameters,"filtersize");
//    m_fFilterStrength  = GetFloatParameter(parameters,"filterstrength");
//    m_fFmax            = GetFloatParameter(parameters,"fmax");
//    m_fEccentricityMin = GetFloatParameter(parameters,"eccentricitymin");
//    m_fEccentricityMax = GetFloatParameter(parameters,"eccentricitymax");


	if (pLUT!=NULL)
		delete pLUT;
	pLUT=new kipl::math::SigmoidLUT(1024, m_fLambda, m_fSigma);

	return 1;
}

std::map<std::string, std::string> AdaptiveFilter::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["lambda"] = kipl::strings::value2string(m_fLambda);
    parameters["sigma"]  = kipl::strings::value2string(m_fSigma);
    parameters["filtersize"] = kipl::strings::value2string(m_nFilterSize);
//    parameters["filterstrength"]  = kipl::strings::value2string(m_fFilterStrength);
//    parameters["fmax"]            = kipl::strings::value2string(m_fFmax);
//    parameters["eccentricitymin"] = kipl::strings::value2string(m_fEccentricityMin);
//    parameters["eccentricitymax"] = kipl::strings::value2string(m_fEccentricityMax);

    return parameters;
}

int AdaptiveFilter::ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{
	kipl::base::TImage<float,2> slice(img.Dims());

	for (size_t i=0; i<img.Size(2); i++) {
		memcpy(slice.GetDataPtr(),img.GetLinePtr(0,i),slice.Size()*sizeof(float));
		ProcessCore(slice,parameters);
		memcpy(img.GetLinePtr(0,i),slice.GetDataPtr(),slice.Size()*sizeof(float));
	}

	return 0;
}

int AdaptiveFilter::ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters)
{
    return SimpleFilter(img,parameters);
}

int AdaptiveFilter::SimpleFilter(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters)
{

    float k[1024];
    float w=1.0f/static_cast<float>(m_nFilterSize);
    for (int i=0; i<m_nFilterSize; i++)
        k[i]=w;

    size_t dimx[2]={static_cast<size_t>(m_nFilterSize), 1UL};

    kipl::filters::TFilter<float,2> filterx(k,dimx);

    kipl::base::TImage<float,2> smoothx;
    kipl::base::TImage<float,2> weight;

    try {
        smoothx=filterx(img,kipl::filters::FilterBase::EdgeMirror);
    }

    catch (kipl::base::KiplException &e)
    {
        throw ReconException(e.what(),__FILE__, __LINE__);
    }

    float *profile=new float[img.Size(1)];
    kipl::base::verticalprofile2D(img.GetDataPtr(),img.Dims(),profile,true);

    float *pImg    = img.GetDataPtr();
    float *pSmooth = smoothx.GetDataPtr();
    float ws,wo;
    for (size_t i=0; i<img.Size(); i++) {
        ws=(*pLUT)(pSmooth[i]);
        wo=1.0f-ws;

        for (size_t x=0; x<img.Size(0); x++) {
            pImg[x]=wo*pImg[x]+ws*pSmooth[x];
        }
    }

//    for (size_t y=0; y<img.Size(1); y++) {
//        float *pImg    = img.GetLinePtr(y);
//        float *pSmooth = smoothx.GetLinePtr(y);

//        float ws=kipl::math::Sigmoid(profile[y],m_fLambda,m_fSigma);
//        float wo=1.0f-ws;

//        for (size_t x=0; x<img.Size(0); x++) {
//            pImg[x]=wo*pImg[x]+ws*pSmooth[x];
//        }
//    }

    delete [] profile;

	return 0;
}

void AdaptiveFilter::MaxProfile(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,2> &profile)
{
    size_t Nx=img.Size(0);
    size_t Ny=img.Size(1);
    size_t Nz=img.Size(2);

    size_t pdims[2]={Ny,Nz};

    profile.Resize(pdims);

    for (size_t z=0; z<Nz; z++) {
        float *pProfile=profile.GetLinePtr(z);

        for (size_t y=0; y<Ny; y++) {
            float *pImg=img.GetLinePtr(y,z);
            pProfile[y]=pImg[0];
            for (size_t x=1; x<Nx; x++) {
                if (pProfile[y]<pImg[x]) pProfile[y]=pImg[x];
            }
        }
    }

}

void AdaptiveFilter::MinMaxProfile(kipl::base::TImage<float,2> &img, std::vector<float> &minprofile, std::vector<float> &maxprofile)
{
    size_t Nx=img.Size(0);
    size_t Ny=img.Size(1);

    size_t pdims[1]={Ny};

    minprofile.resize(Ny);
    maxprofile.resize(Ny);

    for (size_t y=0; y<Ny; y++) {
        float *pImg=img.GetLinePtr(y);
        minprofile[y]=pImg[0];
        maxprofile[y]=pImg[0];
        for (size_t x=1; x<Nx; x++) {
            if (maxprofile[y]<pImg[x]) maxprofile[y]=pImg[x];
            if (pImg[x]<minprofile[y]) minprofile[y]=pImg[x];
        }
    }
}

void AdaptiveFilter::Eccentricity(std::vector<float> &minprofile, std::vector<float> &maxprofile, std::vector<float> &eprofile)
{
    eprofile.resize(minprofile.size());

    for (size_t i=0; i<minprofile.size(); i++) {
        eprofile[i]=1-minprofile[i]/maxprofile[i];
        eprofile[i]=(eprofile[i]-m_fEccentricityMin)/(m_fEccentricityMax-m_fEccentricityMin);
        eprofile[i]=max(0.0f,eprofile[i]);
        eprofile[i]=max(1.0f,eprofile[i]);
    }
}

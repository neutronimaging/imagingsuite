#include "../include/projectionfilter.h"

//<LICENSE>

#include "../include/ImagingAlgorithms_global.h"

#include "../include/ImagingException.h"
//#include <ReconHelpers.h>

//#include <ParameterHandling.h>

#include <math/mathconstants.h>
#include <math/mathfunctions.h>
#include <strings/miscstring.h>
#include <math/compleximage.h>
#include <base/imagecast.h>
#include <io/io_matlab.h>
#include <visualization/GNUPlot.h>

#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>

#include "../include/projectionfilter.h"

std::ostream & operator<<(std::ostream & s, ImagingAlgorithms::ProjectionFilterType ft)
{
    s<<enum2string(ft);

    return s;
}

void string2enum(const std::string str, ImagingAlgorithms::ProjectionFilterType &ft)
{
    std::string s=kipl::strings::toLower(str);

    if (s=="none")
        ft=ImagingAlgorithms::ProjectionFilterType::ProjectionFilterNone;
    else if (s=="ram-lak")
        ft=ImagingAlgorithms::ProjectionFilterType::ProjectionFilterRamLak;
    else if (s=="shepp-logan")
        ft=ImagingAlgorithms::ProjectionFilterType::ProjectionFilterSheppLogan;
    else if (s=="hanning")
        ft=ImagingAlgorithms::ProjectionFilterType::ProjectionFilterHanning;
    else if (s=="hamming")
        ft=ImagingAlgorithms::ProjectionFilterType::ProjectionFilterHamming;
    else if (s=="butterworth")
        ft=ImagingAlgorithms::ProjectionFilterType::ProjectionFilterButterworth;
    else if (s=="parzen")
        ft=ImagingAlgorithms::ProjectionFilterType::ProjectionFilterParzen;
    else
        throw ImagingException("Could not convert filter string to enum.",__FILE__, __LINE__);
}

std::string enum2string(const ImagingAlgorithms::ProjectionFilterType &ft)
{
    switch (ft)
    {
        case ImagingAlgorithms::ProjectionFilterType::ProjectionFilterNone              : return "None";
        case ImagingAlgorithms::ProjectionFilterType::ProjectionFilterRamLak			: return "Ram-Lak";
        case ImagingAlgorithms::ProjectionFilterType::ProjectionFilterSheppLogan		: return "Shepp-Logan";
        case ImagingAlgorithms::ProjectionFilterType::ProjectionFilterHanning		    : return "Hanning";
        case ImagingAlgorithms::ProjectionFilterType::ProjectionFilterHamming		    : return "Hamming";
        case ImagingAlgorithms::ProjectionFilterType::ProjectionFilterButterworth       : return "Butterworth";
        case ImagingAlgorithms::ProjectionFilterType::ProjectionFilterParzen            : return "Parzen";
        default                                     	                                : return "Undefined filter type";
    };

    return "Undefined filter type";
}

namespace ImagingAlgorithms
{
// Projection filter base
ProjectionFilterBase::ProjectionFilterBase(std::string name,kipl::interactors::InteractionBase *interactor) :
    logger(name),
    m_Interactor(interactor),
    m_FilterType(ProjectionFilterHamming),
    m_fCutOff(0.5f),
    m_fOrder(1),
    m_bUseBias(true),
    m_fBiasWeight(0.1f),
    m_nPaddingDoubler(2),
    nFFTsize(0),
    nImageSize(0),
    bParametersChanged(true)
{
}

void ProjectionFilterBase::setFilter(ImagingAlgorithms::ProjectionFilterType ft, float cutOff, float _order)
{
    if ((cutOff<0.0f) || (0.5f<cutOff))
        throw ImagingException("The cut-off frequency must be in the interval [0,0.5]",__FILE__,__LINE__);

    m_FilterType = ft;
    m_fCutOff    = cutOff;
    m_fOrder     = _order;
    bParametersChanged = true;
}

ImagingAlgorithms::ProjectionFilterType ProjectionFilterBase::filterType()
{
    return m_FilterType;
}

float ProjectionFilterBase::cutOff()
{
    return m_fCutOff;
}

float ProjectionFilterBase::order()
{
    return m_fOrder;
}

void ProjectionFilterBase::setBiasBehavior(bool _useBias, float _biasWeight)
{
    m_bUseBias    = _useBias;
    m_fBiasWeight = _biasWeight;
    bParametersChanged = true;
}

bool ProjectionFilterBase::useBias()
{
    return m_bUseBias;
}

float ProjectionFilterBase::biasWeight()
{
    return m_fBiasWeight;
}

void ProjectionFilterBase::setPaddingDoubler(size_t N)
{
    m_nPaddingDoubler = N;
}

size_t ProjectionFilterBase::paddingDoubler()
{
    return m_nPaddingDoubler;
}

size_t ProjectionFilterBase::currentFFTSize()
{
    return nFFTsize;
}

size_t ProjectionFilterBase::currentImageSize()
{
    return nImageSize;
}

int ProjectionFilterBase::process(kipl::base::TImage<float,2> & img)
{
    if (m_FilterType != ProjectionFilterNone)
    {
        if (img.Size()==0)
            throw ImagingException("Empty projection image",__FILE__,__LINE__);

        if ((img.Size(0) != nImageSize) || bParametersChanged)
            buildFilter(img.Size(0));

        filterProjection(img);
    }

    return 0;
}

int ProjectionFilterBase::process(kipl::base::TImage<float,3> & img)
{
    if (m_FilterType != ProjectionFilterNone)
    {
        if (img.Size()==0)
            throw ImagingException("Empty projection image",__FILE__,__LINE__);

        if ((img.Size(0) != nImageSize) || bParametersChanged)
            buildFilter(img.Size(0));

        kipl::base::TImage<float,2> proj(img.Dims());

        for (size_t i=0; (i<img.Size(2)) && (updateStatus(float(i)/img.Size(2),"ProjectionFilter")==false); ++i)
        {
            std::copy_n(img.GetLinePtr(0,i),proj.Size(),proj.GetDataPtr());
            filterProjection(proj);
            std::copy_n(proj.GetDataPtr(),proj.Size(),img.GetLinePtr(0,i));
        }
    }
    return 0;
}

size_t ProjectionFilterBase::ComputeFilterSize(size_t len)
{
    double e=log(static_cast<double>(len))/log(2.0);

    return static_cast<size_t>(1)<<(static_cast<size_t>(ceil(e))+m_nPaddingDoubler); // Double the padding
}

std::map<std::string, std::string> ProjectionFilterBase::parameters()
{
// todo: fix parameter readout
    std::map<std::string, std::string> parameters;

    parameters["filtertype"]     = enum2string(m_FilterType);
    parameters["cutoff"]         = kipl::strings::value2string(m_fCutOff);
    parameters["order"]          = kipl::strings::value2string(m_fOrder);
    parameters["usebias"]        = m_bUseBias ? "true" : "false";
    parameters["biasweight"]     = kipl::strings::value2string(m_fBiasWeight);
    parameters["paddingdoubler"] = kipl::strings::value2string(m_nPaddingDoubler);

    return parameters;

}

void ProjectionFilterBase::setParameters(const std::map<std::string, std::string> &params)
{
    if (params.count("filtertype"))
        string2enum(params.at("filtertype"),m_FilterType);

    if (params.count("cutoff"))
        m_fCutOff = std::stof(params.at("cutoff"));

    if (0.5f<m_fCutOff)
        throw ImagingException("The cut-off frequency must be in the interval [0,0.5]",__FILE__,__LINE__);

    if (params.count("order"))
        m_fOrder = std::stof(params.at("order"));

    if (params.count("usebias"))
        m_bUseBias = kipl::strings::string2bool(params.at("usebias"));

    if (params.count("biasweight"))
        m_fBiasWeight = std::stof(params.at("biasweight"));

    if (params.count("paddingdoubler"))
        m_nPaddingDoubler = std::stoul(params.at("paddingdoubler"));

}

bool ProjectionFilterBase::updateStatus(float val, const std::string & msg)
{
    if (m_Interactor!=nullptr) {
        return m_Interactor->SetProgress(val,msg);
    }

    return false;
}

//------------------------------------------------------------
// Projection filter w. float
ProjectionFilter::ProjectionFilter(kipl::interactors::InteractionBase *interactor) :
    ProjectionFilterBase("ProjectionFilterSingle",interactor),
    fft(nullptr)
{
}

ProjectionFilter::~ProjectionFilter(void)
{
    if (fft!=nullptr)
        delete fft;
}


void ProjectionFilter::buildFilter(const size_t N)
{
    if (m_FilterType == ProjectionFilterNone)
        return ;

    nImageSize = N;
    nFFTsize=ComputeFilterSize(N);
    const size_t N2=nFFTsize/2;

    if ((fft!=nullptr) && (fft->size(0)!=static_cast<int>(nFFTsize)))
    {
        delete fft;
        fft=nullptr;
    }
    if (fft==nullptr)
        fft=new kipl::math::fft::FFTBaseFloat(&nFFTsize,1);

    mFilter.resize(N2);
    std::fill_n(mFilter.begin(),N2,0.0f);

    std::stringstream msg;
    msg<<"Filter :"<<m_FilterType<<" filter size="<<mFilter.size();
    logger(kipl::logging::Logger::LogVerbose, msg.str());

    if (!((0<m_fCutOff) && (m_fCutOff<=0.5f)))
        throw ImagingException("Cut off frequency is not in valid range",__FILE__,__LINE__);

    float FilterOrder=0.0;
    if (m_FilterType==ProjectionFilterType::ProjectionFilterButterworth)
        FilterOrder=m_fOrder;

    size_t cN2cutoff=static_cast<size_t>(2*m_fCutOff*N2);

    float rampstep=1.0f/((N2-1.0f)); // The extra N2 is for scaling the FFT

    for (size_t i=0; i<N2; i++)
        mFilter[i]=i*rampstep;

    float fstep=1.0f/((cN2cutoff-1.0f)); // The extra N2 is for scaling the FFT
    for (size_t i=0; i<cN2cutoff; i++)
    {
        float w = i * fstep;
        switch (m_FilterType)
        {
            case ProjectionFilterRamLak: break;
            case ProjectionFilterSheppLogan:  mFilter[i]   = static_cast<float>(sin(fPi*w*m_fCutOff))/N2;        break;
            case ProjectionFilterHanning:     mFilter[i]  *= static_cast<float>(0.5f+0.5f*cos(fPi*w*m_fCutOff)); break;
            case ProjectionFilterHamming:     mFilter[i]  *= static_cast<float>(0.54f+0.46f*cos(fPi*w*m_fCutOff)); break;
            case ProjectionFilterButterworth: mFilter[i]  /= static_cast<float>(1.0f+pow(w*m_fCutOff,FilterOrder)); break;
            case ProjectionFilterParzen:
                if ((0.0f<w) && (w<=0.5f*m_fCutOff))
                    mFilter[i]=1.0f - 6*w*w/(m_fCutOff*m_fCutOff)*(1.0f-w/m_fCutOff);
                else if ((0.5f*m_fCutOff<w) && (w<=m_fCutOff))
                    mFilter[i]=2.0f * std::powf(1.0f-w/m_fCutOff,3.0f);
                else
                    mFilter[i]=0.0f;

                break;

            default: break;
        }
    }



    //memset(mFilter.GetDataPtr()+cN2cutoff,0, sizeof(float)*(N2-cN2cutoff));
    std::fill_n(mFilter.begin()+cN2cutoff,N2-cN2cutoff,0.0f);

    if (m_bUseBias==true)
        mFilter[0]=m_fBiasWeight*mFilter[1];

    PreparePadding(nImageSize,nFFTsize);
    logger(kipl::logging::Logger::LogVerbose,"Filter init done");
}

void ProjectionFilter::filterProjection(kipl::base::TImage<float,2> & img)
{
    const size_t nLines=img.Size(1);

    const size_t cnFilterLength=mFilter.size();
    const size_t cnLoopCnt=nFFTsize/2;
    kipl::base::TImage<complex<float>,1> ft1Dimg(&nFFTsize);

  //  float *pFilter=mFilter.GetDataPtr();
    complex<float> *pFTLine=ft1Dimg.GetDataPtr();
    const size_t nLenPad=nFFTsize+16;
    float *pLine=new float[nLenPad];

    for (size_t line=0; line<(nLines); line++)
    {
        std::fill_n(pLine,nLenPad,0.0f);
        std::fill_n(pFTLine,nLenPad,0.0f);

        size_t insert=Pad(img.GetLinePtr(line),img.Size(0),pLine,nLenPad);

        fft->operator()(pLine,pFTLine);

        for (size_t i=0; i< cnFilterLength; i++)
        {
            pFTLine[i]*=mFilter[i];
        }

        fft->operator ()(pFTLine, pLine);

        float *pImg=img.GetLinePtr(line);
        const float scale=fPi/(4.0f*cnLoopCnt);
        for (size_t i=0; i<img.Size(0); i++)
        {
            pImg[i]=pLine[i+insert]*scale;
        }

    }

    delete [] pLine;
}

size_t ProjectionFilter::Pad(float const * const pSrc,
                                   const size_t nSrcLen,
                                   float *pDest,
                                   const size_t nDestLen)
{
    memset(pDest,0,nDestLen*sizeof(float));
    memcpy(pDest+nInsert,pSrc,nSrcLen*sizeof(float));

    for (size_t i=0; i<nInsert; i++)
    {
        pDest[i]=pSrc[0]*mPadData[i];
        pDest[nDestLen-1-i]=pSrc[nSrcLen-1]*mPadData[i];
    }

    return nInsert;
}

void ProjectionFilter::PreparePadding(const size_t nImage, const size_t nFilter)
{
    nInsert=(nFilter/2)-(nImage/2);

    mPadData.Resize(&nInsert);

    for (size_t i=0; i<nInsert; i++)
    {
        float x=static_cast<float>(i)/ static_cast<float>(nInsert) - 0.5f;
        mPadData[i]=kipl::math::Sigmoid(x,0.0f,0.07f);
    }


}
}

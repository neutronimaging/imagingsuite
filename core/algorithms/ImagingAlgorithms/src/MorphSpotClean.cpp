//<LICENSE>
#include <algorithm>
#include <functional>
#include <thread>
#include <morphology/morphextrema.h>
#include <math/image_statistics.h>
#include <morphology/label.h>
#include <math/median.h>
#include <math/mathfunctions.h>
#include <io/io_tiff.h>
#include <segmentation/thresholds.h>
#include <math/sums.h>
#include <strings/miscstring.h>

#include "../include/MorphSpotClean.h"
#include "../include/ImagingException.h"

namespace ImagingAlgorithms
{

MorphSpotClean::MorphSpotClean(kipl::interactors::InteractionBase *interactor) :
    logger("MorphSpotClean"),
    mark(std::numeric_limits<float>::max()),
    m_bUseThreading(true),
    m_nNumberOfThreads(-1),
    m_eConnectivity(kipl::base::conn8),
    m_eMorphClean(MorphCleanReplace),
    m_eMorphDetect(MorphDetectHoles),
    m_seSize(5),
    m_nEdgeSmoothLength(9),
    m_nPadMargin(5),
    m_nMaxArea(100),
    m_bRemoveInfNan(false),
    m_bClampData(false),
    m_bThresholdByPercentage(true),
    m_fMinLevel(-0.1f), // This shouldnt exist...
    m_fMaxLevel(7.0f), // This corresponds to 0.1% transmission
    m_fThreshold{0.95f,0.95f},
    m_fSigma{0.025f,0.025f},
    m_LUT(1<<15,0.1f,0.0075f),
    m_nCounter(0),
    m_interactor(interactor)

{
    setNumberOfThreads(m_nNumberOfThreads);
}


void MorphSpotClean::process(kipl::base::TImage<float,2> &img, float th, float sigma)
{
    if (m_bRemoveInfNan)
        replaceInfNaN(img);

    if (m_bClampData)
         kipl::segmentation::LimitDynamics(img.GetDataPtr(),img.Size(),m_fMinLevel,m_fMaxLevel,false);

    std::fill_n(m_fThreshold.begin(),2,th);
    std::fill_n(m_fSigma.begin(),2,sigma);

    switch (m_eMorphClean) {
        case MorphCleanReplace  : ProcessReplace(img); break;
        case MorphCleanFill     : ProcessFill(img); break;
    default : throw ImagingException("Unkown cleaning method selected", __FILE__,__LINE__);
    }

}

void MorphSpotClean::process(kipl::base::TImage<float,2> &img, std::vector<float> &th, std::vector<float> &sigma)
{
    if (m_bRemoveInfNan)
        replaceInfNaN(img);

    if (m_bClampData)
         kipl::segmentation::LimitDynamics(img.GetDataPtr(),img.Size(),m_fMinLevel,m_fMaxLevel,false);

    m_fThreshold = th;
    m_fSigma     = sigma;

    switch (m_eMorphClean)
    {
        case MorphCleanReplace  : ProcessReplace(img); break;
        case MorphCleanFill     : ProcessFill(img); break;
    default : throw ImagingException("Unkown cleaning method selected", __FILE__,__LINE__);
    }

}

void MorphSpotClean::process(kipl::base::TImage<float, 3> &img, float th, float sigma)
{
    std::vector<float> thvec(2,th);
    std::vector<float> sigvec(2,sigma);

    process(img,thvec,sigvec);
}

void MorphSpotClean::process(kipl::base::TImage<float, 3> &img, std::vector<float> &th, std::vector<float> &sigma)
{
    m_nCounter = 0;
//    logger.message(dumpParameters());
    if (m_bUseThreading)
    {
        logger.message("Multi-threaded processing");
        std::ostringstream msg;
        const size_t N = img.Size(2);
        const size_t concurentThreadsSupported = std::min(m_nNumberOfThreads,static_cast<int>(N));

        std::vector<std::thread> threads;

        size_t M=N/concurentThreadsSupported;

        msg.str("");
        msg<<N<<" projections on "<<concurentThreadsSupported<<" threads, "<<M<<" projections per thread";
        logger.message(msg.str());
        int restCnt = N % concurentThreadsSupported;
        int begin = 0;
        int end   = M + (restCnt>0 ? 1 :0) ;

        auto pImg = &img;
        for (size_t i = 0; i < concurentThreadsSupported; ++i)
        {   // spawn threads
            threads.push_back(std::thread([=] { process(pImg,begin,end, th,sigma,i); }));

            --restCnt;
            begin  = end;
            end   += M + (restCnt>0 ? 1 :0) ;
        }
        msg.str("");
        msg<<"Started "<<threads.size()<<" threads";
        logger.message(msg.str());

        // call join() on each thread in turn
        for_each(threads.begin(), threads.end(),
            std::mem_fn(&std::thread::join));
    }
    else
    {
        logger.message("Single thread processing");
        process(&img,0UL,img.Size(2),th,sigma);
    }
}

void MorphSpotClean::process(kipl::base::TImage<float, 3> *pImg, size_t first, size_t last, std::vector<float> th, std::vector<float> sigma, size_t tid)
{
    std::ignore = tid;
    std::ostringstream msg;

    kipl::base::TImage<float,2> slice(pImg->dims());
    kipl::base::TImage<float,2> orig(pImg->dims());
    msg.str("");
    for (size_t i=first; i<last; ++i)
    {
        std::copy_n(pImg->GetLinePtr(i),slice.Size(),slice.GetDataPtr());
        orig.Clone(slice);
        process(slice,th,sigma);

        std::copy_n(slice.GetDataPtr(), slice.Size(),pImg->GetLinePtr(i));
        size_t cnt=0UL;
        float *pRes=pImg->GetLinePtr(i);
        for (size_t j=0; j<slice.Size(); ++j)
        {
            if (orig[j]!=pRes[j])
                ++cnt;
        }

        msg<<i<<": "<<cnt<<", ";
        ++m_nCounter;
        UpdateStatus(static_cast<float>(m_nCounter.load())/static_cast<float>(pImg->Size(2)),"Morph spot clean");
    }
    logger.message(msg.str());
}

void MorphSpotClean::detectionImage(kipl::base::TImage<float,2> &img,
                                    kipl::base::TImage<float,2> &padded,
                                    kipl::base::TImage<float,2> &noholes,
                                    kipl::base::TImage<float,2> &nopeaks,
                                    bool removeBias)
{
    PadEdges(img,padded);

    switch (m_eMorphDetect)
    {
    case MorphDetectBrightSpots : noholes.FreeImage();
                                  nopeaks  = DetectBrightSpots(padded);
                                  break;

    case MorphDetectDarkSpots   : noholes  = DetectDarkSpots(padded);
                                  nopeaks.FreeImage();
                                  break;

    case MorphDetectAllSpots    : nopeaks  = DetectBrightSpots(padded);
                                  noholes  = DetectDarkSpots(padded);
                                  break;

    case MorphDetectHoles       : noholes  = DetectHoles(padded);
                                  nopeaks.FreeImage();
                                  break;

    case MorphDetectPeaks       : noholes.FreeImage();
                                  nopeaks  = DetectPeaks(padded);  break;

    case MorphDetectBoth        : noholes  = DetectHoles(padded);
                                  nopeaks  = DetectPeaks(padded);  break;

    default: throw ImagingException("Unkown detection method selected", __FILE__,__LINE__);
    }

    if (removeBias)
    {
        if (noholes.Size()== padded.Size())
            for (size_t i=0 ; i<noholes.Size(); ++i)
                noholes[i]=abs(noholes[i]-padded[i]);

        if (nopeaks.Size()== padded.Size())
            for (size_t i=0 ; i<nopeaks.Size(); ++i)
                nopeaks[i]=abs(nopeaks[i]-padded[i]);
    }
}

void MorphSpotClean::ProcessReplace(kipl::base::TImage<float,2> &img)
{
    kipl::base::TImage<float,2> padded, nopeaks, noholes;

    detectionImage(img,padded,noholes,nopeaks,false);

    std::ostringstream msg;

    std::vector<float> threshold = m_fThreshold;
    std::vector<float> sigma     = m_fSigma;

    if (m_bThresholdByPercentage)
    {
        size_t idx;
        std::vector<size_t> hist;
        std::vector<float> axis;

        if (noholes.Size()==padded.Size())
        {
            auto diff = kipl::math::abs(noholes-padded);
            kipl::base::highEntropyHistogram(diff.GetDataPtr(), diff.Size(),
                                                  1024UL,
                                                  hist, axis,
                                                  0.0f, 0.0f,
                                                  false);
            kipl::base::FindLimit(hist,threshold[0],idx);
            threshold[0] = axis[idx];
        }
        if (nopeaks.Size()==padded.Size())
        {
            auto diff = kipl::math::abs(nopeaks-padded);
            kipl::base::highEntropyHistogram(diff.GetDataPtr(), diff.Size(),
                                                  1024UL,
                                                  hist, axis,
                                                  0.0f, 0.0f,
                                                  false);
            kipl::base::FindLimit(hist,threshold[1],idx);
            threshold[1] = axis[idx];
        }
    }

    sigma[0] = sigma[0]*threshold[0];
    sigma[1] = sigma[1]*threshold[1];

    msg.str("");
    msg<<"Thresholds: Dark="<<threshold[0]<<"("<<sigma[0]<<"), Bright="<<threshold[1]<<"("<<sigma[1]<<")";
    logger.verbose(msg.str());

    float *pImg   = padded.GetDataPtr();
    float *pHoles = noholes.GetDataPtr();
    float *pPeaks = nopeaks.GetDataPtr();


    if ((m_fSigma[0]==0.0f) && (m_fSigma[1]==0.0f))
    {
        for (size_t i=0; i<padded.Size(); i++)
        {
            float val=pImg[i];
            switch (m_eMorphDetect)
            {
            case MorphDetectDarkSpots :
                if (threshold[0]<abs(pHoles[i]-val))
                    pImg[i]=pHoles[i];
                break;

            case MorphDetectBrightSpots :
                if (threshold[1]<abs(pPeaks[i]-val))
                    pImg[i]=pPeaks[i];
                break;

            case MorphDetectAllSpots :
                if (threshold[0]<abs(pHoles[i]-val))
                    pImg[i]=pHoles[i];
                if (threshold[1]<abs(pPeaks[i]-val))
                    pImg[i]=pPeaks[i];
                break;

            case MorphDetectHoles :
                if (threshold[0]<abs(val-pHoles[i]))
                    pImg[i]=pHoles[i];
                break;

            case MorphDetectPeaks :
                if (threshold[1]<abs(pPeaks[i]-val))
                    pImg[i]=pPeaks[i];
                break;


            case MorphDetectBoth :
                if (threshold[0]<abs(val-pHoles[i]))
                    pImg[i]=pHoles[i];
                if (threshold[1]<abs(pPeaks[i]-val))
                    pImg[i]=pPeaks[i];
                break;
            }
        }
    }
    else {
        float dh,dp;
        for (size_t i=0; i<padded.Size(); i++)
        {
            float val=pImg[i];
            switch (m_eMorphDetect)
            {
            case MorphDetectDarkSpots :
                pImg[i]=kipl::math::SigmoidWeights(fabs(pHoles[i]-val),val,pHoles[i],threshold[0],sigma[0]);
                break;
            case MorphDetectBrightSpots :
                pImg[i]=kipl::math::SigmoidWeights(fabs(val-pPeaks[i]),val,pPeaks[i],threshold[1],sigma[1]);
                break;
            case MorphDetectAllSpots :
                dp=fabs(val-pPeaks[i]);
                dh=fabs(pHoles[i]-val);

                if (dh<dp)
                    pImg[i]=kipl::math::SigmoidWeights(dp,val,pPeaks[i],threshold[0],m_fSigma[0]);
                else
                    pImg[i]=kipl::math::SigmoidWeights(dh,val,pHoles[i],threshold[1],m_fSigma[1]);

                break;
            case MorphDetectHoles :
                pImg[i]=kipl::math::SigmoidWeights(pHoles[i]-val,val,pHoles[i],threshold[0],sigma[0]);
                break;

            case MorphDetectPeaks :
                pImg[i]=kipl::math::SigmoidWeights(val-pPeaks[i],val,pPeaks[i],threshold[1],sigma[1]);
                break;

            case MorphDetectBoth :
                dp=val-pPeaks[i];
                dh=pHoles[i]-val;

                if (fabs(dh)<fabs(dp))
                    pImg[i]=kipl::math::SigmoidWeights(dp,val,pPeaks[i],threshold[0],m_fSigma[0]);
                else
                    pImg[i]=kipl::math::SigmoidWeights(dh,val,pHoles[i],threshold[1],m_fSigma[1]);

                break;
            }
        }
    }

    unpadEdges(padded,img);
}

void MorphSpotClean::ProcessFill(kipl::base::TImage<float,2> &img)
{
    kipl::base::TImage<float,2> res;
    kipl::base::TImage<float,2> padded,noholes, nopeaks;

    detectionImage(img,padded,noholes,nopeaks,false);
    size_t N=padded.Size();
    res=padded; res.Clone();

    float *pImg=padded.GetDataPtr();
    float *pRes=res.GetDataPtr();
    float *pHoles=noholes.GetDataPtr();
    float *pPeaks=nopeaks.GetDataPtr();

    kipl::containers::ArrayBuffer<PixelInfo> spots(img.Size());

    float diffH=0.0f;
    float diffP=0.0f;

    for (size_t i=0; i<N; i++) {
        float val=pImg[i];
        switch (m_eMorphDetect)
        {
        case MorphDetectHoles :
            diffH=abs(val-pHoles[i]);
            if (m_fThreshold[0]<diffH)
            {
                spots.push_back(PixelInfo(i,val,kipl::math::Sigmoid(diffH,m_fThreshold[0],m_fSigma[0])));
                pRes[i]=mark;
            }
            break;

        case MorphDetectPeaks :
            diffP=abs(val-pPeaks[i]);
            if (m_fThreshold[0]<diffP)
            {
                spots.push_back(PixelInfo(i,val,kipl::math::Sigmoid(diffP,m_fThreshold[1],m_fSigma[1])));
                pRes[i]=mark;
            }
            break;

        case MorphDetectBoth :
            diffH=abs(val-pHoles[i]);
            diffP=abs(val-pPeaks[i]);

            if ((m_fThreshold[0]<diffH) || (m_fThreshold[1]<diffP))
            {
                spots.push_back(PixelInfo(i,val,
                                          std::min(kipl::math::Sigmoid(diffP,m_fThreshold[0],m_fSigma[0]),
                                                   kipl::math::Sigmoid(diffP,m_fThreshold[1],m_fSigma[1]))));
                pRes[i]=mark;
            }


            break;
        }
    }

    img=CleanByArray(res,&spots);

}

void MorphSpotClean::setConnectivity(kipl::base::eConnectivity conn)
{
    if ((conn!=kipl::base::conn8) && (conn!=kipl::base::conn4))
        throw ImagingException("MorphSpotClean only supports 4- and 8-connectivity",__FILE__,__LINE__);

    m_eConnectivity = conn;
}

kipl::base::eConnectivity MorphSpotClean::connectivity()
{
    return m_eConnectivity;
}

void MorphSpotClean::setCleanMethod(eMorphDetectionMethod mdm, eMorphCleanMethod mcm)
{
    m_eMorphDetect =mdm;
    m_eMorphClean = mcm;
}

eMorphDetectionMethod MorphSpotClean::detectionMethod()
{
    return m_eMorphDetect;
}

eMorphCleanMethod MorphSpotClean::cleanMethod()
{
    return m_eMorphClean;
}



void MorphSpotClean::PadEdges(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &padded)
{
    std::vector<size_t> dims={img.Size(0)+2*m_nPadMargin, img.Size(1)+2*m_nPadMargin};
    padded.resize(dims);
    padded=0.0f;
    for (size_t i=0; i<img.Size(1); ++i)
    {
        copy_n(img.GetLinePtr(i),img.Size(0),padded.GetLinePtr(i+m_nPadMargin)+m_nPadMargin);
    }

    // Top and bottom padding
    for (size_t i=0; i<m_nPadMargin; ++i)
    {
        copy_n(img.GetLinePtr(i),img.Size(0),padded.GetLinePtr(m_nPadMargin-1-i)+m_nPadMargin);
        copy_n(img.GetLinePtr(img.Size(1)-1-i),img.Size(0),padded.GetLinePtr(padded.Size(1)-m_nPadMargin+i)+m_nPadMargin);
    }


    size_t l2=m_nEdgeSmoothLength/2;
    size_t N=padded.Size(0)-l2;
    float *buffer=new float[m_nEdgeSmoothLength];
    // Median filter horizontal upper edge
    float *pEdge=padded.GetLinePtr(0);
    for (size_t i=l2; i<N; ++i)
    {
        std::copy_n(pEdge+i-l2,m_nEdgeSmoothLength,buffer);
        kipl::math::median(buffer,m_nEdgeSmoothLength,pEdge+i);
    }

    // Median filter horizontal bottom edge
    pEdge=padded.GetLinePtr(padded.Size(1)-1);
    for (size_t i=l2; i<N; ++i)
    {
        std::copy_n(pEdge+i-l2,m_nEdgeSmoothLength,buffer);
        kipl::math::median(buffer,m_nEdgeSmoothLength,pEdge+i);
    }

    delete [] buffer;
}

void MorphSpotClean::setLimits(bool bClamp, float fMin, float fMax, int nMaxArea)
{
    m_bClampData = bClamp;
    m_fMinLevel  = fMin;
    m_fMaxLevel  = fMax;

    if (0<nMaxArea)
        m_nMaxArea = nMaxArea;
}

void MorphSpotClean::setThresholdByFraction(bool method)
{
    m_bThresholdByPercentage = method;
}

void MorphSpotClean::setDetectionStrelSize(size_t size)
{
    m_seSize = size;
}

size_t MorphSpotClean::detectionStrelSize()
{
    return m_seSize;
}

std::vector<float> MorphSpotClean::clampLimits()
{
    std::vector<float> limits={m_fMinLevel,m_fMaxLevel};

    return limits;
}

bool MorphSpotClean::clampActive()
{
    return m_bClampData;
}

int MorphSpotClean::maxArea()
{
    return m_nMaxArea;
}

void MorphSpotClean::setCleanInfNan(bool activate)
{
    m_bRemoveInfNan = activate;
}

bool MorphSpotClean::cleanInfNan()
{
    return m_bRemoveInfNan;
}

void MorphSpotClean::setEdgeConditioning(int nSmoothLenght)
{
    if (1<nSmoothLenght)
        m_nEdgeSmoothLength=static_cast<size_t>(nSmoothLenght);

}

int MorphSpotClean::edgeConditionLength()
{
    return static_cast<int>(m_nEdgeSmoothLength);
}

void MorphSpotClean::unpadEdges(kipl::base::TImage<float,2> &padded, kipl::base::TImage<float,2> &img)
{
    for (size_t i=0; i<img.Size(1); i++)
    {
        std::copy_n(padded.GetLinePtr(i+m_nPadMargin)+m_nPadMargin,img.Size(0),img.GetLinePtr(i));
    }
}

pair<kipl::base::TImage<float,2>,kipl::base::TImage<float,2>> MorphSpotClean::detectionImage(kipl::base::TImage<float,2> img, bool removeBias)
{
    kipl::base::TImage<float,2> padded, noholes, nopeaks;

    detectionImage(img,padded,noholes,nopeaks,removeBias);

    auto imgs = make_pair(noholes,nopeaks);

    return imgs;
}

void MorphSpotClean::useThreading(bool x)
{
    m_bUseThreading = x;
}

bool MorphSpotClean::isThreaded()
{
    return m_bUseThreading;
}

string MorphSpotClean::dumpParameters()
{
    std::ostringstream msg;

    msg<<"m_bUseThreading     = "<<kipl::strings::bool2string(m_bUseThreading)<<"\n";
    msg<<"m_nNumberOfThreads  = "<<m_nNumberOfThreads<<"\n";
    msg<<"m_eConnectivity     = "<<m_eConnectivity<<"\n";
    msg<<"m_eMorphClean       = "<<m_eMorphClean<<"\n";
    msg<<"m_eMorphDetect      = "<<m_eMorphDetect<<"\n";
    msg<<"m_seSize            = "<<m_seSize<<"\n";
    msg<<"m_nEdgeSmoothLength = "<<m_nEdgeSmoothLength<<"\n";
    msg<<"m_nPadMargin        = "<<m_nPadMargin<<"\n";
    msg<<"m_nMaxArea          = "<<m_nMaxArea<<"\n";
    msg<<"m_bRemoveInfNan     = "<<m_bRemoveInfNan<<"\n";
    msg<<"m_bClampData        = "<<m_bClampData<<"\n";
    msg<<"m_bThresholdByPercentage = "<<m_bThresholdByPercentage<<"\n";
    msg<<"m_fMinLevel         = "<<m_fMinLevel<<"\n";
    msg<<"m_fMaxLevel         = "<<m_fMaxLevel<<"\n";

    msg<<"m_fThreshold        ";
    for (const auto &x : m_fThreshold)
        msg<<x<<" ";
    msg<<"\nm_fSigma           =";
    for (const auto &x : m_fSigma)
        msg<<x<<" ";

    msg<<"\n";

    return msg.str();
}

void MorphSpotClean::setNumberOfThreads(int N)
{
    int hwMaxThreads = std::thread::hardware_concurrency();

    if ((hwMaxThreads<N) || (N<1))
    {
        m_nNumberOfThreads = hwMaxThreads;
    }
    else
    {
        m_nNumberOfThreads = N;
    }
    std::ostringstream msg;
    msg<<"Using "<<m_nNumberOfThreads<<" threads";
    logger.verbose(msg.str());
}

int MorphSpotClean::numberOfThreads()
{
    return m_nNumberOfThreads;
}

kipl::base::TImage<float,2> MorphSpotClean::DetectHoles(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> noholes;

    noholes = kipl::morphology::FillHole(img,m_eConnectivity);

    return noholes;
}

kipl::base::TImage<float,2> MorphSpotClean::DetectPeaks(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> nopeaks;

    nopeaks = kipl::morphology::FillPeaks(img,m_eConnectivity);

    return nopeaks;
}


kipl::base::TImage<float, 2> MorphSpotClean::DetectBrightSpots(kipl::base::TImage<float, 2> &img)
{
    kipl::base::TImage<float,2> padded, nopeaks;

    padded  = -img;
    nopeaks = kipl::morphology::FillSpot(padded,m_seSize,m_eConnectivity);
    padded  = -nopeaks;

    return padded;
}

kipl::base::TImage<float, 2> MorphSpotClean::DetectDarkSpots(kipl::base::TImage<float, 2> &img)
{
    kipl::base::TImage<float,2> noholes;

    noholes = kipl::morphology::FillSpot(img,m_seSize,m_eConnectivity);

    return noholes;
}

kipl::base::TImage<float,2> MorphSpotClean::DetectSpots(kipl::base::TImage<float,2> img, kipl::containers::ArrayBuffer<PixelInfo> *pixels)
{
    throw ImagingException("Detect spots is not implemented",__FILE__,__LINE__);
    kipl::base::TImage<float,2> result;
    std::ignore = img;
    std::ignore = pixels;
//    auto s=detectionImage(img);

//
//    result.Clone(img);

//    ExcludeLargeRegions(s);


//    float *pWeightDark   = s.first.GetDataPtr();
//    float *pWeightBright = s.second.GetDataPtr();
//    float *pRes          = result.GetDataPtr();

//    for (size_t i=0; i<img.Size(); i++)
//    {
//        if ((pRes[i]<m_fMinLevel) || (m_fMaxLevel<pRes[i]))
//        {
//            pixels->push_back(PixelInfo(i,pRes[i],1.0f));
//            pRes[i]=mark;
//        }
//        else if (pWeightDark[i]!=0)
//        {
//            pixels->push_back(PixelInfo(i,pRes[i],pWeightDark[i]));
//            pRes[i]=mark;
//        }
//        else if (pWeightBright[i]!=0)
//        {

//        }
//    }

//    if (img.Size()<4*pixels->size()) {
//        std::ostringstream msg;

//        msg<<"Detected "<<static_cast<float>(pixels->size())/static_cast<float>(img.Size())<<"pixels. The result may be too smooth.";
//        logger(kipl::logging::Logger::LogWarning,msg.str());
//    }

    return result;
}

void MorphSpotClean::ExcludeLargeRegions(kipl::base::TImage<float,2> &img)
{
    std::ostringstream msg;
    if (m_nMaxArea==0)
        return;

    kipl::base::TImage<float,2> thimg=img;
    thimg.Clone();

    float *pTh=thimg.GetDataPtr();

    for (size_t i=0; i<thimg.Size(); i++)
        pTh[i]= pTh[i]!=0.0f;

    kipl::base::TImage<int,2> lbl;
    size_t N=kipl::morphology::LabelImage(thimg, lbl,m_eConnectivity);

    vector<pair<size_t,size_t> > area;
    vector<size_t> removelist;

    kipl::morphology::LabelArea(lbl,N,area);
    vector<pair<size_t,size_t> >::iterator it;

    for (it=area.begin(); it!=area.end(); it++)
    {
        if (m_nMaxArea<(it->first))
            removelist.push_back(it->second);
    }
    msg<<"Found "<<N<<" regions, "<<removelist.size()<<" are larger than "<<m_nMaxArea;
    logger.message(msg.str());

    kipl::morphology::RemoveConnectedRegion(lbl, removelist, m_eConnectivity);

    int *pLbl=lbl.GetDataPtr();
    float *pImg=img.GetDataPtr();

    for (size_t i=0; i<img.Size(); i++)
        if (pLbl[i]==0)
            pImg[i]=0.0f;

}

void MorphSpotClean::replaceInfNaN(kipl::base::TImage<float, 2> &img)
{
    float *pImg = img.GetDataPtr();

    float maxval=-std::numeric_limits<float>::max();
    vector<size_t> badList;

    for (size_t i=0 ; i<img.Size(); ++i)
    {
        if (std::isfinite(pImg[i]))
        {
            if (maxval<pImg[i]) maxval=pImg[i];
        }
        else
        {
            badList.push_back(i);
        }
    }

    for (auto &idx: badList)
    {
        pImg[idx]=maxval;
    }

}

kipl::base::TImage<float,2> MorphSpotClean::CleanByArray(kipl::base::TImage<float,2> img,
                                                     kipl::containers::ArrayBuffer<PixelInfo> *pixels)
{
    PrepareNeighborhood(img.Size(0),img.Size());

    kipl::containers::ArrayBuffer<PixelInfo > toProcess(img.Size()), corrected(img.Size()), remaining(img.Size());

    float neigborhood[8];
    std::ostringstream msg;
    toProcess.copy(pixels);
    float *pRes=img.GetDataPtr();

    while (!toProcess.empty())
    {
        size_t N=toProcess.size();
        PixelInfo *pixel=toProcess.dataptr();

        for (size_t i=0; i<N; i++)
        {
            // Extract neighborhood values
            int cnt=Neighborhood(pRes,pixel[i].pos,neigborhood);

            if (cnt!=0)
            {
                // Compute replacement value. Here the mean is used, other replacements posible.
                float mean=0.0f;

                for (int j=0; j<cnt; j++)
                    mean+=neigborhood[j];
                mean=mean/cnt;

                pixel[i].value+= pixel[i].weight * (mean - pixel[i].value);
                corrected.push_back(pixel[i]);
            }
            else
            {
                remaining.push_back(pixel[i]);
            }
        }

        if (N!=(corrected.size()+remaining.size()))
            throw ImagingException("List sizes doesn't match in correction loop",__FILE__,__LINE__);

        // Insert the replacements
        PixelInfo *correctedpixel=corrected.dataptr();
        size_t correctedN=corrected.size();

        for (size_t i=0; i<correctedN; i++)
        {
            pRes[correctedpixel[i].pos]=correctedpixel[i].value;
        }

        toProcess.copy(&remaining);
        remaining.reset();
        corrected.reset();
    }

    int cnt=0;
    for (size_t i=0; i<img.Size(); i++)
    {
        if (pRes[i]==mark)
            cnt++;
    }

    if (cnt!=0)
    {
        msg<<"Failed to correct "<<cnt<<" pixels";
        throw ImagingException(msg.str(),__FILE__,__LINE__);
    }
    return img;
}

int MorphSpotClean::PrepareNeighborhood(int dimx, int N)
{
    sx=dimx;
    first_line=sx;
    last_line=N-sx;
    ng[0] = -sx-1;  ng[1] = -sx; ng[2] = -sx+1;
    ng[3] = -1;     ng[4] = 1;
    ng[5] = sx-1;   ng[6] = sx;  ng[7] = sx+1;

    return 0;
}

int MorphSpotClean::Neighborhood(float * pImg, int idx, float * neigborhood)
{
    int cnt=0;
    int start = first_line < idx       ? 0 : (idx!=0 ? 3: 4);
    int stop  = idx        < last_line ? 8 : 5;

    for (int i=start; i<stop ; i++)
    {
        float val=pImg[idx+ng[i]];
        if (val!=mark)
        {
            neigborhood[cnt]=val;
            ++cnt;
        }
    }

    return cnt;
}

//const kipl::base::TImage<float> & MorphSpotClean::getNoHoles()
//{
//    return noholes;
//}
//const kipl::base::TImage<float> & MorphSpotClean::getNoPeaks()
//{
//    return nopeaks;
//}

bool MorphSpotClean::UpdateStatus(float val, std::string msg)
{
    if (m_interactor!=nullptr)
    {
        return m_interactor->SetProgress(val,msg);
    }

    return false;
}

} // End of namespace

//********************************

std::string enum2string(ImagingAlgorithms::eMorphCleanMethod mc)
{

    switch (mc)
    {
    case ImagingAlgorithms::MorphCleanReplace : return "morphcleanreplace"; break;
    case ImagingAlgorithms::MorphCleanFill : return "morphcleanfill"; break;
    default: throw ImagingException("Failed to convert enum to string.",__FILE__,__LINE__);
    }

    return "bad value";
}

std::ostream & operator<<(std::ostream &s, ImagingAlgorithms::eMorphCleanMethod mc)
{
    s<<enum2string(mc);

    return s;
}

void string2enum(std::string str, ImagingAlgorithms::eMorphCleanMethod &mc)
{
    if (str=="morphcleanreplace")
        mc=ImagingAlgorithms::MorphCleanReplace;
    else if (str=="morphcleanfill")
        mc=ImagingAlgorithms::MorphCleanFill;
    else
    {
        std::ostringstream msg;
        msg<<"String ("<<str<<") could not be converted to eMorphCleanMethod";
        throw ImagingException(msg.str(),__FILE__,__LINE__);
    }
}


std::string enum2string(ImagingAlgorithms::eMorphDetectionMethod mc)
{

    switch (mc)
    {
    case ImagingAlgorithms::MorphDetectBrightSpots : return "morphdetectbrightspots"; break;
    case ImagingAlgorithms::MorphDetectDarkSpots   : return "morphdetectdarkspots";   break;
    case ImagingAlgorithms::MorphDetectAllSpots    : return "morphdetectallspots";    break;
    case ImagingAlgorithms::MorphDetectHoles       : return "morphdetectholes";       break;
    case ImagingAlgorithms::MorphDetectPeaks       : return "morphdetectpeaks";       break;
    case ImagingAlgorithms::MorphDetectBoth        : return "morphdetectboth";        break;
    default:
    {
        std::ostringstream msg;
        throw ImagingException("Failed to convert enum to string.",__FILE__,__LINE__);
    }
    }

    return "bad value";
}

std::ostream & operator<<(std::ostream &s, ImagingAlgorithms::eMorphDetectionMethod mc)
{
    s<<enum2string(mc);

    return s;
}

void string2enum(std::string str, ImagingAlgorithms::eMorphDetectionMethod &mc)
{
    std::map<std::string, ImagingAlgorithms::eMorphDetectionMethod> enummap;
    enummap["morphdetectbrightspots"] = ImagingAlgorithms::MorphDetectBrightSpots ;
    enummap["morphdetectdarkspots"]   = ImagingAlgorithms::MorphDetectDarkSpots ;
    enummap["morphdetectallspots"]    = ImagingAlgorithms::MorphDetectAllSpots ;
    enummap["morphdetectholes"]       = ImagingAlgorithms::MorphDetectHoles ;
    enummap["morphdetectpeaks"]       = ImagingAlgorithms::MorphDetectPeaks ;
    enummap["morphdetectboth"]        = ImagingAlgorithms::MorphDetectBoth ;

    try
    {
        mc = enummap.at(str);
    }
    catch (std::out_of_range & e)
    {
        std::ostringstream msg;
        msg<<"String ("<<str<<") could not be converted to eMorphDetectionMethod ("<<e.what()<<")";
        throw ImagingException(msg.str(),__FILE__,__LINE__);
    }
}






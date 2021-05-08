//<LICENSE>
#include <algorithm>
#include <thread>
#include <morphology/morphextrema.h>
#include <math/image_statistics.h>
#include <morphology/label.h>
#include <math/median.h>
#include <math/mathfunctions.h>
#include <io/io_tiff.h>
#include <segmentation/thresholds.h>

#include "../include/MorphSpotClean.h"
#include "../include/ImagingException.h"

#if !defined(NO_QT)
#include <QDebug>
#endif

namespace ImagingAlgorithms {

MorphSpotClean::MorphSpotClean() :
    logger("MorphSpotClean"),
    mark(std::numeric_limits<float>::max()),
    m_bUseThreading(false),
    m_eConnectivity(kipl::base::conn8),
    m_eMorphClean(MorphCleanReplace),
    m_eMorphDetect(MorphDetectHoles),
    m_nEdgeSmoothLength(9),
    m_nPadMargin(5),
    m_nMaxArea(100),
    m_bRemoveInfNan(false),
    m_bClampData(false),
    m_fMinLevel(-0.1f), // This shouldnt exist...
    m_fMaxLevel(7.0f), // This corresponds to 0.1% transmission
    m_fThreshold{0.025f,0.025f},
    m_fSigma{0.00f,0.0f},
    m_LUT(1<<15,0.1f,0.0075f)
{

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

//    qDebug() << "Threshold " << m_fThreshold <<", Sigma "<<m_fSigma;
    switch (m_eMorphClean) {
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
    if (m_bUseThreading)
    {
        throw ImagingException("Threading is not implemented for MorphSpotClean", __FILE__,__LINE__);

        std::ostringstream msg;
        const size_t concurentThreadsSupported = std::thread::hardware_concurrency();


        std::vector<std::thread> threads;
        const size_t N = img.Size(2);

        size_t M=N/concurentThreadsSupported;

        msg.str("");
        msg<<N<<" projections on "<<concurentThreadsSupported<<" threads, "<<M<<" projections per thread";
        logger(logger.LogMessage,msg.str());
        size_t restCnt = N % concurentThreadsSupported;
        for(size_t i = 0; i < concurentThreadsSupported; ++i)
        {
            // spawn threads
            size_t rest=(i==concurentThreadsSupported-1)*(N % concurentThreadsSupported); // Take care of the rest slices
            auto pImg = &img;
            threads.push_back(std::thread([=] { process(pImg,i*M,M+rest,th,sigma,i); }));
        }

        // call join() on each thread in turn
        for_each(threads.begin(), threads.end(),
            std::mem_fn(&std::thread::join));
    }
    else
    {
        process(&img,0UL,img.Size(2),th,sigma);
    }
}

void MorphSpotClean::process(kipl::base::TImage<float, 3> *pImg, size_t first, size_t last, std::vector<float> th, std::vector<float> sigma, size_t tid)
{
    auto & img = *pImg;
    kipl::base::TImage<float,2> slice(img.dims());

    for (size_t i=first; i<last; ++i)
    {
        std::copy_n(img.GetLinePtr(i),slice.Size(),slice.GetDataPtr());
        process(slice,th,sigma);
        std::copy_n(slice.GetDataPtr(), slice.Size(),img.GetLinePtr(i));
    }
}

void MorphSpotClean::FillOutliers(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &padded, kipl::base::TImage<float,2> &noholes, kipl::base::TImage<float,2> &nopeaks)
{
    PadEdges(img,padded);

    switch (m_eMorphDetect) {
    case MorphDetectHoles :
        noholes=kipl::morphology::FillHole(padded,m_eConnectivity);
        break;
    case MorphDetectPeaks :
        nopeaks=kipl::morphology::FillPeaks(padded,m_eConnectivity);
        break;

    case MorphDetectBoth :
        noholes=kipl::morphology::FillHole(padded,m_eConnectivity);

        // Alternative
        for (size_t i=0; i<padded.Size(); i++ )
            padded[i]=-padded[i];
        nopeaks=kipl::morphology::FillHole(padded,m_eConnectivity);
        for (size_t i=0; i<padded.Size(); i++ ) {
            padded[i]=-padded[i];
            nopeaks[i]=-nopeaks[i];
        }
        break;

    default: throw ImagingException("Unkown detection method selected", __FILE__,__LINE__);
    }
}

void MorphSpotClean::ProcessReplace(kipl::base::TImage<float,2> &img)
{
    kipl::base::TImage<float,2> padded,noholes, nopeaks;

    FillOutliers(img,padded,noholes,nopeaks);

    size_t N=padded.Size();
//    qDebug()<<m_fThreshold[0]<<m_fSigma[0]<<m_fThreshold[1]<<m_fSigma[1];

    float *pImg=padded.GetDataPtr();
    float *pHoles=noholes.GetDataPtr();
    float *pPeaks=nopeaks.GetDataPtr();
//    kipl::io::WriteTIFF(nopeaks,"nopeaks.tif",kipl::base::Float32);
//    kipl::io::WriteTIFF(noholes,"noholes.tif",kipl::base::Float32);
//    kipl::io::WriteTIFF(padded,"padded.tif",kipl::base::Float32);
    if ((m_fSigma[0]==0.0f) && (m_fSigma[1]==0.0f))
    {
        for (size_t i=0; i<N; i++) {
            float val=pImg[i];
            switch (m_eMorphDetect) {
            case MorphDetectHoles :
                if (m_fThreshold[0]<abs(val-pHoles[i]))
                    pImg[i]=pHoles[i];
                break;

            case MorphDetectPeaks :
                if (m_fThreshold[1]<abs(pPeaks[i]-val))
                    pImg[i]=pPeaks[i];
                break;

            case MorphDetectBoth :
                if (m_fThreshold[0]<abs(val-pHoles[i]))
                    pImg[i]=pHoles[i];
                if (m_fThreshold[1]<abs(pPeaks[i]-val))
                    pImg[i]=pPeaks[i];
                break;
            }
        }
    }
    else {
        float dh,dp;
        for (size_t i=0; i<N; i++) {
            float val=pImg[i];
            switch (m_eMorphDetect) {
            case MorphDetectHoles :
                  pImg[i]=kipl::math::SigmoidWeights(pHoles[i]-val,val,pHoles[i],m_fThreshold[0],m_fSigma[0]);
                break;

            case MorphDetectPeaks :
                pImg[i]=kipl::math::SigmoidWeights(val-pPeaks[i],val,pPeaks[i],m_fThreshold[1],m_fSigma[1]);
                break;

            case MorphDetectBoth :
                dp=val-pPeaks[i];
                dh=pHoles[i]-val;

                if (fabs(dh)<fabs(dp))
                    pImg[i]=kipl::math::SigmoidWeights(dp,val,pPeaks[i],m_fThreshold[0],m_fSigma[0]);
                else
                    pImg[i]=kipl::math::SigmoidWeights(dh,val,pHoles[i],m_fThreshold[1],m_fSigma[1]);

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

    FillOutliers(img,padded,noholes,nopeaks);
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
        switch (m_eMorphDetect) {
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
    for (size_t i=l2; i<N; ++i) {
        std::copy_n(pEdge+i-l2,m_nEdgeSmoothLength,buffer);
        kipl::math::median(buffer,m_nEdgeSmoothLength,pEdge+i);
    }

    // Median filter horizontal bottom edge
    pEdge=padded.GetLinePtr(padded.Size(1)-1);
    for (size_t i=l2; i<N; ++i) {
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
    for (size_t i=0; i<img.Size(1); i++) {
        std::copy_n(padded.GetLinePtr(i+m_nPadMargin)+m_nPadMargin,img.Size(0),img.GetLinePtr(i));
    }
}

kipl::base::TImage<float,2> MorphSpotClean::detectionImage(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> det_img;

    switch (m_eMorphDetect) {
    case MorphDetectBrightSpots : det_img = DetectBrightSpots(img); break;
    case MorphDetectDarkSpots   : det_img = DetectDarkSpots(img); break;
    case MorphDetectAllSpots    : det_img = DetectAllSpots(img); break;
    case MorphDetectHoles       : det_img = DetectHoles(img);  break;
    case MorphDetectPeaks       : det_img = DetectPeaks(img);  break;
    case MorphDetectBoth        : det_img = DetectBoth(img);   break;
    };

    return det_img;
}

void MorphSpotClean::useThreading(bool x)
{
    m_bUseThreading = x;
}

bool MorphSpotClean::isThreaded()
{
    return m_bUseThreading;
}

kipl::base::TImage<float,2> MorphSpotClean::DetectHoles(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> padded,noholes, detection(img.dims());

    PadEdges(img,padded);

    size_t N=padded.Size();
    float *pImg=padded.GetDataPtr();

    float *pHoles=nullptr;

    noholes=kipl::morphology::FillHole(padded,m_eConnectivity);
    pHoles=noholes.GetDataPtr();

    for (size_t i=0; i<N; i++) {
        pImg[i]=abs(pImg[i]-pHoles[i]);
    }

    unpadEdges(padded,detection);

    return detection;
}

kipl::base::TImage<float,2> MorphSpotClean::DetectPeaks(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> padded, nopeaks, detection(img.dims());

    PadEdges(img,padded);

    size_t N=padded.Size();
    float *pImg=padded.GetDataPtr();

    float *pPeaks=nullptr;

    nopeaks=kipl::morphology::FillPeaks(padded,m_eConnectivity);

    pPeaks=nopeaks.GetDataPtr();

    for (size_t i=0; i<N; i++) {
        pImg[i]=abs(pPeaks[i]-pImg[i]);
    }

    unpadEdges(padded,detection);

    return detection;
}

kipl::base::TImage<float,2> MorphSpotClean::DetectBoth(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> padded,noholes,nopeaks, detection(img.dims());

    PadEdges(img,padded);

    size_t N=padded.Size();
    float *pImg=padded.GetDataPtr();

    float *pHoles=nullptr;
    float *pPeaks=nullptr;

    noholes=kipl::morphology::FillHole(padded,m_eConnectivity);
    nopeaks=kipl::morphology::FillPeaks(padded,m_eConnectivity);
    pHoles=noholes.GetDataPtr();
    pPeaks=nopeaks.GetDataPtr();

    for (size_t i=0; i<N; i++) {
        float val=pImg[i];

        pImg[i]=max(abs(val-pHoles[i]),abs(pPeaks[i]-val));
    }

    unpadEdges(padded,detection);
    return detection;
}

kipl::base::TImage<float, 2> MorphSpotClean::DetectBrightSpots(kipl::base::TImage<float, 2> &img)
{
    kipl::base::TImage<float,2> padded, nopeaks, detection(img.dims());

    PadEdges(img,padded);

    size_t N=padded.Size();
    float *pImg=padded.GetDataPtr();

    float *pPeaks=nullptr;

    padded = -padded;
    nopeaks=kipl::morphology::FillSpot(padded,5,m_eConnectivity);
    nopeaks = -nopeaks;

    pPeaks=nopeaks.GetDataPtr();

    for (size_t i=0; i<N; i++) {
        pImg[i]=abs(pPeaks[i]-pImg[i]);
    }

    unpadEdges(padded,detection);

    return detection;
}

kipl::base::TImage<float, 2> MorphSpotClean::DetectDarkSpots(kipl::base::TImage<float, 2> &img)
{
    kipl::base::TImage<float,2> padded,noholes, detection(img.dims());

    PadEdges(img,padded);

    size_t N=padded.Size();
    float *pImg=padded.GetDataPtr();

    float *pHoles=nullptr;

    noholes=kipl::morphology::FillSpot(padded,5,m_eConnectivity);
    pHoles=noholes.GetDataPtr();

    for (size_t i=0; i<N; i++) {
        pImg[i]=abs(pImg[i]-pHoles[i]);
    }

    unpadEdges(padded,detection);

    return detection;
}



kipl::base::TImage<float,2> MorphSpotClean::DetectSpots(kipl::base::TImage<float,2> img, kipl::containers::ArrayBuffer<PixelInfo> *pixels)
{
    kipl::base::TImage<float,2> s=detectionImage(img);

    kipl::base::TImage<float,2> result=img;
    result.Clone();

    ExcludeLargeRegions(s);

    float *pWeight=s.GetDataPtr();
    float *pRes=result.GetDataPtr();

    for (size_t i=0; i<img.Size(); i++) {
        if ((pRes[i]<m_fMinLevel) || (m_fMaxLevel<pRes[i])) {
            pixels->push_back(PixelInfo(i,pRes[i],1.0f));
            pRes[i]=mark;
        }
        else if (pWeight[i]!=0) {
            pixels->push_back(PixelInfo(i,pRes[i],pWeight[i]));
            pRes[i]=mark;
        }
    }

    if (img.Size()<4*pixels->size()) {
        std::ostringstream msg;

        msg<<"Detected "<<static_cast<float>(pixels->size())/static_cast<float>(img.Size())<<"pixels. The result may be too smooth.";
        logger(kipl::logging::Logger::LogWarning,msg.str());
    }

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

    for (it=area.begin(); it!=area.end(); it++) {
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

    for (size_t i=0 ; i<img.Size(); ++i) {
        if (std::isfinite(pImg[i])) {
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

            if (cnt!=0) {
                // Compute replacement value. Here the mean is used, other replacements posible.
                float mean=0.0f;

                for (int j=0; j<cnt; j++)
                    mean+=neigborhood[j];
                mean=mean/cnt;

                pixel[i].value+= pixel[i].weight * (mean - pixel[i].value);
                corrected.push_back(pixel[i]);
            }
            else {
                remaining.push_back(pixel[i]);
            }
        }

        if (N!=(corrected.size()+remaining.size()))
            throw ImagingException("List sizes doesn't match in correction loop",__FILE__,__LINE__);

        // Insert the replacements
        PixelInfo *correctedpixel=corrected.dataptr();
        size_t correctedN=corrected.size();

        for (size_t i=0; i<correctedN; i++) {
            pRes[correctedpixel[i].pos]=correctedpixel[i].value;
        }

        toProcess.copy(&remaining);
        remaining.reset();
        corrected.reset();
    }

    int cnt=0;
    for (size_t i=0; i<img.Size(); i++) {
        if (pRes[i]==mark)
            cnt++;
    }
    if (cnt!=0) {
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

    for (int i=start; i<stop ; i++) {
        float val=pImg[idx+ng[i]];
        if (val!=mark) {
            neigborhood[cnt]=val;
            cnt++;
        }
    }

    return cnt;
}
}

//********************************

std::string enum2string(ImagingAlgorithms::eMorphCleanMethod mc)
{

    switch (mc) {
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
    if (str=="morphcleanreplace") mc=ImagingAlgorithms::MorphCleanReplace;
    else if (str=="morphcleanfill") mc=ImagingAlgorithms::MorphCleanFill;
    else {
        std::ostringstream msg;
        msg<<"String ("<<str<<") could not be converted to eMorphCleanMethod";
        throw ImagingException(msg.str(),__FILE__,__LINE__);
    }
}


std::string enum2string(ImagingAlgorithms::eMorphDetectionMethod mc)
{

    switch (mc) {
    case ImagingAlgorithms::MorphDetectBrightSpots : return "morphdetectbrightspots"; break;
    case ImagingAlgorithms::MorphDetectDarkSpots   : return "morphdetectdarkspots";   break;
    case ImagingAlgorithms::MorphDetectAllSpots    : return "morphdetectallspots";    break;
    case ImagingAlgorithms::MorphDetectHoles       : return "morphdetectholes";       break;
    case ImagingAlgorithms::MorphDetectPeaks       : return "morphdetectpeaks";       break;
    case ImagingAlgorithms::MorphDetectBoth        : return "morphdetectboth";        break;
    default: throw ImagingException("Failed to convert enum to string.",__FILE__,__LINE__);
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






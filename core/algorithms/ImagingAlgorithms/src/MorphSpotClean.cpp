//<LICENSE>

#include <morphology/morphextrema.h>
#include <math/image_statistics.h>
#include <morphology/label.h>
#include <math/median.h>
#include <math/mathfunctions.h>

#include "../include/MorphSpotClean.h"
#include "../include/ImagingException.h"

namespace ImagingAlgorithms {

MorphSpotClean::MorphSpotClean() :
    logger("MorphSpotClean"),
    mark(std::numeric_limits<float>::max()),
    m_eConnectivity(kipl::morphology::conn8),
    m_eMorphDetect(MorphDetectHoles),
    m_eMorphClean(MorphCleanReplace),
    m_nEdgeSmoothLength(5),
    m_nPadMargin(1),
    m_nMaxArea(100),
    m_fMinLevel(-0.1f),
    m_fMaxLevel(12.0f),
    m_fThreshold(0.025f),
    m_fSigma(0.00f),
    m_LUT(1<<15,0.1f,0.0075f)
{

}


void MorphSpotClean::Process(kipl::base::TImage<float,2> &img, float th, float sigma)
{
    m_fThreshold = th;
    m_fSigma = sigma;

    switch (m_eMorphClean) {
        case MorphCleanReplace  : ProcessReplace(img); break;
        case MorphCleanFill     : ProcessFill(img); break;
    default : throw ImagingException("Unkown cleaning method selected", __FILE__,__LINE__);
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

        // Original
        //nopeaks=kipl::morphology::FillPeaks(padded,m_eConnectivity);

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

    float *pImg=padded.GetDataPtr();
    float *pHoles=noholes.GetDataPtr();
    float *pPeaks=nopeaks.GetDataPtr();
    if (m_fSigma==0.0f)
    {
        for (size_t i=0; i<N; i++) {
            float val=pImg[i];
            switch (m_eMorphDetect) {
            case MorphDetectHoles :
                if (m_fThreshold<abs(val-pHoles[i]))
                    pImg[i]=pHoles[i];
                break;

            case MorphDetectPeaks :
                if (m_fThreshold<abs(pPeaks[i]-val))
                    pImg[i]=pPeaks[i];
                break;

            case MorphDetectBoth :
                if (m_fThreshold<abs(val-pHoles[i]))
                    pImg[i]=pHoles[i];
                if (m_fThreshold<abs(pPeaks[i]-val))
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
                pImg[i]=kipl::math::SigmoidWeights(fabs(val-pHoles[i]),val,pHoles[i],m_fThreshold,m_fSigma);
                break;

            case MorphDetectPeaks :
                pImg[i]=kipl::math::SigmoidWeights(fabs(pPeaks[i]-val),val,pPeaks[i],m_fThreshold,m_fSigma);
                break;

            case MorphDetectBoth :
                dp=fabs(pPeaks[i]-val);
                dh=fabs(val-pHoles[i]);

                if (dh<dp)
                    pImg[i]=kipl::math::SigmoidWeights(dp,val,pPeaks[i],m_fThreshold,m_fSigma);
                else
                    pImg[i]=kipl::math::SigmoidWeights(dh,val,pHoles[i],m_fThreshold,m_fSigma);

                break;
            }
        }
    }

    UnpadEdges(padded,img);
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
    float diff=0.0f;
    for (size_t i=0; i<N; i++) {
        float val=pImg[i];
        switch (m_eMorphDetect) {
        case MorphDetectHoles :
            diff=abs(val-pHoles[i]);
            break;

        case MorphDetectPeaks :
            diff=abs(val-pPeaks[i]);
            break;

        case MorphDetectBoth :
            diff=max(abs(val-pHoles[i]),abs(val-pPeaks[i]));
            break;
        }

        if (m_fThreshold<diff) {
            spots.push_back(PixelInfo(i,val,kipl::math::Sigmoid(diff,m_fThreshold,m_fSigma)));
            pRes[i]=mark;
        }
    }

    img=CleanByArray(res,&spots);

}

void MorphSpotClean::setConnectivity(kipl::morphology::MorphConnect conn)
{
    if ((conn!=kipl::morphology::conn8) && (conn!=kipl::morphology::conn4))
        throw ImagingException("MorphSpotClean only supports 4- and 8-connectivity",__FILE__,__LINE__);

    m_eConnectivity = conn;
}

void MorphSpotClean::setCleanMethod(eMorphDetectionMethod mdm, eMorphCleanMethod mcm)
{
    m_eMorphDetect =mdm;
    m_eMorphClean = mcm;
}

void MorphSpotClean::PadEdges(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &padded)
{
    size_t dims[2]={img.Size(0)+2*m_nPadMargin, img.Size(1)+2*m_nPadMargin};
    padded.Resize(dims);
    size_t last=padded.Size(0);
    size_t nLine=img.Size(0);

    for (size_t i=0; i<img.Size(1); i++) {
        float *pPad=padded.GetLinePtr(i+1);
        memcpy(pPad+m_nPadMargin,img.GetLinePtr(i),nLine*sizeof(float));
        if (m_nPadMargin==1) {
            pPad[0]    = pPad[1];
            pPad[last-1] = pPad[last-2];
        }
    }

    memcpy(padded.GetLinePtr(0)+1,img.GetLinePtr(0),nLine*sizeof(float));
    memcpy(padded.GetLinePtr(img.Size(1))+1,img.GetLinePtr(img.Size(1)-1),nLine*sizeof(float));

    int l2=m_nEdgeSmoothLength/2;
    int N=padded.Size(0)-l2;
    float *buffer=new float[m_nEdgeSmoothLength];
    // Median filter horizontal upper edge
    float *pLine=padded.GetLinePtr(1);
    float *pEdge=padded.GetLinePtr(0);
    for (int i=l2; i<N; i++) {
        memcpy(buffer,pLine+i-l2,m_nEdgeSmoothLength*sizeof(float));
        kipl::math::median_quick_select(buffer,m_nEdgeSmoothLength,pEdge+i);
    }

    // Median filter horizontal bottom edge
    pLine=padded.GetLinePtr(padded.Size(1)-2);
    pEdge=padded.GetLinePtr(padded.Size(1)-1);
    for (int i=l2; i<N; i++) {
        memcpy(buffer,pLine+i-l2,m_nEdgeSmoothLength*sizeof(float));
        kipl::math::median_quick_select(buffer,m_nEdgeSmoothLength,pEdge+i);
    }

    delete [] buffer;
}

void MorphSpotClean::setLimits(float fMin,float fMax, int nMaxArea)
{
    m_fMinLevel = fMin;
    m_fMaxLevel = fMax;
    if (0<nMaxArea)
        m_nMaxArea = nMaxArea;
}

void MorphSpotClean::setEdgeConditioning(int nSmoothLenght)
{
    if (1<nSmoothLenght)
        m_nEdgeSmoothLength=nSmoothLenght;

}

void MorphSpotClean::UnpadEdges(kipl::base::TImage<float,2> &padded, kipl::base::TImage<float,2> &img)
{
    for (size_t i=0; i<img.Size(1); i++) {
        float *pPad=padded.GetLinePtr(i+1);
        memcpy(img.GetLinePtr(i),pPad+1,img.Size(0)*sizeof(float));
    }
}

kipl::base::TImage<float,2> MorphSpotClean::DetectionImage(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> det_img;

    switch (m_eMorphDetect) {
    case MorphDetectHoles    : det_img = DetectHoles(img);  break;
    case MorphDetectPeaks    : det_img = DetectPeaks(img);  break;
    case MorphDetectBoth     : det_img = DetectBoth(img);   break;
    };

    return det_img;
}

kipl::base::TImage<float,2> MorphSpotClean::DetectHoles(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> padded,noholes, detection(img.Dims());

    PadEdges(img,padded);

    size_t N=padded.Size();
    float *pImg=padded.GetDataPtr();

    float *pHoles=nullptr;

    noholes=kipl::morphology::FillHole(padded,m_eConnectivity);
    pHoles=noholes.GetDataPtr();

    for (size_t i=0; i<N; i++) {
        pImg[i]=abs(pImg[i]-pHoles[i]);
    }

    UnpadEdges(padded,detection);

    return detection;
}

kipl::base::TImage<float,2> MorphSpotClean::DetectPeaks(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> padded, nopeaks, detection(img.Dims());

    PadEdges(img,padded);

    size_t N=padded.Size();
    float *pImg=padded.GetDataPtr();

    float *pPeaks=NULL;

    nopeaks=kipl::morphology::FillPeaks(padded,m_eConnectivity);

    pPeaks=nopeaks.GetDataPtr();

    for (size_t i=0; i<N; i++) {
        pImg[i]=abs(pPeaks[i]-pImg[i]);
    }

    UnpadEdges(padded,detection);

    return detection;
}

kipl::base::TImage<float,2> MorphSpotClean::DetectBoth(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> padded,noholes,nopeaks, detection(img.Dims());

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

    UnpadEdges(padded,detection);
    return detection;
}

kipl::base::TImage<float,2> MorphSpotClean::DetectSpots(kipl::base::TImage<float,2> img, kipl::containers::ArrayBuffer<PixelInfo> *pixels)
{
    kipl::base::TImage<float,2> s=DetectionImage(img);

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
    size_t N=LabelImage(thimg, lbl,m_eConnectivity);

    vector<pair<size_t,size_t> > area;
    vector<size_t> removelist;

    kipl::morphology::LabelArea(lbl,N,area);
    vector<pair<size_t,size_t> >::iterator it;

    for (it=area.begin(); it!=area.end(); it++) {
        if (m_nMaxArea<(it->first))
            removelist.push_back(it->second);
    }
    msg<<"Found "<<N<<" regions, "<<removelist.size()<<" are larger than "<<m_nMaxArea;
    logger(kipl::logging::Logger::LogVerbose,msg.str());

    RemoveConnectedRegion(lbl, removelist, m_eConnectivity);

    int *pLbl=lbl.GetDataPtr();
    float *pImg=img.GetDataPtr();

    for (size_t i=0; i<img.Size(); i++)
        if (pLbl[i]==0)
            pImg[i]=0.0f;

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
    case ImagingAlgorithms::MorphDetectHoles : return "morphdetectholes"; break;
    case ImagingAlgorithms::MorphDetectPeaks : return "morphdetectpeaks"; break;
    case ImagingAlgorithms::MorphDetectBoth  : return "morphdetectboth"; break;
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
    if (str=="morphdetectholes") mc=ImagingAlgorithms::MorphDetectHoles;
    else if (str=="morphdetectpeaks") mc=ImagingAlgorithms::MorphDetectPeaks;
    else if (str=="morphdetectboth") mc=ImagingAlgorithms::MorphDetectBoth;
    else {
        std::ostringstream msg;
        msg<<"String ("<<str<<") could not be converted to eMorphDetectionMethod";
        throw ImagingException(msg.str(),__FILE__,__LINE__);
    }
}

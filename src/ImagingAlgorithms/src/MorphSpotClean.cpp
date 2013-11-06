#include "../include/MorphSpotClean.h"
#include <morphology/morphextrema.h>
#include <math/image_statistics.h>
#include "../include/ImagingException.h"
#include <math/median.h>

namespace ImagingAlgorithms {
MorphSpotClean::MorphSpotClean() :
    m_eConnectivity(kipl::morphology::conn8),
    m_eMorphClean(MorphCleanBoth),
    m_nEdgeSmoothLength(5)
{

}

void MorphSpotClean::Process(kipl::base::TImage<float,2> &img, float th)
{
    kipl::base::TImage<float,2> padded,noholes,nopeaks;

    PadEdges(img,padded);

    size_t N=padded.Size();
    float *pImg=padded.GetDataPtr();

    float *pHoles=NULL;
    float *pPeaks=NULL;

    switch (m_eMorphClean) {
    case MorphCleanHoles :
        noholes=kipl::morphology::FillHole(padded,m_eConnectivity);
        pHoles=noholes.GetDataPtr();
        break;
    case MorphCleanPeaks :
        nopeaks=kipl::morphology::FillPeaks(padded,m_eConnectivity);
        break;

    case MorphCleanBoth :
        noholes=kipl::morphology::FillHole(padded,m_eConnectivity);
        nopeaks=kipl::morphology::FillPeaks(padded,m_eConnectivity);
        pHoles=noholes.GetDataPtr();
        pPeaks=nopeaks.GetDataPtr();
        break;
    }

    for (size_t i=0; i<N; i++) {
        float val=pImg[i];
        switch (m_eMorphClean) {
        case MorphCleanHoles :
            if (th<abs(val-pHoles[i]))
                pImg[i]=pHoles[i];
            break;

        case MorphCleanPeaks :
            if (th<abs(pPeaks[i]-val))
                pImg[i]=pPeaks[i];
            break;

        case MorphCleanBoth :
            if (th<abs(val-pHoles[i]))
                pImg[i]=pHoles[i];
            if (th<abs(pPeaks[i]-val))
                pImg[i]=pPeaks[i];
            break;
        }
    }

    UnpadEdges(padded,img);
}


void MorphSpotClean::setConnectivity(kipl::morphology::MorphConnect conn)
{
    if ((conn!=kipl::morphology::conn8) && (conn!=kipl::morphology::conn4))
        throw ImagingException("MorphSpotClean only supports 4- and 8-connectivity",__FILE__,__LINE__);

    m_eConnectivity = conn;
}

void MorphSpotClean::setCleanMethod(eMorphCleanMethod mcm)
{
    m_eMorphClean = mcm;
}

void MorphSpotClean::PadEdges(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &padded)
{
    size_t dims[2]={img.Size(0)+2, img.Size(1)+2};
    padded.Resize(dims);
    size_t last=padded.Size(0);
    size_t nLine=img.Size(0);

    for (size_t i=0; i<img.Size(1); i++) {
        float *pPad=padded.GetLinePtr(i+1);
        memcpy(pPad+1,img.GetLinePtr(i),nLine*sizeof(float));
        pPad[0]    = pPad[1];
        pPad[last-1] = pPad[last-2];
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

void MorphSpotClean::UnpadEdges(kipl::base::TImage<float,2> &padded, kipl::base::TImage<float,2> &img)
{
    for (size_t i=0; i<img.Size(1); i++) {
        float *pPad=padded.GetLinePtr(i+1);
        memcpy(img.GetLinePtr(i),pPad+1,img.Size(0)*sizeof(float));
    }
}
}

//********************************

std::string enum2string(ImagingAlgorithms::eMorphCleanMethod mc)
{

    switch (mc) {
    case ImagingAlgorithms::MorphCleanHoles : return "morphcleanholes"; break;
    case ImagingAlgorithms::MorphCleanPeaks : return "morphcleanpeaks"; break;
    case ImagingAlgorithms::MorphCleanBoth  : return "morphcleanboth"; break;
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
    if (str=="morphcleanholes") mc=ImagingAlgorithms::MorphCleanHoles;
    else if (str=="morphcleanpeaks") mc=ImagingAlgorithms::MorphCleanPeaks;
    else if (str=="morphcleanboth") mc=ImagingAlgorithms::MorphCleanBoth;
    else throw ImagingException("String could not be converted to eMorphCleanMethod",__FILE__,__LINE__);
}

//<LICENSE>

#include <tnt_array1d.h>
#include <tnt_array2d.h>
#include <jama_lu.h>
#include <jama_qr.h>

#include <base/thistogram.h>
#include <filters/filter.h>
#include <math/mathconstants.h>
#include <math/circularhoughtransform.h>
#include <morphology/morphextrema.h>
#include <morphology/label.h>
#include <io/io_tiff.h>
#include <fstream>

#include "contrastsampleanalysis.h"
namespace ImagingQAAlgorithms {
ContrastSampleAnalysis::ContrastSampleAnalysis() :
    logger("ContrastSampleAnalysis"),
    saveIntermediateImages(false),
    metricInsetDiameter(6.0f),
    pixelsize(0.1f),
    hist_size(1024),
    hist_axis(nullptr),
    hist_bins(nullptr)
{
    createAllocation();
}

ContrastSampleAnalysis::~ContrastSampleAnalysis()
{
    clearAllocation();
}

void ContrastSampleAnalysis::clearAllocation()
{
    if (hist_axis!=nullptr) {
        delete [] hist_axis;
        hist_axis=nullptr;
    }

    if (hist_bins!=nullptr) {
        delete [] hist_bins;
        hist_bins=nullptr;
    }
}

void ContrastSampleAnalysis::createAllocation()
{
    clearAllocation();

    hist_axis=new float[hist_size];
    hist_bins=new size_t[hist_size];

}

void ContrastSampleAnalysis::setImage(kipl::base::TImage<float,2> img)
{
   m_Img2D.Clone(img);
   makeHistogram();
   if (saveIntermediateImages)
       kipl::io::WriteTIFF32(m_Img2D,"csa_2d_orig.tif");
}

void ContrastSampleAnalysis::setImage(kipl::base::TImage<float,3> img)
{
    m_Img3D.Clone(img);

//    logger(logger.LogMessage,"Compute histogram");
//    kipl::base::Histogram(m_Img3D.GetDataPtr(),m_Img3D.Size(),hist_bins, hist_size,0.0f,0.0f,hist_axis);

    logger(logger.LogMessage,"Average slice");
    m_Img2D.Resize(m_Img3D.Dims());
    for (size_t slice=0; slice<m_Img3D.Size(2); ++slice)
    {
        float *pSlice=m_Img3D.GetLinePtr(0,slice);
        for (size_t pix=0; pix<m_Img2D.Size(); ++pix) {
            m_Img2D[pix]+=pSlice[pix];
        }
    }

    for (size_t pix=0; pix<m_Img2D.Size(); ++pix) {
        m_Img2D[pix]/=m_Img3D.Size(2);
    }

    makeHistogram();

    if (saveIntermediateImages)
        kipl::io::WriteTIFF32(m_Img2D,"csa_2d_orig.tif");
}

void ContrastSampleAnalysis::analyzeContrast(float ps)
{
    findCenters(ps);
    std::ostringstream msg;
    msg<<"Values of insets:";
    for (int i=0; i<6; ++i) {
        m_insetStats.push_back(computeInsetStatistics(m_insetCenters[i],ps));
        msg<<m_insetStats[i].E()<<", s="<<m_insetStats[i].s()<<std::endl;
    }
    logger(logger.LogMessage,msg.str());
}

std::vector<kipl::math::Statistics> ContrastSampleAnalysis::getStatistics()
{
    return m_insetStats;
}

void ContrastSampleAnalysis::findCenters(float ps)
{
    logger(logger.LogMessage,"Find centers");
    kipl::math::CircularHoughTransform cht;

    const float radius = 0.5f*metricInsetDiameter/ps;

    logger(logger.LogMessage,"Circ Hough transform");
    chm=cht(m_Img2D,radius,true);

    if (saveIntermediateImages)
        kipl::io::WriteTIFF32(chm,"csa_cht.tif");

    float *max=std::max_element(chm.GetDataPtr(),chm.GetDataPtr()+chm.Size());
    size_t maxpos=max-chm.GetDataPtr();
    kipl::base::coords3Df maxinset_center;
    maxinset_center.y=maxpos/m_Img2D.Size(0);
    maxinset_center.x=maxpos%m_Img2D.Size(0);

    float threshold=*max-chm[maxpos+size_t(0.05f*radius)];
    std::ostringstream msg;
    msg<<"hmax with h="<<threshold;
    logger(logger.LogMessage,msg.str());
    kipl::morphology::hMax(chm,peaks,threshold, kipl::base::conn4);

    if (saveIntermediateImages)
        kipl::io::WriteTIFF32(peaks,"csa_hmax.tif");

    kipl::base::TImage<float,2> insetpeaks=chm-peaks;

    if (saveIntermediateImages)
        kipl::io::WriteTIFF32(insetpeaks,"csa_insetpeaks.tif");

    for (size_t i=0; i<insetpeaks.Size(); ++i)
        insetpeaks[i]=0.5f*threshold<insetpeaks[i];

    if (saveIntermediateImages)
        kipl::io::WriteTIFF32(insetpeaks,"csa_insetpeaks_bi.tif");

    std::vector<pair<float,float>> dots;
    size_t idx=0;
    std::ofstream dotfile("dots.csv");
    for (size_t y=0; y<insetpeaks.Size(1); ++y)
        for (size_t x=0; x<insetpeaks.Size(0); ++x , ++idx) {
            if (insetpeaks[idx]!=0.0f) {
                dots.push_back(make_pair(float(x),float(y)));
                dotfile<<x<<", "<<y<<std::endl;
            }
        }

    msg.str("");
    msg<<"Found "<<dots.size()<<" dots";
    logger(logger.LogMessage,msg.str());
    TNT::Array2D<float> H(dots.size()-1UL,2);
    TNT::Array1D<float> a(dots.size()-1UL);
    std::pair<float,float> xyN=dots.back();
    for (size_t i=0; i<dots.size()-1; ++i) {
        H[i][0]=2*(xyN.first-dots[i].first);
        H[i][1]=2*(xyN.second-dots[i].second);
        a[i]=xyN.first*xyN.first
                +xyN.second*xyN.second
                -dots[i].first*dots[i].first
                -dots[i].second*dots[i].second;
    }

    JAMA::QR<float> qr(H);
    TNT::Array1D<float> parameters;
    parameters = qr.solve(a);
    m_ringCenter.x=parameters[0];
    m_ringCenter.y=parameters[1];
    msg.str("");
    msg<<"Ring center at ["<<parameters[0]<<", "<<parameters[1]<<"]";
    logger(logger.LogMessage,msg.str());

    float phi=atan2f(maxinset_center.x-m_ringCenter.x,maxinset_center.y-m_ringCenter.y);
    msg.str("");
    msg<<"Max intensity at ["<<maxinset_center.x<<", "<<maxinset_center.y<<", phi="<<phi*180.0f/fPi;
    logger(logger.LogDebug,msg.str());

    msg.str("");
    msg<<"Dot centers at:"<<std::endl;
    float r=hypotf(maxinset_center.x-m_ringCenter.x,maxinset_center.y-m_ringCenter.y);
    for (int i=0 ; i<6 ; i++) {
        kipl::base::coords3Df coord;

        coord.y=r*cos(phi+i*fPi/3)+m_ringCenter.y;
        coord.x=r*sin(phi+i*fPi/3)+m_ringCenter.x;
        m_insetCenters.push_back(coord);

        msg<<coord.x<<", "<<coord.y<<std::endl;
    }
    logger(logger.LogDebug,msg.str());
}

kipl::math::Statistics ContrastSampleAnalysis::computeInsetStatistics(kipl::base::coords3Df pos,float ps)
{
    kipl::math::Statistics stats;
    stats.reset();
    const float fRadius = 0.4f*metricInsetDiameter/ps; // Reduced radius to avoid edge effects
    const int nRadius = static_cast<int>(floor(fRadius));
    for (int y=-nRadius; y<=nRadius; ++y) {
        int wx=floor(sqrt(fRadius*fRadius-y*y));
        float *pImg=m_Img2D.GetLinePtr(int(y+pos.y))+int(pos.x);
        for (int x=-wx; x<=wx; ++x) {
            stats.put(pImg[x]);
        }
    }

    return stats;
}

int ContrastSampleAnalysis::getHistogramSize()
{
    return hist_size;
}

int ContrastSampleAnalysis::getHistogram(float *axis, size_t *bins)
{
    std::copy(hist_axis,hist_axis+hist_size,axis);
    std::copy(hist_bins,hist_bins+hist_size,bins);
    return hist_size;
}

void ContrastSampleAnalysis::makeHistogram()
{
    std::ostringstream msg;
    msg<<"Compute histogram (size="<<m_Img2D.Size()<<", #bins="<<hist_size;
    logger(logger.LogMessage,msg.str());
    kipl::base::Histogram(m_Img2D.GetDataPtr(),m_Img2D.Size(),hist_bins,hist_size,0.0f,0.0f,hist_axis);
    logger.message("Histogram ready");
}

}

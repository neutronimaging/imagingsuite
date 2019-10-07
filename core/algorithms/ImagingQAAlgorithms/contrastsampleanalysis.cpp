//<LICENSE>

#include <tnt_array1d.h>
#include <tnt_array2d.h>
#include <jama_lu.h>
#include <jama_qr.h>

#include <base/thistogram.h>
#include <base/tsubimage.h>
#include <filters/filter.h>
#include <math/mathconstants.h>
#include <math/circularhoughtransform.h>
#include <morphology/morphextrema.h>
#include <morphology/label.h>
#include <io/io_tiff.h>
#include <fstream>
#include <profile/Timer.h>
#include <filters/medianfilter.h>
#include <base/KiplException.h>
#include <stltools/stlvecmath.h>

#include "contrastsampleanalysis.h"
namespace ImagingQAAlgorithms {
ContrastSampleAnalysis::ContrastSampleAnalysis() :
    logger("ContrastSampleAnalysis"),
    saveIntermediateImages(false),
    metricInsetDiameter(6.0f),
    pixelsize(0.1f),
    hist_size(1024),
    filterSize(5)
{
    createAllocation();
}

ContrastSampleAnalysis::~ContrastSampleAnalysis()
{
    clearAllocation();
}

void ContrastSampleAnalysis::clearAllocation()
{

}

void ContrastSampleAnalysis::createAllocation()
{
    clearAllocation();

}

void ContrastSampleAnalysis::setImage(kipl::base::TImage<float,2> img)
{
   m_Img2D.Clone(img);
   if (1UL<filterSize)
   {
       size_t fltdims[]={filterSize,filterSize};
       kipl::filters::TMedianFilter<float,2> flt(fltdims);
       m_Img2D = flt(m_Img2D);
   }
   makeHistogram();
   if (saveIntermediateImages)
       kipl::io::WriteTIFF32(m_Img2D,"csa_2d_orig.tif");
}

void ContrastSampleAnalysis::setImage(kipl::base::TImage<float,3> img)
{
    m_Img3D.Clone(img);

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

    if (1UL<filterSize)
    {
        size_t fltdims[]={filterSize,filterSize};
        kipl::filters::TMedianFilter<float,2> flt(fltdims);
        m_Img2D = flt(m_Img2D);
    }
    makeHistogram();

    if (saveIntermediateImages)
        kipl::io::WriteTIFF32(m_Img2D,"csa_2d_orig.tif");
}

void ContrastSampleAnalysis::analyzeContrast(float pixelSize, const std::list<kipl::base::RectROI> &ROIs)
{
    circleTransform(pixelSize);

    if (ROIs.size()<3)
    {
        logger.message("Less than 3 ROIs provided. Using slow algorithm");
        findCenters();
    }
    else
        findCenters(ROIs);

    estimateInsetRing();
    std::ostringstream msg;
    msg<<"Values of insets:";
    for (int i=0; i<6; ++i) {
        m_insetStats.push_back(computeInsetStatistics(m_insetCenters[i],pixelSize));
        msg<<m_insetStats[i].E()<<", s="<<m_insetStats[i].s()<<std::endl;
    }
    logger(logger.LogMessage,msg.str());
}

std::vector<kipl::math::Statistics> ContrastSampleAnalysis::getStatistics()
{
    return m_insetStats;
}

void ContrastSampleAnalysis::findCenters(const std::list<kipl::base::RectROI> &ROIs)
{
    std::ostringstream msg;
    logger.message("Find centers");

    kipl::profile::Timer timer;

    float *max=std::max_element(chm.GetDataPtr(),chm.GetDataPtr()+chm.Size());
    size_t maxpos=max-chm.GetDataPtr();
    kipl::base::coords3Df maxinset_center;
    maxinset_center.y=maxpos/m_Img2D.Size(0);
    maxinset_center.x=maxpos%m_Img2D.Size(0);

    float threshold=*max-chm[maxpos+size_t(0.05f*radius)];

    msg.str("");
    msg<<"hmax with h="<<threshold;
    logger(logger.LogMessage,msg.str());

    dots.clear();

    timer.Reset();
    timer.Tic();
    for (const auto &roi : ROIs)
    {
        kipl::base::TImage<float,2> chmCrop=kipl::base::TSubImage<float,2>::Get(chm, roi.box());
        kipl::morphology::hMax(chmCrop,peaks,threshold, kipl::base::conn4);
        size_t idx=0;

        insetpeaks=chmCrop-peaks;

        for (size_t i=0; i<insetpeaks.Size(); ++i)
            insetpeaks[i]=0.5f*threshold<insetpeaks[i];

        for (size_t y=0; y<insetpeaks.Size(1); ++y)
        {
            for (size_t x=0; x<insetpeaks.Size(0); ++x , ++idx)
            {
                if (insetpeaks[idx]!=0.0f)
                {
                    dots.push_back(make_pair(float(x+roi.box()[0]),float(y+roi.box()[1])));
                }
            }
        }

    }
    timer.Toc();
    msg.str(""); msg<<"hMax timing: "<<timer;
    qDebug() << msg.str().c_str();

}

void ContrastSampleAnalysis::findCenters()
{
    std::ostringstream msg;
    logger.message("Find centers");
    qDebug() << "Find centers";


    float *max=std::max_element(chm.GetDataPtr(),chm.GetDataPtr()+chm.Size());
    size_t maxpos=max-chm.GetDataPtr();

    m_maxInsetCenter.y=maxpos/m_Img2D.Size(0);
    m_maxInsetCenter.x=maxpos%m_Img2D.Size(0);
    float threshold=*max-chm[maxpos+size_t(0.05f*radius)];

    msg.str("");
    msg<<"hmax with h="<<threshold;
    logger(logger.LogMessage,msg.str());

    kipl::profile::Timer timer;
    timer.Tic();
    kipl::morphology::hMax(chm,peaks,threshold, kipl::base::conn4);
    timer.Toc();
    msg.str(""); msg<<"hMax timing: "<<timer;
    qDebug() << msg.str().c_str();

    if (saveIntermediateImages)
        kipl::io::WriteTIFF32(peaks,"csa_hmax.tif");

    insetpeaks=chm-peaks;

    if (saveIntermediateImages)
        kipl::io::WriteTIFF32(insetpeaks,"csa_insetpeaks.tif");

    for (size_t i=0; i<insetpeaks.Size(); ++i)
        insetpeaks[i]=0.5f*threshold<insetpeaks[i];

    if (saveIntermediateImages)
        kipl::io::WriteTIFF32(insetpeaks,"csa_insetpeaks_bi.tif");
    timer.Reset();
    timer.Tic();

    size_t idx=0;
    std::ofstream dotfile("dots.csv");

    dots.clear();
    for (size_t y=0; y<insetpeaks.Size(1); ++y)
    {
        for (size_t x=0; x<insetpeaks.Size(0); ++x , ++idx)
        {
            if (insetpeaks[idx]!=0.0f)
            {
                dots.push_back(make_pair(float(x),float(y)));
                dotfile<<x<<", "<<y<<std::endl;
            }
        }
    }
}

void ContrastSampleAnalysis::estimateInsetRing()
{
    std::ostringstream msg;

    msg.str("");
    msg<<"Found "<<dots.size()<<" dots";
    logger(logger.LogMessage,msg.str());
    qDebug() << msg.str().c_str();
    if (dots.size()<5)
        throw kipl::base::KiplException("Too few dots to estimate the ring parameters.");
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

    kipl::profile::Timer timer;
    timer.Toc();

    msg.str(""); msg<<"Circle fitting timing: "<<timer;
    qDebug() << msg.str().c_str();

    msg.str("");
    msg<<"Ring center at ["<<parameters[0]<<", "<<parameters[1]<<"]";
    logger(logger.LogMessage,msg.str());

    float phi=atan2f(m_maxInsetCenter.x-m_ringCenter.x,m_maxInsetCenter.y-m_ringCenter.y);
    msg.str("");
    msg<<"Max intensity at ["<<m_maxInsetCenter.x<<", "<<m_maxInsetCenter.y<<", phi="<<phi*180.0f/fPi;
    logger(logger.LogDebug,msg.str());

    msg.str("");
    msg<<"Dot centers at:"<<std::endl;
    float r=hypotf(m_maxInsetCenter.x-m_ringCenter.x,m_maxInsetCenter.y-m_ringCenter.y);
    for (int i=0 ; i<6 ; i++) {
        kipl::base::coords3Df coord;

        coord.y=r*cos(phi+i*fPi/3)+m_ringCenter.y;
        coord.x=r*sin(phi+i*fPi/3)+m_ringCenter.x;
        m_insetCenters.push_back(coord);

        msg<<coord.x<<", "<<coord.y<<std::endl;
    }
    logger.debug(msg.str());
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

void ContrastSampleAnalysis::setFilterSize(size_t N)
{
    filterSize=N;
}

int ContrastSampleAnalysis::getHistogramSize()
{
    return hist_size;
}

int ContrastSampleAnalysis::getHistogram(std::vector<float> &axis, std::vector<size_t> &bins)
{
    axis = hist_axis;
    bins = hist_bins;

    return hist_size;
}

void ContrastSampleAnalysis::makeHistogram()
{
    std::ostringstream msg;
    msg<<"Compute histogram (size="<<m_Img2D.Size()<<", #bins="<<hist_size;
    logger(logger.LogMessage,msg.str());
    kipl::base::Histogram(m_Img2D.GetDataPtr(),m_Img2D.Size(),hist_size,hist_bins,hist_axis,0.0f,0.0f,false);
    hist_bins = medianFilter(hist_bins,5UL);

    logger.message("Histogram ready");
}

void ContrastSampleAnalysis::circleTransform(float pixelSize)
{
    std::ostringstream msg;

    logger.message("Circ Hough transform");
    kipl::math::CircularHoughTransform cht;
    radius = 0.5f*metricInsetDiameter/pixelSize;

    kipl::profile::Timer timer;
    timer.Tic();
    chm=cht(m_Img2D,radius,true);
    timer.Toc();
    msg.str(""); msg<<"Circular Hough timing: "<<timer;
    qDebug() << msg.str().c_str();

    if (saveIntermediateImages)
        kipl::io::WriteTIFF32(chm,"csa_cht.tif");
}

}

//<LICENSE>

#include <armadillo>

#include <base/thistogram.h>
#include <base/tsubimage.h>
#include <filters/filter.h>
#include <math/mathconstants.h>
#include <math/image_statistics.h>
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
       std::vector<size_t> fltdims={filterSize,filterSize};
       kipl::filters::TMedianFilter<float,2> flt(fltdims);
       m_Img2D = flt(m_Img2D);
   }
   makeHistogram();
   if (saveIntermediateImages)
       kipl::io::WriteTIFF(m_Img2D,"csa_2d_orig.tif",kipl::base::Float32);
}

void ContrastSampleAnalysis::setImage(kipl::base::TImage<float,3> img)
{
    m_Img3D.Clone(img);

    logger(logger.LogMessage,"Average slice");

    m_Img2D.resize(m_Img3D.dims());
    m_Img2D=0.0f;
    for (size_t slice=0; slice<m_Img3D.Size(2); ++slice)
    {
        float *pSlice=m_Img3D.GetLinePtr(0,slice);
        for (size_t pix=0; pix<m_Img2D.Size(); ++pix)
        {
            m_Img2D[pix]+=pSlice[pix];
        }
    }

    for (size_t pix=0; pix<m_Img2D.Size(); ++pix)
    {
        m_Img2D[pix]/=m_Img3D.Size(2);
    }

//    if (1UL<filterSize)
//    {
//        size_t fltdims[]={filterSize,filterSize};
//        kipl::filters::TMedianFilter<float,2> flt(fltdims);
//        m_Img2D = flt(m_Img2D);
//    }
    makeHistogram();

    if (saveIntermediateImages)
        kipl::io::WriteTIFF(m_Img2D,"csa_2d_orig.tif",kipl::base::Float32);

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
    for (auto &center :m_insetCenters)
    {
        auto stats =computeInsetStatistics(center,pixelSize);
        m_insetStats.push_back(stats);
        msg<<stats.E()<<", s="<<stats.s()<<"\n";
    }
    logger(logger.LogMessage,msg.str());
}

std::vector<kipl::math::Statistics> ContrastSampleAnalysis::getStatistics()
{
    return m_insetStats;
}

std::vector<kipl::base::coords3Df> ContrastSampleAnalysis::getInsetCoordinates()
{
    logger(logger.LogMessage,"Find centers");
    kipl::math::CircularHoughTransform cht;

    radius = 0.5f*metricInsetDiameter/pixelsize;
    logger(logger.LogMessage,"Circ Hough transform");
    return m_insetCenters;
}

kipl::base::coords3Df ContrastSampleAnalysis::centerCoordinate()
{
    return m_ringCenter;
}

void ContrastSampleAnalysis::findCenters(const std::list<kipl::base::RectROI> &ROIs)
{
    std::ostringstream msg;
    logger.message("Find centers");

    kipl::profile::Timer timer;

    float *max=std::max_element(chm.GetDataPtr(),chm.GetDataPtr()+chm.Size());
    size_t maxpos=max-chm.GetDataPtr();

    m_maxInsetCenter.y=maxpos/m_Img2D.Size(0);
    m_maxInsetCenter.x=maxpos%m_Img2D.Size(0);

    dots.clear();
    std::vector<pair<float, float> > roiDots;
    std::ofstream dotfile("dots.csv");
    timer.reset();
    timer.Tic();
    int loopidx=0;
    for (const auto &roi : ROIs)
    {
        kipl::base::TImage<float,2> chmCrop=kipl::base::TSubImage<float,2>::Get(chm, roi.box());
        size_t idx=0;

        msg.str(""); msg<<"chmcrop_"<<loopidx++<<".tif";
        if (saveIntermediateImages)
            kipl::io::WriteTIFF(chmCrop,msg.str(),kipl::base::Float32);


        float minVal=0.0f;
        float maxVal=0.0f;

        kipl::math::minmax(chmCrop.GetDataPtr(),chmCrop.Size(),&minVal,&maxVal,true);
        float threshold = (maxVal-minVal)*0.8f+minVal;
        insetpeaks.resize(chmCrop.dims());
        for (size_t i=0; i<chmCrop.Size(); ++i)
            insetpeaks[i]=threshold < chmCrop[i];

        roiDots.clear();
        for (size_t y=0; y<insetpeaks.Size(1); ++y)
        {
            for (size_t x=0; x<insetpeaks.Size(0); ++x , ++idx)
            {
                if (insetpeaks[idx]!=0.0f)
                {
                    roiDots.push_back(make_pair(float(x+roi.box()[0]),float(y+roi.box()[1])));
                    dotfile<<x+roi.box()[0]<<", "<<y+roi.box()[1]<<"\n";
                }
            }
        }
        if (roiDots.size()<400)
            dots.insert( dots.end(), roiDots.begin(), roiDots.end() );
    }
    dotfile.close();
    timer.Toc();
    msg.str(""); msg<<"hMax timing: "<<timer;

}

void ContrastSampleAnalysis::findCenters()
{
    std::ostringstream msg;
    logger.message("Find centers");


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

    if (saveIntermediateImages)
        kipl::io::WriteTIFF(peaks,"csa_hmax.tif",kipl::base::Float32);

    insetpeaks=chm-peaks;

    if (saveIntermediateImages)
        kipl::io::WriteTIFF(insetpeaks,"csa_insetpeaks.tif",kipl::base::Float32);

    for (size_t i=0; i<insetpeaks.Size(); ++i)
        insetpeaks[i]=0.5f*threshold<insetpeaks[i];

    if (saveIntermediateImages)
        kipl::io::WriteTIFF(insetpeaks,"csa_insetpeaks_bi.tif",kipl::base::Float32);
    timer.reset();
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
                dotfile<<x<<", "<<y<<"\n";
            }
        }
    }
    dotfile.close();
}

void ContrastSampleAnalysis::estimateInsetRing()
{
    std::ostringstream msg;

    msg.str("");
    msg<<"Found "<<dots.size()<<" dots";
    logger(logger.LogMessage,msg.str());
    if (dots.size()<5)
        throw kipl::base::KiplException("Too few dots to estimate the ring parameters.");
    arma::mat H(dots.size()-1UL,2);
    arma::vec a(dots.size()-1UL);

    std::pair<float,float> xyN=dots.back();

    for (size_t i=0; i<dots.size()-1; ++i)
    {
        H(i,0) = 2*(xyN.first-dots[i].first);
        H(i,1) = 2*(xyN.second-dots[i].second);
        a(i)   = xyN.first*xyN.first
                +xyN.second*xyN.second
                -dots[i].first*dots[i].first
                -dots[i].second*dots[i].second;
    }

    arma::vec parameters = arma::solve(H,a);
    m_ringCenter.x=parameters[0];
    m_ringCenter.y=parameters[1];

    kipl::profile::Timer timer;
    timer.Toc();

    msg.str(""); msg<<"Circle fitting timing: "<<timer;

    msg.str("");
    msg<<"Ring center at ["<<parameters[0]<<", "<<parameters[1]<<"]";
    logger(logger.LogMessage,msg.str());

    float phi=atan2f(m_maxInsetCenter.x-m_ringCenter.x,m_maxInsetCenter.y-m_ringCenter.y);
    msg.str("");
    msg<<"Max intensity at ["<<m_maxInsetCenter.x<<", "<<m_maxInsetCenter.y<<", phi="<<phi*180.0f/fPi;
    logger(logger.LogDebug,msg.str());

    msg.str("");
    msg<<"Dot centers at:\n";
    float r=hypotf(m_maxInsetCenter.x-m_ringCenter.x,m_maxInsetCenter.y-m_ringCenter.y);

    m_insetCenters.clear();
    for (int i=0 ; i<6 ; i++)
    {
        kipl::base::coords3Df coord;

        coord.y=r*cos(phi+i*fPi/3)+m_ringCenter.y;
        coord.x=r*sin(phi+i*fPi/3)+m_ringCenter.x;
        coord.z=0.0f;
        m_insetCenters.push_back(coord);

        msg<<"Inset "<<i<<" at "<<coord.x<<", "<<coord.y<<"\n";
    }

    logger.message(msg.str());
}

kipl::math::Statistics ContrastSampleAnalysis::computeInsetStatistics(kipl::base::coords3Df pos,float ps)
{
    kipl::math::Statistics stats;
    stats.reset();
    const float fRadius = 0.3f*metricInsetDiameter/ps; // Reduced radius to avoid edge effects
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
    kipl::base::Histogram(m_Img2D.GetDataPtr(),m_Img2D.Size(),hist_size,hist_bins,hist_axis,0.0f,0.0f,true);

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
    chm=0.0f;
    chm=cht(m_Img2D,radius,true);
    timer.Toc();
    msg.str(""); msg<<"Circular Hough timing: "<<timer;

    if (saveIntermediateImages)
        kipl::io::WriteTIFF(chm,"csa_cht.tif",kipl::base::Float32);
}

}

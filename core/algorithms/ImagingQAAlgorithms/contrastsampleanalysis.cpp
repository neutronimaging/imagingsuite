//<LICENSE>

#include <base/thistogram.h>
#include <filters/filter.h>
#include <math/circularhoughtransform.h>
#include <morphology/morphextrema.h>
#include <morphology/label.h>

#include "contrastsampleanalysis.h"
namespace ImagingQAAlgorithms {
ContrastSampleAnalysis::ContrastSampleAnalysis() :
    logger("ContrastSampleAnalysis"),
    pixelsize(0.1f),
    hist_size(1024)
{
}

ContrastSampleAnalysis::~ContrastSampleAnalysis()
{
    clearAllocation();
}

void ContrastSampleAnalysis::clearAllocation()
{

}

void ContrastSampleAnalysis::setImage(kipl::base::TImage<float,2> img)
{
   m_Img2D.Clone(img);
   makeHistogram();
}

void ContrastSampleAnalysis::setImage(kipl::base::TImage<float,3> img)
{
    m_Img3D.Clone(img);

    kipl::base::Histogram(m_Img3D.GetDataPtr(),m_Img3D.Size(),hist_bins, hist_size,0.0f,0.0f,hist_axis);

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
}

void ContrastSampleAnalysis::analyzeContrast(float ps, kipl::math::Statistics *stats, kipl::base::coords3Df *centers)
{
    findCenters(ps);
}

void ContrastSampleAnalysis::findCenters(float ps)
{
    kipl::math::CircularHoughTransform cht;

    const float radius = 2.0f/ps;

    chm=cht(m_Img2D,radius,false);

    kipl::morphology::hMax(chm,peaks,10.0f, kipl::morphology::conn4);
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
    kipl::base::Histogram(m_Img2D.GetDataPtr(),m_Img2D.Size(),hist_bins,hist_size,0.0f,0.0f,hist_axis);
}

}

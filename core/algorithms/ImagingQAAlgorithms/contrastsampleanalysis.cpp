//<LICENSE>

#include <base/thistogram.h>
#include <filters/filter.h>

#include "contrastsampleanalysis.h"
namespace ImagingQAAlgorithms {
ContrastSampleAnalysis::ContrastSampleAnalysis() :
    logger("ContrastSampleAnalysis"),
    pixelsize(0.1f),
    hist_size(1024)
{

}

void ContrastSampleAnalysis::setImage(kipl::base::TImage<float,3> img)
{
    m_Img3D.Clone(img);

    kipl::base::Histogram(m_Img3D.GetDataPtr(),m_Img3D.Size(),hist_bins, hist_size,0.0f,0.0f,hist_axis);

    m_Img2D.Resize(m_Img3D.Dims());
    for (int slice=0; i<m_Img3D.Size(2); ++i)
    {
        float *pSlice=m_Img3D.GetLinePtr(0,slice);
        for (int pix=0; pix<m_Img2D.Size(); ++pix) {
            m_Img2D[pix]+=pSlice[pix];
        }
    }
    for (int pix=0; pix<m_Img2D.Size(); ++pix) {
        m_Img2D[pix]/=m_Img3D.Size(2);
    }
}

void ContrastSampleAnalysis::analyzeContrast(float ps, kipl::math::Statistics *stats, kipl::base::coords3Df *centers)
{
    findCenters(ps);

}

void ContrastSampleAnalysis::findCenters(float ps)
{
    kipl::base::TImage<float,2> edges(m_Img2D.Dims());

    size_t filtdims[2]={3,3};
    float vkernel[9]={-3.0f, -10.0f, -3.0f, 0.0f,0.0f,0.0f,3.0f,10.0f,3.0f};
    kipl::filters::TFilter<float,2> vert(vkernel,filtdims);
    kipl::base::TImage<float,2> dy=vert(m_Img2D,kipl::filters::FilterBase::EdgeMirror);

    float hkernel[9]={-3.0f, 0.0f, 3.0f,-10.0f,0.0f,10.0f,-3.0,3.0f,0.0f,3.0f};
    kipl::filters::TFilter<float,2> hor(hkernel,filtdims);
    kipl::base::TImage<float,2> dx=hor(m_Img2D,kipl::filters::FilterBase::EdgeMirror);

    for (size_t i=0; i<edges.Size(); ++i)
    {
        edges[i]=sqrt(dx[i]*dx[i]+dy[i]*dy[i]);
    }
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

}

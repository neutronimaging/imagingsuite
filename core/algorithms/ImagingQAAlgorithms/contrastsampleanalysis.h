#ifndef CONTRASTSAMPLEANALYSIS_H
#define CONTRASTSAMPLEANALYSIS_H

#include "imagingqaalgorithms_global.h"

#include <logging/logger.h>
#include <base/timage.h>
#include <base/index2coord.h>
#include <math/statistics.h>

namespace ImagingQAAlgorithms {

class ContrastSampleAnalysis
{
    kipl::logging::Logger logger;

public:
    ContrastSampleAnalysis();
    ~ContrastSampleAnalysis();

    void setImage(kipl::base::TImage<float,2> img);
    void setImage(kipl::base::TImage<float,3> img);

    void analyzeContrast(float pixelSize, kipl::math::Statistics *stats, kipl::base::coords3Df *centers);

    int getHistogram(float *axis, size_t *bins);
    int getHistogramSize();
    bool saveIntermediateImages;
    float metricInsetDiameter;

protected:

    void clearAllocation();
    void createAllocation();
    void makeHistogram();
    void findCenters(float ps);
    kipl::math::Statistics computeInsetStatistics(kipl::base::coords3Df pos,float ps);
    void buildRingKernel(float radius);

    kipl::base::TImage<float,3> m_Img3D;
    kipl::base::TImage<float,2> m_Img2D;
    kipl::base::TImage<float,2> chm;
    kipl::base::TImage<float,2> peaks;

    float pixelsize;

    int hist_size;
    float *hist_axis;
    size_t *hist_bins;
    kipl::base::TImage<float,2> ringKernel;


    kipl::math::Statistics m_insetStats[6];
    kipl::base::coords3Df m_ringCenter;
    kipl::base::coords3Df m_insetCenters[6];

};
}

#endif // CONTRASTSAMPLEANALYSIS_H

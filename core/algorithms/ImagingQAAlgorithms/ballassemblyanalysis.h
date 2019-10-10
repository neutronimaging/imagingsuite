#ifndef BALLASSEMBLYANALYSIS_H
#define BALLASSEMBLYANALYSIS_H

#include "imagingqaalgorithms_global.h"

#include <vector>
#include <list>
#include <map>

#include <logging/logger.h>
#include <base/timage.h>
#include <base/kiplenums.h>
#include <base/roi.h>
#include <math/statistics.h>

namespace ImagingQAAlgorithms {
class IMAGINGQAALGORITHMSSHARED_EXPORT BallAssemblyAnalysis
{
    kipl::logging::Logger logger;
public:
    BallAssemblyAnalysis();
    void analyzeImage(kipl::base::TImage<float,3> &img);
    void analyzeImage(kipl::base::TImage<float,3> &img, std::list<kipl::base::RectROI> &roiList);

    std::map<float, kipl::math::Statistics> &getStatistics();


    kipl::base::TImage<float,2> & getZProjection();
    kipl::base::TImage<float,3> & getMask();
    kipl::base::TImage<int,3> &getLabels();
    kipl::base::TImage<float,3> &getDist();
    float pixelSize;
    float strelRadius;
private:
    void createLabelledMask(kipl::base::TImage<float,3> &img);

    kipl::base::TImage<float,2> projection;
    kipl::base::TImage<float,3> mask;
    kipl::base::TImage<float,3> dist;
    kipl::base::TImage<int,3> labels;
    std::map<float,kipl::math::Statistics> assemblyStats;
};
}
#endif // BALLASSEMBLYANALYSIS_H

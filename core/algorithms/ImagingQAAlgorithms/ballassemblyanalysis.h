#ifndef BALLASSEMBLYANALYSIS_H
#define BALLASSEMBLYANALYSIS_H

#include "imagingqaalgorithms_global.h"

#include <vector>

#include <logging/logger.h>
#include <base/timage.h>
#include <math/statistics.h>

namespace ImagingQAAlgorithms {
class IMAGINGQAALGORITHMSSHARED_EXPORT BallAssemblyAnalysis
{
    kipl::logging::Logger logger;
public:
    BallAssemblyAnalysis();
    void analyzeImage(kipl::base::TImage<float,3> &img);

    std::vector<kipl::math::Statistics> getStatistics();

    kipl::base::TImage<float,3> & getMask();
    kipl::base::TImage<int,3> &getLabels();
    float pixelSize;
    float strelRadius;
private:
    void createLabelledMask(kipl::base::TImage<float,3> &img);

    kipl::base::TImage<float,3> mask;
    kipl::base::TImage<int,3> labels;
    std::vector<kipl::math::Statistics> assemblyStats;
};
}
#endif // BALLASSEMBLYANALYSIS_H

#ifndef PROJECTIONSERIESCORRECTION_H
#define PROJECTIONSERIESCORRECTION_H

#include "ImagingAlgorithms_global.h"

#include <base/timage.h>
#include <logging/logger.h>

#include <vector>

namespace ImagingAlgorithms
{

class IMAGINGALGORITHMSSHARED_EXPORT ProjectionSeriesCorrection
{
private:
    kipl::logging::Logger logger;
public:
    enum eDoseOutlierDetection
    {
        FixedThreshold,
        MAD
    };

    enum eDoseOutlierReplacement
    {
        NeighborAverage,
        WeightedAverage
    };

    ProjectionSeriesCorrection();
    ProjectionSeriesCorrection(float                  th,
                               eDoseOutlierDetection   dmethod,
                               eDoseOutlierReplacement rmethod);

    void process(kipl::base::TImage<float,3> &img, vector<float> &dose, float threshold);
    vector<size_t> detectUnderExposure(std::vector<float> & doses, float threshold);

private:
    void processNeighborAverage(kipl::base::TImage<float,3> &img, vector<float> &dose, float threshold);
    void processWeightedAverage(kipl::base::TImage<float,3> &img, vector<float> &dose, float threshold);

    eDoseOutlierDetection   detectionMethod;
    eDoseOutlierReplacement replacementMethod;
};
}

void         IMAGINGALGORITHMSSHARED_EXPORT   string2enum(const std::string &str,
                                                          ImagingAlgorithms::ProjectionSeriesCorrection::eDoseOutlierDetection &dod);
std::string  IMAGINGALGORITHMSSHARED_EXPORT   enum2string(ImagingAlgorithms::ProjectionSeriesCorrection::eDoseOutlierDetection dod);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s,
                                                         ImagingAlgorithms::ProjectionSeriesCorrection::eDoseOutlierDetection dod);

void         IMAGINGALGORITHMSSHARED_EXPORT   string2enum(const std::string &str,
                                                          ImagingAlgorithms::ProjectionSeriesCorrection::eDoseOutlierReplacement &dor);
std::string  IMAGINGALGORITHMSSHARED_EXPORT   enum2string(ImagingAlgorithms::ProjectionSeriesCorrection::eDoseOutlierReplacement dor);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s,
                                                         ImagingAlgorithms::ProjectionSeriesCorrection::eDoseOutlierReplacement &dor);

#endif // PROJECTIONSERIESCORRECTION_H

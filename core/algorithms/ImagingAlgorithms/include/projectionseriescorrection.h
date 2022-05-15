#ifndef PROJECTIONSERIESCORRECTION_H
#define PROJECTIONSERIESCORRECTION_H

#include "ImagingAlgorithms_global.h"

#include <base/timage.h>

#include <vector>

namespace ImagingAlgorithms
{

class IMAGINGALGORITHMSSHARED_EXPORT ProjectionSeriesCorrection
{
public:
    enum DoseOutlierDetection
    {
        FixedThreshold,
        MAD
    };

    enum DoseOutlierReplacement
    {
        NeighborAverage,
        WeightedAverage
    };

    ProjectionSeriesCorrection(float                  th      = 1.0f,
                               DoseOutlierDetection   dmethod = FixedThreshold,
                               DoseOutlierReplacement rmethod = NeighborAverage);

    void process(kipl::base::TImage<float,3> &img, vector<float> &dose);

private:
    void processNeighborAverage(kipl::base::TImage<float,3> &img, vector<float> &dose);
    void processWeightedAverage(kipl::base::TImage<float,3> &img, vector<float> &dose);

    DoseOutlierDetection   detectionMethod;
    DoseOutlierReplacement replacementMethod;
};

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(const std::string &str, ImagingAlgorithms::ProjectionSeriesCorrection::DoseOutlierDetection &dod);
std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(const ImagingAlgorithms::ProjectionSeriesCorrection::DoseOutlierDetection &dod);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ProjectionSeriesCorrection::DoseOutlierDetection dod);

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(const std::string &str, ImagingAlgorithms::ProjectionSeriesCorrection::DoseOutlierReplacement &dor);
std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(const ImagingAlgorithms::ProjectionSeriesCorrection::DoseOutlierReplacement &dor);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ProjectionSeriesCorrection::DoseOutlierReplacement dor);

}
#endif // PROJECTIONSERIESCORRECTION_H

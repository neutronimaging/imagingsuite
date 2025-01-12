#ifndef ReplaceUnderexposed_H
#define ReplaceUnderexposed_H

#include "ImagingAlgorithms_global.h"

#include <base/timage.h>
#include <logging/logger.h>

#include <vector>

namespace ImagingAlgorithms
{

class IMAGINGALGORITHMSSHARED_EXPORT ReplaceUnderexposed
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

    ReplaceUnderexposed();
    ReplaceUnderexposed(float                   th,
                        eDoseOutlierDetection   dmethod,
                        eDoseOutlierReplacement rmethod);

    void process(kipl::base::TImage<float,3> &img, vector<float> &dose, float threshold);
    vector<size_t> detectUnderExposure(std::vector<float> & doses, float threshold);

private:
    void processNeighborAverage(kipl::base::TImage<float,3> &img, vector<float> &dose, float threshold);
    void processWeightedAverage(kipl::base::TImage<float,3> &img, vector<float> &dose, float threshold);

    // eDoseOutlierDetection   detectionMethod;
    eDoseOutlierReplacement replacementMethod;
};
}

void         IMAGINGALGORITHMSSHARED_EXPORT   string2enum(const std::string &str,
                                                          ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection &dod);
std::string  IMAGINGALGORITHMSSHARED_EXPORT   enum2string(ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection dod);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s,
                                                         ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection dod);

void         IMAGINGALGORITHMSSHARED_EXPORT   string2enum(const std::string &str,
                                                          ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement &dor);
std::string  IMAGINGALGORITHMSSHARED_EXPORT   enum2string(ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement dor);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s,
                                                         ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement &dor);

#endif // ReplaceUnderexposed_H

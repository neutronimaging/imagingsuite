//<LICENSE>

#ifndef NORMALIZEIMAGE_H
#define NORMALIZEIMAGE_H

#include "ImagingAlgorithms_global.h"

#include <vector>
#include <logging/logger.h>
#include <base/timage.h>

namespace ImagingAlgorithms
{

class IMAGINGALGORITHMSSHARED_EXPORT NormalizeImage
{
    kipl::logging::Logger logger;

public:
    NormalizeImage(bool useLog=true);

    void setReferences( kipl::base::TImage<float,2> &ff,
                        kipl::base::TImage<float,2> &dc);
    void setDoseROI( std::vector<size_t> &roi);

    void process(kipl::base::TImage<float,2> &img);
    void process(kipl::base::TImage<float,3> &img);

    void useThreading();
    bool usingLog();
protected:
    void normalizeSlices(kipl::base::TImage<float,3> &img, size_t begin, size_t end);
    float computeDose(kipl::base::TImage<float,2> &img);
    std::vector<size_t> mROI;
    bool bUseLog;

    kipl::base::TImage<float,2> mFlatField;
    kipl::base::TImage<float,2> mDark;
    bool bUseDose;
    float referenceDose;
};

}
#endif // NORMALIZEIMAGE_H

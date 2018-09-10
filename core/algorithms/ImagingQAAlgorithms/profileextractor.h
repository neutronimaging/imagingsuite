#ifndef PROFILEEXTRACTOR_H
#define PROFILEEXTRACTOR_H
#include "imagingqaalgorithms_global.h"
#include <vector>

#include <base/timage.h>

namespace ImagingQAAlgorithms {

class IMAGINGQAALGORITHMSSHARED_EXPORT ProfileExtractor
{
public:
    ProfileExtractor();

    std::vector<float> getProfile(kipl::base::TImage<float,2> &img);

private:
    void makeRawProfiles(kipl::base::TImage<float,2> &img);


};

}
#endif // PROFILEEXTRACTOR_H

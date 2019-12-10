//<LICENSE>

#ifndef PROFILEEXTRACTOR_H
#define PROFILEEXTRACTOR_H
#include "imagingqaalgorithms_global.h"
#include <vector>
#include <map>

#include <base/timage.h>

namespace ImagingQAAlgorithms {

class IMAGINGQAALGORITHMSSHARED_EXPORT ProfileExtractor
{
public:
    ProfileExtractor();

    void setPrecision(float p);
    float precision();
    std::map<float,float> getProfile(kipl::base::TImage<float,2> &img, size_t *roi=nullptr);

private:
    kipl::base::TImage<float,2> diffEdge(kipl::base::TImage<float,2> &img);
    void computeEdgeEquation(kipl::base::TImage<float,2> &img);

    float distanceToLine(int x,int y);

    float lineCoeffs[2];
    float mPrecision;
};

}
#endif // PROFILEEXTRACTOR_H

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

    void setPrecision(float prec);
    float precision();
    std::map<float,float> getProfile(kipl::base::TImage<float,2> &img, const std::vector<size_t> &roi = {});
    std::map<float,float> fittedLine(const std::vector<float> & x = {});
    const vector<float> & coefficients();

    kipl::base::TImage<float,2> distanceMap(const std::vector<size_t> &dims);
    bool saveImages;

private:
    kipl::base::TImage<float,2> diffEdge(kipl::base::TImage<float,2> &img);
    void computeEdgeEquation(kipl::base::TImage<float,2> &img);

    float distanceToLine(int x,int y);

    std::vector<float> lineCoeffs;
    float mPrecision;
};

}
#endif // PROFILEEXTRACTOR_H

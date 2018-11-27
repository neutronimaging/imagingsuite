#include "profileextractor.h"

namespace ImagingQAAlgorithms {
ProfileExtractor::ProfileExtractor()
{
    std::fill_n(lineCoeffs,2,0.0f);
}

void ProfileExtractor::setPrecision(float p)
{
    mPrecision=fabs(p);
}

float ProfileExtractor::precision()
{
    return mPrecision;
}

std::map<float, float> ProfileExtractor::getProfile(kipl::base::TImage<float, 2> &img,size_t *roi)
{
    (void)roi;
    std::map<float, float> profile;


    return profile;
}

void ProfileExtractor::makeRawProfiles(kipl::base::TImage<float,2> &img)
{

}

void ProfileExtractor::computeEdgeEquation(kipl::base::TImage<float, 2> &img)
{

}

kipl::base::TImage<float, 2> ProfileExtractor::diffEdge(kipl::base::TImage<float, 2> &img)
{

}

float ProfileExtractor::distanceToLine(int x, int y)
{
  float d=(x*lineCoeffs[0]-y+lineCoeffs[1])/sqrt(lineCoeffs[0]*lineCoeffs[0]+1.0f);

  d = floor(d/mPrecision)*mPrecision;

  return d;
}
}

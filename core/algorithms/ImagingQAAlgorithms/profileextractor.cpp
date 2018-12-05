//<LICENSE>

#include "profileextractor.h"
#include <filters/filter.h>

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
    computeEdgeEquation(img);




    return profile;
}

void ProfileExtractor::makeRawProfiles(kipl::base::TImage<float,2> &img)
{

}

void ProfileExtractor::computeEdgeEquation(kipl::base::TImage<float, 2> &img)
{
    auto dimg=diffEdge(img);
}

kipl::base::TImage<float, 2> ProfileExtractor::diffEdge(kipl::base::TImage<float, 2> &img)
{
    float dx[9]={ -3.0f, 0.0f, 3.0f,
                 -10.0f, 0.0f, 10.0f,
                  -3.0f, 0.0f, 3.0f};

    float dy[9]={-3.0f, -10.0f, -3.0f,
                  0.0f,  0.0f,  0.0f,
                  3.0f,  10.0f, 3.0f};

    size_t ddims[2]={3,3};

    kipl::filters::TFilter<float,2> diff_x(dx,ddims);
    kipl::filters::TFilter<float,2> diff_y(dy,ddims);


    auto dximg=diff_x(img,kipl::filters::FilterBase::EdgeMirror);
    auto dyimg=diff_y(img,kipl::filters::FilterBase::EdgeMirror);
    kipl::base::TImage<float,2> dimg(img.Dims());
    for (size_t i=0; i<img.Size(); ++i)
    {
        dimg[i]=sqrt(dximg[i]*dximg[i]+dyimg[i]*dyimg[i]);
    }
}

float ProfileExtractor::distanceToLine(int x, int y)
{
  float d=(x*lineCoeffs[0]-y+lineCoeffs[1])/sqrt(lineCoeffs[0]*lineCoeffs[0]+1.0f);

  d = floor(d/mPrecision)*mPrecision;

  return d;
}

}

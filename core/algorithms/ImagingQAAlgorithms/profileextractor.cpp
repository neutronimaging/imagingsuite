//<LICENSE>

#include "profileextractor.h"
#include <filters/filter.h>
#include <math/statistics.h>

#include <filters/filter.h>
#include <io/io_tiff.h>
#include <base/tprofile.h>
#include <math/mathconstants.h>
#include <math/findpeaks.h>

namespace ImagingQAAlgorithms {
ProfileExtractor::ProfileExtractor() :
    saveImages(false),
    lineCoeffs{0.0f,1.0f},
    mPrecision(0.1f)
{
}

void ProfileExtractor::setPrecision(float p)
{
    mPrecision=fabs(p);
}

float ProfileExtractor::precision()
{
    return mPrecision;
}

std::map<float, float> ProfileExtractor::fittedLine(const std::vector<float> &x)
{
    std::map<float, float> line;

    for (auto const &xx : x)
        line.insert(make_pair(xx,lineCoeffs[0]+xx*lineCoeffs[1]));

    return line;
}

const vector<float> &ProfileExtractor::coefficients()
{
    return lineCoeffs;
}

kipl::base::TImage<float, 2> ProfileExtractor::distanceMap(const std::vector<size_t> &dims)
{
    kipl::base::TImage<float, 2> img(dims);

    for (size_t y=0 ; y<dims[1]; ++y)
        for (size_t x=0 ; x<dims[0]; ++x)
            img(x,y)=distanceToLine(x,y);

    return img;
}

std::map<float, float> ProfileExtractor::getProfile(kipl::base::TImage<float, 2> &img,const std::vector<size_t> &roi)
{
    (void)roi;
    std::map<float, kipl::math::Statistics> profile_stats;
    profile_stats.clear();

    auto de=diffEdge(img);

    computeEdgeEquation(de);

    if (saveImages)
       kipl::io::WriteTIFF(de,"PE_diffEdge.tif",kipl::base::Float32);

    int nx=static_cast<int>(de.Size(0));
    int ny=static_cast<int>(de.Size(1));

    for (int y=0; y<ny; ++y)
    {
        for (int x=0; x<nx; ++x)
        {
            float dist=distanceToLine(x,y);
            if (profile_stats.find(dist)!=profile_stats.end())
                profile_stats[dist].put(img(x,y));
            else {
                kipl::math::Statistics s;
                s.put(img(x,y));
                profile_stats.insert(std::make_pair(dist,s));
            }
        }
    }

    std::map<float, float> profile;
    for (const auto & item : profile_stats )
        profile.insert(std::make_pair(item.first,static_cast<float>(item.second.E())));

    return profile;
}

void ProfileExtractor::computeEdgeEquation(kipl::base::TImage<float, 2> &img)
{
    float x=0.0f;
    // float x2=0.0f;
    float y=0.0f;
    float y2=0.0f;
    float xy=0.0f;

    size_t Nx=img.Size(0);
    size_t Ny=img.Size(1);
    float cnt=0.0f;
    for (size_t yy=1; yy<Ny; ++yy)
    {
        float *pLine=img.GetLinePtr(yy);

        float maxpos=kipl::math::findPeakCOG(pLine,Nx,true,false);
        x  += maxpos;
        // x2 += maxpos*maxpos;
        y  += yy;
        y2 += yy*yy;
        xy += maxpos*yy;
        cnt++;
    }

    lineCoeffs[0]=(x*y2-y*xy)/(cnt*y2-y*y);
    lineCoeffs[1]=-(cnt*xy-x*y)/(cnt*y2-y*y);

}

kipl::base::TImage<float, 2> ProfileExtractor::diffEdge(kipl::base::TImage<float, 2> &img)
{

    std::vector<float> kx={-3.0f,0.0f,3.0f,
                -10.0f,0.0f,10.0f,
                -3.0f,0.0f,3.0f};
    std::vector<size_t> dims={3,3};
    kipl::filters::TFilter<float,2> dx(kx,dims);

    std::vector<float> ky={-3.0f,-10.0f,-3.0f,
                            0.0f,  0.0f, 0.0f,
                            3.0f, 10.0f, 3.0f};

    kipl::filters::TFilter<float,2> dy(ky,dims);

    auto dximg=dx(img,kipl::filters::FilterBase::EdgeMirror);
    auto dyimg=dy(img,kipl::filters::FilterBase::EdgeMirror);

    kipl::base::TImage<float,2> absgrad(img.dims());

    float *pA=absgrad.GetDataPtr();
    float *pX=dximg.GetDataPtr();
    float *pY=dyimg.GetDataPtr();

    for (size_t i=0; i<img.Size(); ++i,++pA,++pX,++pY)
    {
        *pA=sqrt((*pX)*(*pX)+(*pY)*(*pY));
    }

    return absgrad;

}

float ProfileExtractor::distanceToLine(int x, int y)
{
    const float &m =  lineCoeffs[0];
    const float &k = -lineCoeffs[1];
//  float d=(y*lineCoeffs[1]-x+lineCoeffs[0])/sqrt(lineCoeffs[1]*lineCoeffs[1]+1.0f);
    float d=(k*y-x+m)/sqrt(k*k+1.0f);

  d = floor(d/mPrecision)*mPrecision;

  return d;
}

}

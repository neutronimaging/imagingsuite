//<LICENSE>

#ifndef GAMMACLEAN_H
#define GAMMACLEAN_H

#include <base/timage.h>
#include <filters/medianfilter.h>

namespace ImagingAlgorithms {

class GammaClean
{
public:
    GammaClean();
    kipl::base::TImage<float,2> Process(kipl::base::TImage<float,2> & img);
    void Configure(float sigma, float th3, float th5, float th7, size_t medsize);

    kipl::base::TImage<float,2> DetectionImage(kipl::base::TImage<float,2> &img);

private:
    void PrepareNeighborhoods(size_t *dims);
    void MedianNeighborhood(float *pImg, float *pRes, ptrdiff_t pos, ptrdiff_t *ng, size_t N);
    float m_fSigma;
    float m_fThreshold3;
    float m_fThreshold5;
    float m_fThreshold7;
    size_t m_nMedianSize;


    ptrdiff_t m_nNG3[9];
    ptrdiff_t m_nNG5[25];
    ptrdiff_t m_nNG7[49];
    ptrdiff_t m_nData;

    float medvec[49];
};

}
#endif // GAMMACLEAN_H

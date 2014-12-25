#ifndef GAMMACLEAN_H
#define GAMMACLEAN_H

#include <base/timage.h>
#include <filters/medianfilter.h>

namespace ImagingAlgorithms {

class GammaClean
{
public:
    GammaClean();
    kipl::base::TImage<float,2> process(kipl::base::TImage<float,2> & img);

private:
    float m_fSigma;
    float m_fThreshold3;
    float m_fThreshold5;
    float m_fThreshold7;
    size_t m_nMedianSize;
};

}
#endif // GAMMACLEAN_H

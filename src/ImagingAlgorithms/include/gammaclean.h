#ifndef GAMMACLEAN_H
#define GAMMACLEAN_H

#include <base/timage.h>
#include <filters/medianfilter.h>

namespace ImagingAlgorithms {

}
class GammaClean
{
public:
    GammaClean();
    kipl::base::TImage<float,2> process(kipl::base::TImage<float,2> & img);

private:
    void BuildLOGkernel(float alpha);

    float m_fLOGkernel[9];
};

}
#endif // GAMMACLEAN_H

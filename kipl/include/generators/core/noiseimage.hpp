#ifndef NOISEIMAGE_HPP
#define NOISEIMAGE_HPP

#include "../../base/timage.h"
#include "../../math/GaussianNoise.h"

namespace kipl { namespace generators {
template <typename T, size_t N>
void NoiseGenerator<T,N>::Gauss(kipl::base::TImage<T,N> &img, float m, float s)
{
#if __cplusplus > 199711L
    std::normal_distribution<T> distr(m,s);
    T * pImg=img.GetDataPtr();
    for (size_t i=0; i<img.Size(); i++) {
        pImg[i]+=distr(m_Generator);
    }
#endif
}

template <typename T, size_t N>
void NoiseGenerator<T,N>::Poisson(kipl::base::TImage<T,N> &img, float lambda)
{
#if __cplusplus > 199711L
    std::poisson_distribution<T> distr(lambda);
    T * pImg=img.GetDataPtr();
    for (size_t i=0; i<img.Size(); i++) {
        pImg[i]+=distr(m_Generator);
    }
#endif
}


}}

#endif // NOISEIMAGE_HPP


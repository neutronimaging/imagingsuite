#ifndef __NOISEIMAGE_H
#define __NOISEIMAGE_H

#if __cplusplus > 199711L
#include <random>
#endif
#include "../base/timage.h"
#include "../math/GaussianNoise.h"

namespace kipl { namespace generators {

void AddTexturedNoise(kipl::base::TImage<float,2> &img, float variance, size_t width);
void AddTexturedNoise(kipl::base::TImage<float,3> *img, float variance, size_t width);

template <typename T, size_t N>
class NoiseGenerator {
public:
    void Gauss(kipl::base::TImage<T,N> &img, float m, float s);
    void Poisson(kipl::base::TImage<T,N> &img, float lambda);

private:
    #if __cplusplus > 199711L
    std::default_random_engine m_Generator;
    #endif

};

}}

#include "core/noiseimage.hpp"

#endif

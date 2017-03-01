//<LICENCE>

#ifndef NOISEIMAGE_HPP
#define NOISEIMAGE_HPP

#include "../../base/timage.h"
#include "../../math/GaussianNoise.h"
#include "../../math/PoissonNoise.h"
#include "../NoiseImage.h"

namespace kipl { namespace generators {

template <typename T, size_t N>
NoiseGenerator<T,N>::NoiseGenerator() {}

template <typename T, size_t N>
NoiseGenerator<T,N>::~NoiseGenerator() {}

template <typename T, size_t N>
kipl::base::TImage<T,N> NoiseGenerator<T,N>::MakeNoise() {
    kipl::base::TImage<T,N> img;
    return img;
}

template <typename T, size_t N>
void NoiseGenerator<T,N>::AddNoise(kipl::base::TImage<T,N> &img)
{

}

template <typename T, size_t N>
void NoiseGenerator<T,N>::Gauss(kipl::base::TImage<T,N> &img, float m, float s)
{
}

template <typename T, size_t N>
void NoiseGenerator<T,N>::Poisson(kipl::base::TImage<T,N> &img, float lambda)
{}

template <typename T, size_t N>
void NoiseGenerator<T,N>::AddTexturedNoise(kipl::base::TImage<float,2> &img, float variance, size_t width)
{}

template <typename T, size_t N>
void NoiseGenerator<T,N>::AddTexturedNoise(kipl::base::TImage<float,3> &img, float variance, size_t width)
{}

}}

#endif // NOISEIMAGE_HPP


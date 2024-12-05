

#ifndef NOISEIMAGE_HPP
#define NOISEIMAGE_HPP

#include "../../base/timage.h"
#include "../../math/GaussianNoise.h"
#include "../../math/PoissonNoise.h"
#include "../NoiseImage.h"
#include <random>

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
    // A function to add Gaussian noise to an image using the STL random number generator
    // The function takes an image and the mean and standard deviation of the noise to be added
    // The function returns the image with the added noise

    // Create a random number generator
    std::default_random_engine generator;
    std::normal_distribution<float> distribution(m,s);

    // Loop through the image and add noise to each pixel
    for (size_t i = 0; i < img.Size(); i++)
    {
        img[i] += distribution(generator);
    }   

    return;
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


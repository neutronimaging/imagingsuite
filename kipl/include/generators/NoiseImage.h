#ifndef __NOISEIMAGE_H
#define __NOISEIMAGE_H

#if __cplusplus > 199711L
#include <random>
#endif
#include "../base/timage.h"
#include "../math/GaussianNoise.h"

namespace kipl {
/// \brief The generator name space contains classes and functions to generate test images for algorithm performance and function testing
namespace generators {
/// \brief Enum to select noise type
enum eNoiseTypes {
    noiseFree,      ///< No noise is generated
    noiseUniform,   ///< Add uniform noise
    noiseGaussian,  ///< Add Gaussian noise
    noisePoisson,   ///< Generate Poisson noise
    noiseGamma      ///< Generate gamma noise
};

/// \brief The class produce noise images using different noise distributions.
/// \note The class requires compilation using C++11.
/// \todo The NoiseGenerator is currently in an incomplete condition and is not yet recommended to use.
template <typename T, size_t N>
class NoiseGenerator {
public:
    NoiseGenerator();
    ~NoiseGenerator();

    kipl::base::TImage<T,N> MakeNoise();
    void AddNoise(kipl::base::TImage<T,N> &img);

private: 
    void Gauss(kipl::base::TImage<T,N> &img, float m, float s);
    void Poisson(kipl::base::TImage<T,N> &img, float lambda);
    /// \brief Adds textured noise to a 2D image
    /// \param img the image to corrupt by noise
    /// \param variance noise variance of the noise source. The noise has a Gaussian distribution
    /// \param width the width of the smoothing kernel to produce a spatially coherent noise. The smoothing is realized using a box filter.
    void AddTexturedNoise(kipl::base::TImage<float,2> &img, float variance, size_t width);

    /// \brief Adds textured noise to a 3D image. The function is not yet fully implemented
    /// \param img the image to corrupt by noise
    /// \param variance noise variance of the noise source. The noise has a Gaussian distribution
    /// \param width the width of the smoothing kernel to produce a spatially coherent noise. The smoothing is realized using a box filter.
    void AddTexturedNoise(kipl::base::TImage<float,3> &img, float variance, size_t width);

    eNoiseTypes m_NoiseType;
    std::default_random_engine m_Generator;
    float m_fMean;
    float m_fVariance;

    bool m_bTexture;
    float m_fTextureWidth;

};

}}

#include "core/noiseimage.hpp"

#endif

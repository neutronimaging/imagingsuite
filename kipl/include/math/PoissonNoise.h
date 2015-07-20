#ifndef POISSONNOISE_H
#define POISSONNOISE_H

#include <cstdlib>

namespace kipl { namespace math {

size_t Poisson(float m, float N);

void PoissonNoise(float *pData, size_t N, float lambda=0.0f, float k=1.0f);

void AddPoissonNoise(float *pData, size_t N, float lambda=0.0f, float k=1.0f);
}}
#endif // POISSONNOISE_H

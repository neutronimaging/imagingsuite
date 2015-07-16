#ifndef POISSONNOISE_H
#define POISSONNOISE_H

namespace kipl { namespace math {

float Poisson(float m, float s);

void PoissonNoise(float *pData, size_t N, float lambda=0.0f, float k=1.0f);

void AddPoissonNoise(float *pData, size_t N, float lambda=0.0f, float k=1.0f);
}}
#endif // POISSONNOISE_H

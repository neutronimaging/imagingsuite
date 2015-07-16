#include "../../include/math/PoissonNoise.h"
#include <cstdlib>

namespace kipl { namespace math {

size_t Poisson(float lambda, float k)
{
    const double scale=1.0/static_cast<double>(RAND_MAX);
    const double L=exp(-lambda);
    size_t k=0;

    double p=1;
    do {
        ++k;
        p *= rand()*scale;
      } while (p > L);
    return --k;
}

void PoissonNoise(float *pData, size_t N, float lambda, float k)
{


}

void AddPoissonNoise(float *pData, size_t N, float lambda, float k)
{

}

}}


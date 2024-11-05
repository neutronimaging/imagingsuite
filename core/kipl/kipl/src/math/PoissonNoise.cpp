

#include <cstdlib>
#include <cmath>

#include "../../include/math/PoissonNoise.h"
#include "../../include/math/mathconstants.h"

namespace kipl { namespace math {

size_t Poisson(float lambda, float N)
{
    const long double scale=1.0L/static_cast<long double>(RAND_MAX);
    const long double L=exp(-lambda);


    long double ll=static_cast<long double>(lambda);
    long double lN=static_cast<long double>(lambda);
    long double lC=0.5*log(2.0L*lPi*lN);

    const long double lL=exp(log(ll)*lN-lN*log(lN)+lN-lC)*exp(-ll);

    size_t k=0;

    long double p=1;
    do {
        ++k;
        p *= rand()*scale;
    } while (p > lL);
    return --k;
}

void PoissonNoise(float *pData, size_t N, float lambda, float k)
{


}

void AddPoissonNoise(float *pData, size_t N, float lambda, float k)
{

}

}}




#include <cmath>
#include <random>
#include <cstdlib>

namespace kipl { namespace math {


float Gaussian(float m, float s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
    std::random_device rd{};
    std::mt19937 gen{rd()};

    // values near the mean are the most likely
    // standard deviation affects the dispersion of generated values from the mean
    std::normal_distribution<> d{m,s};

    return d(gen);
}


void GaussianNoise(float *pData, size_t N, float m, float s)
{
    std::random_device rd{};
    std::mt19937 gen{rd()};

    // values near the mean are the most likely
    // standard deviation affects the dispersion of generated values from the mean
    std::normal_distribution<> d{m,s};
    for (size_t i=0; i<N; i++)
    {
        pData[i] = d(gen);
	}
}

void AddGaussianNoise(float *pData, size_t N, float m, float s)
{
    std::random_device rd{};
    std::mt19937 gen{rd()};

    // values near the mean are the most likely
    // standard deviation affects the dispersion of generated values from the mean
    std::normal_distribution<> d{m,s};

    for (size_t i=0; i<N; i++)
    {
        pData[i] += d(gen);
	}
}

}}



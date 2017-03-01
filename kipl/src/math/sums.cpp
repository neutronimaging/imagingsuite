//<LICENCE>

#include <cstddef>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "../include/math/sums.h"

namespace kipl { namespace math {

double KIPLSHARED_EXPORT sum(float const * const data, const size_t N)
{
	double s=0.0;
	const ptrdiff_t sN=static_cast<ptrdiff_t>(N);
	#pragma omp parallel for reduction(+:s)
	for (long long int i=0; i<sN; i++)
		s+=data[i];
	
	return s;
}

size_t KIPLSHARED_EXPORT sum(size_t const * const data, const size_t N)
{
	size_t s=0;
	const ptrdiff_t sN=static_cast<ptrdiff_t>(N);
	#pragma omp parallel for reduction(+:s)
	for (ptrdiff_t i=0; i<sN; i++)
		s+=data[i];
		
		
	return s;
}


int KIPLSHARED_EXPORT sum2(float const * const data, double *s, double *s2, const size_t N)
{
	double local_s=0.0;
	double local_s2=0.0;
	float x;
	const ptrdiff_t sN=static_cast<ptrdiff_t>(N);
		
	#pragma omp parallel for reduction(+:local_s,local_s2)
	for (ptrdiff_t i=0; i<sN; i++) {
		x=data[i];
	
		local_s+=x;
		local_s2+=x*x;
	}

	*s=local_s;
	*s2=local_s2;
	
	return 0;
}

int KIPLSHARED_EXPORT cumsum(float const * const data, double * cumdata, size_t N, bool norm)
{
	cumdata[0]=data[0];
	for (size_t i=1; i<N; i++)
		cumdata[i]=cumdata[i-1]+data[i];

    if (norm) {
        for (size_t i=1; i<N; i++)
            cumdata[i]=cumdata[i]/cumdata[N-1];
    }

	return 0;
}

int KIPLSHARED_EXPORT cumsum(size_t const * const data, size_t * cumdata, size_t N)
{
	cumdata[0]=data[0];
	for (size_t i=1; i<N; i++)
		cumdata[i]=cumdata[i-1]+data[i];

	return 0;
}



}}


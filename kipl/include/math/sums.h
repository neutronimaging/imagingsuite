#ifndef __SUMS_H
#define __SUMS_H

#ifdef _OPENMP
#include <omp.h>
#endif

namespace kipl { namespace math {
double sum(float const * const data, const size_t N);
size_t sum(size_t const * const data, const size_t N);

double sum2(float const * const data, const size_t N);

int cumsum(float const * const data, double * cumdata, size_t N, bool norm=false);

int cumsum(size_t const * const data, size_t * cumdata, size_t N);

template <typename T>
double meansquareddiff(T const * const dataA, T const * const dataB, const size_t N)
{
	ptrdiff_t i=0;
	ptrdiff_t n=static_cast<ptrdiff_t>(N);
	double sum=0.0;
	double diff;
#pragma omp parallel for reduction(+:sum)
	for (i=0; i<n ; i++) {
		diff=static_cast<double>(dataA[i])-static_cast<double>(dataB[i]);
		sum+=static_cast<double>(diff*diff);
	}

	return sum/n;
}


}}
#endif



#ifndef IMAGE_STATISTICS_H_
#define IMAGE_STATISTICS_H_

#include "../base/timage.h"

namespace kipl { namespace math {

template <typename T, size_t N>
double RegionMean(kipl::base::TImage<T,N> img, const std::vector<size_t> & region);

template <typename T>
double RegionMean(T const * const data, size_t width, size_t height, size_t stride);

template <typename T>
void minmax(T const * const data, const size_t N, T *minval, T *maxval, bool finiteSafe=true);

template <typename T>
T sum(T * const begin, T const * const end);

template <typename T>
double sum2(T * const begin, T const * const end);

template <typename T>
double diffsum(T * const beginA, T const * const endA, T * const beginB);

template <typename T>
double diffsum2(T * const beginA, T const * const endA, T * const beginB);

template <typename T>
double diffsum(T * const beginA, T const * const endA, const T val);

template <typename T>
double diffsum2(T * const beginA, T const * const endA, const T val);

template <typename T>
std::pair<double,double> statistics(T const * const x, const size_t N);

template <typename T>
void statistics(T const * const x,double *m, double *s, const size_t *dims, size_t nDims,bool bAllocate=false);
}}


#include "core/image_statistics.hpp"
#endif /*STATISTICS_H_*/

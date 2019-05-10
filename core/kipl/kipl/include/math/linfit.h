//<LICENSE>

#ifndef LINFIT_H
#define LINFIT_H

#include <tnt.h>

namespace kipl { namespace math {

template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *q);
template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *k, double *m, double *R2);
template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *k, double *m, double *R2, double fraction);
template <typename T, typename S> void LinearLSFit(std::vector<std::pair<T,S>> &data, double &m, double &k, double *R2);


void weightedLSFit(TNT::Array2D<double> &H, TNT::Array2D<double> &C, TNT::Array2D<double> &y, TNT::Array2D<double> &param);
}}

#include "core/linfit.hpp"
#endif // LINFIT_H

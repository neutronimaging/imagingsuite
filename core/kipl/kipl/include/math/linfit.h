//<LICENSE>

#ifndef LINFIT_H
#define LINFIT_H

#include <tnt.h>
#include <vector>

namespace kipl { namespace math {

template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *q);
template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *k, double *m, double *R2);
template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *k, double *m, double *R2, double fraction);
template <typename T, typename S> void LinearLSFit(std::vector<std::pair<T,S>> &data, double &m, double &k, double *R2);


void weightedLSFit(TNT::Array2D<double> &H, TNT::Array2D<double> &C, TNT::Array2D<double> &y, TNT::Array2D<double> &param);

template<typename T, typename S>
std::vector<double> polyFit(const std::vector<T> &x,const std::vector<S> &y, int polyOrder);

template<typename T, typename S>
std::vector<double> polyVal(const std::vector<T> &x,const std::vector<S> &c);

template<typename T>
std::vector<T> polyDeriv(const std::vector<T> &c,int deriv);

}}

#include "core/linfit.hpp"
#endif // LINFIT_H

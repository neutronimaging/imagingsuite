//<LICENSE>

#ifndef LINFIT_H
#define LINFIT_H
#include "../kipl_global.h"

#include <vector>
#include <armadillo>

namespace kipl { namespace math {

template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *q);
template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *k, double *m, double *R2);
template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *k, double *m, double *R2, double fraction);
template <typename T, typename S> void LinearLSFit(std::vector<std::pair<T,S>> &data, double &m, double &k, double *R2);

/// \brief Makes a polynomial fit to the provided data
/// \param x,y data pair vectors
/// \param polyOrder the order of the polynomial to be fitted
/// \returns a vector with the polynomial coefficients.
/// \test A unit test is implemented in tKiplMath
template<typename T, typename S>
std::vector<double> polyFit(const std::vector<T> &x,const std::vector<S> &y, int polyOrder);

/// \brief Generates a value vector corresponding to the provided coefficients
/// \param x data vectors
/// \param c coefficient vector
/// \returns a vector with the polynomial values corresponding to x and c.
/// \test A unit test is implemented in tKiplMath
template<typename T, typename S>
std::vector<double> polyVal(const std::vector<T> &x,const std::vector<S> &c);

/// \brief Modifies polynomial coefficients to provide the derivative of the polynomial
/// \param c coefficient vector
/// \param deriv the order of the derivative
/// \returns a vector with the derivative coefficients.
/// \test A unit test is implemented in tKiplMath
template<typename T>
std::vector<T> polyDeriv(const std::vector<T> &c,int deriv);

void KIPLSHARED_EXPORT weightedLSFit(arma::mat &H, arma::mat &C, arma::vec &y, arma::vec &param);
}}

#include "core/linfit.hpp"
#endif // LINFIT_H

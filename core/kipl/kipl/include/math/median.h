
#include <vector>

#ifndef MEDIAN_H_
#define MEDIAN_H_

#include <iostream>
namespace kipl { namespace math {

/// \brief Median computed using heap sort
///
/// \param v Array containing data
/// \param n number of elements in the array
/// \param med The median value of the array
template<class T, class S>
void median(T *v, size_t n, S * med);

template<class T, class S>
void median(const std::vector<T> &v, S * med);

/// \brief Median computed using the quickselect algorithm
///
/// A median algorithm base on based on the quickselect algorithm described in
/// "Numerical recipes in C", Second Edition,  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
/// This code by Nicolas Devillard - 1998. Public domain.
/// Please note that the orignal data order is destroyed.
/// 
/// \param arr Array containing data
/// \param n number of elements in the array
/// \param med The median value of the array
template<class T, class S>
void median_quick_select(T *arr, size_t n, S *med);

/// \brief Median computed using the quickselect algorithm
///
/// A median algorithm base on based on the quickselect algorithm described in
/// "Numerical recipes in C", Second Edition,  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
/// This code by Nicolas Devillard - 1998. Public domain.
/// Please note that the orignal data order is destroyed.
/// 
/// \param arr Array containing data
/// \param n number of elements in the array
/// \param med The median value of the array
void median_quick_select_sse(float *arr, size_t n, float *med);
void median_quick_select_sse2(float *arr, const size_t n, float *med);
/// \brief Median computed using std::sort
///
/// A median algorithm base on based on the quickselect algorithm described in
/// "Numerical recipes in C", Second Edition,  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
/// This code by Nicolas Devillard - 1998. Public domain.
/// Please note that the orignal data order is destroyed.
/// 
/// \param arr Array containing data
/// \param n number of elements in the array
/// \param med The median value of the array
/// \note The method destroys the order of the data. This approach saves time.
template<class T, class S>
void median_STL(T *arr, size_t n, S *med);


}}

#include "core/median.hpp"
#endif /*MEDIAN_H_*/

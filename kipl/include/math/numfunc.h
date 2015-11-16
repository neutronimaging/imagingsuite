#ifdef _MSC_VER
#pragma once
#endif

#ifndef __NUMFUNC_H
#define __NUMFUNC_H
namespace NRC {
double gammp(double a, double x);

double gammq(double a, double x);

void gcf(double *gammcf, double a, double x, double *gln);

void gser(double *gamser, double a, double x, double *gln);

double gammln(double xx);

double bessj1(double x);

double bessj0(double x);

double bessj(int n, double x);

/// The cumulative normal distribution function
double Q(double x);

/// The inverse of the cumulative normal distribution function
double Qinv(double x);

/// The inverse error function
double erffinv(double p);

/// The error function
double erff(double x);

/// The complementary error function
double erffc(double x);

}

namespace kipl { namespace math {
template <typename T> T sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
}

}
#endif



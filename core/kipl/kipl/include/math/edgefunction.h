//<LICENCE>

#ifndef EDGEFUNCTION_H
#define EDGEFUNCTION_H

#include "../kipl_global.h"
#include <Faddeeva.hh>
#include "GaussianNoise.h"
#include <math.h>
#include "mathconstants.h"

using namespace Faddeeva;

namespace kipl { namespace math {


class KIPLSHARED_EXPORT edgefunction
{
public:
    edgefunction();
    ~edgefunction();

//private:
    double *erfc1;
    double *erfc2;
    double *gauss;
    double *edgeFunction;
    double sigma;
    double alpha;
    double t0;
    double *t;
    int size;
    void computeGauss(double *gauss, double *t);
    void computeErfc1(double *erfc, double *t);
    void computeErfc2(double *erfc, double *t);
    void computeEdge(double *t);
};

}}

#endif // EDGEFUNCTION_H

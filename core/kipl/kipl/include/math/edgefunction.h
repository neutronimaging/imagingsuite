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

//private: // should those be private or not?
    double *term1;
    double *term2;
    double *term3;
    double *term4;
    double *term5;
    double *edgeFunction;
    double sigma;
    double alpha;
    double t0;
    double a1;
    double a2;
    double a5;
    double a6;
    double *t;
    int size;
    void computeTerm4(double *fun, double *t);
    void computeTerm3(double *fun, double *t);
    void computeTerm5(double *fun, double *t);
    void computeTerm1(double *fun, double *t);
    void computeTerm2(double *fun, double *t);
    void computeSimplifiedEdge(double *t);
    void computeEdge(double *t);
};

}}

#endif // EDGEFUNCTION_H

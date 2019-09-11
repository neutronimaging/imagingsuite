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
    edgefunction(int size, double *t);
    ~edgefunction();

    void computeTerm4(double *fun, double *t);
    void computeTerm3(double *fun, double *t);
    void computeTerm5(double *fun, double *t);
    void computeTerm1(double *fun, double *t); // old implementation
    void computeTerm2(double *fun, double *t); // old implementation
    void computeLineBefore(double *fun, double *t);
    void computeLineAfter(double *fun, double *t);
    void computeExpBefore(double *fun, double *t);
    void computeExpAfter(double *fun, double *t);
    void computeSimplifiedEdge(double *t);
    void computeEdgeLinear(double *t);
    void computeEdgeExponential(double *t);

    double *edgeFunction;
    double t0;
    double sigma;
    double alpha;
    double a1;
    double a2;
    double a5;
    double a6;

//private: // should those be private or not?
    double *term1;
    double *term2;
    double *term3;
    double *term4;
    double *term5;
    double *line_before;
    double *line_after;
    double *exp_before;
    double *exp_after;

    double *fullEdge;

    double *t;
    int size;

};

}}

#endif // EDGEFUNCTION_H

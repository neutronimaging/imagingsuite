#ifndef GRADIENT_H
#define GRADIENT_H

#include <tnt.h>

namespace kipl {
namespace math {

/// implements the gradient of an array:
/// second order accurate central differences in the interior points
/// first order accurate one-sides (forward or backwards) differences at the boundaries
/// formulas are taken from numpy gradient
//void num_gradient(TNT::Array1D<double> &Y, TNT::Array1D<double> &X, int N, TNT::Array1D<double> &grad);
void num_gradient(double *Y, double *X, int N, double *grad);
}

}


#endif // GRADIENT_H

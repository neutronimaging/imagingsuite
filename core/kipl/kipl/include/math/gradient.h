#ifndef GRADIENT_H
#define GRADIENT_H

#include <tnt.h>
#include <type_traits>

namespace kipl {
namespace math {

/// implements the gradient of an array:
/// second order accurate central differences in the interior points
/// first order accurate one-sides (forward or backwards) differences at the boundaries
/// formulas are taken from numpy gradient
template <typename myType>
void num_gradient(myType *Y, myType *X, int N, myType *grad){

    static_assert(std::is_floating_point<myType>::value,
                    "num_gradient function can only be instantiated with floating point types");

    myType hs, hd, fhs, fhd, f;
    for (int i=0; i<N; ++i)
    {

        if (i==0) // for the first element I compute the forward derivative
        {
            grad[i] = (Y[i+1]-Y[i])/(X[i+1]-X[i]);
        }
        else if (i==N-1) // for the first element I compute the backward derivative
        {
            grad[i] = (Y[i]-Y[i-1])/(X[i]-X[i-1]);
        }
        else // for all other cases I compute the second order accurate centered derivative
        {
            hs = X[i]-X[i-1];
            hd = X[i+1]-X[i];
            f = Y[i];
            fhs = Y[i-1];
            fhd = Y[i+1];
            grad[i] = ( hs*hs*fhd + (hd*hd-hs*hs)*f - hd*hd*fhs)/ (hs*hd*(hs+hd));
        }

    }
}
}

}


#endif // GRADIENT_H

#include "../../include/math/gradient.h"

namespace kipl {
namespace math {


void num_gradient(TNT::Array1D<double> &Y, TNT::Array1D<double> &X, int N, TNT::Array1D<double> &grad)
{
    double hs, hd, fhs, fhd, f;
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

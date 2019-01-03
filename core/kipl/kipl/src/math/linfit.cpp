//<LICENCE>

#include <jama_qr.h>
#include "../../include/math/linfit.h"
using namespace TNT;

namespace kipl {
namespace math {

void weightedLSFit(TNT::Array2D<double> &H, TNT::Array2D<double> &C, TNT::Array2D<double> &y, TNT::Array2D<double> &param)
{
    TNT::Array2D<double> Ch=matmult(C,H);
    TNT::Array2D<double> Cy=matmult(C,y);

    JAMA::QR<double> qr(Ch);
    param = qr.solve(Cy);

}

}
}

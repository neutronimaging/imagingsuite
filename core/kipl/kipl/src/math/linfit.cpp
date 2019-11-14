//<LICENCE>

#include <armadillo>
#include <jama_qr.h>
#include "../../include/math/linfit.h"

using namespace TNT;

namespace kipl {
namespace math {

void weightedLSFit(TNT::Array2D<double> &H, TNT::Array2D<double> &C, TNT::Array2D<double> &y, TNT::Array2D<double> &param)
{
    TNT::Array2D<double> Ht(H.dim2(),H.dim1());

    for (int i=0; i<H.dim2(); ++i)
    {
        for (int j=0; j<H.dim1(); ++j)
        {
            Ht[i][j]=H[j][i];
        }
    }
    TNT::Array2D<double> HtC=matmult(Ht,C);
    TNT::Array2D<double> Ch=matmult(HtC,H);
    TNT::Array2D<double> Cy=matmult(HtC,y);

    JAMA::QR<double> qr(Ch);
    param = qr.solve(Cy);

}

void weightedLSFit(const arma::mat &H, const arma::mat &C, const arma::vec &y, arma::vec &param)
{
    arma::mat HtCH = H.t()*C*H;

    arma::mat HtCy = H.t()*C*y;

    param = arma::solve(HtCH,HtCy);
}

}
}

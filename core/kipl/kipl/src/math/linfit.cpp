//<LICENCE>

#include <armadillo>
#include <jama_qr.h>
#include "../../include/math/linfit.h"
#include <armadillo>
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

void weightedLSFit(arma::mat &H, arma::mat &C, arma::vec &y, arma::vec &param)
{
    arma::mat Ht=H.t();


    arma::mat HtC=Ht*C;
    arma::mat Ch=HtC*H;
    arma::mat Cy=HtC*y;

    param=arma::solve(Ch,Cy);
}
}
}

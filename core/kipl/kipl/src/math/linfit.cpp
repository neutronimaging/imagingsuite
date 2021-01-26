//<LICENCE>

#include "../../include/math/linfit.h"
#include <armadillo>

namespace kipl {
namespace math {

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

#ifndef LINFIT_HPP
#define LINFIT_HPP
namespace kipl { namespace math {

template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *q)
{
    LinearLSFit(x, y, N, q, q+1, q+2);
}

template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *m, double *k, double *R2)
{
    double sx=0.0;
    double sx2=0.0;
    for (int i=0; i<N; i++) {
        double xx=static_cast<double>(x[i]);
        sx+=xx;
        sx2+=xx*xx;
    }

    double det=(N*sx2-sx*sx);

    *m=0.0;
    *k=0.0;

    for (int i=0; i<N; i++) {
        double xx=static_cast<double>(x[i]);
        double yy=static_cast<double>(y[i]);
        *m+=yy*(sx2-xx*sx);
        *k+=yy*(N*xx-sx);
    }

    *m/=det;
    *k/=det;

    if (R2!=NULL) {
        double sstot=0.0;
        double ssres=0.0;
        double sy=0.0;
        for (int i=0; i<N; i++) {
            double yy=static_cast<double>(y[i]);
            sy+=yy;
        }

        double my=sy/N;
        double dmy;
        double ry;
        for (int i=0; i<N; i++) {
            double yy=static_cast<double>(y[i]);
            double xx=static_cast<double>(x[i]);

            sstot+=(yy-my)*(yy-my);

            ssres+=(yy-(*m+*k * xx)) * (yy-(*m+*k * xx));
        }

        *R2 = 1-ssres/sstot;
    }
}

}}
#endif // LINFIT_HPP

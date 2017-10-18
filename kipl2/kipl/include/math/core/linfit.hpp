#ifndef LINFIT_HPP
#define LINFIT_HPP
#include <algorithm>
#include <map>
#include <iostream>

namespace kipl { namespace math {

template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *q)
{
    LinearLSFit(x, y, N, q, q+1, q+2);
}

template <typename T, typename S> void LinearLSFit(T *x, S *y, int N, double *m, double *k, double *R2)
{
    std::cout<<"N="<<N<<std::endl;
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

template <typename T, typename S>
void LinearLSFit(T *x, S *y, int N, double *m, double *k, double *R2, double fraction)
{
    std::map<S,T> sorted;
    int N2=static_cast<int>(N*fraction);
    int Nstart=(N-N2)/2;

    for (int i=0; i<N; ++i)
        sorted.insert(std::make_pair(y[i],x[i]));

    auto it=sorted.begin();
    std::advance(it,Nstart);

    T *x2=new T[N2];
    S *y2=new S[N2];

    for (int i=0; i<N2; ++i, ++it) {
        x2[i]=it->second;
        y2[i]=it->first;
    }

    LinearLSFit(x2,y2,N2,m,k,R2);

    delete [] x2;
    delete [] y2;

}

}}
#endif // LINFIT_HPP

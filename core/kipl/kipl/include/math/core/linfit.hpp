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

template <typename T, typename S> void LinearLSFit(std::vector<std::pair<T,S>> &data, float &m, float &k, float *R2)
{
    double dm=0.0;
    double dk=0.0;
    double dR2=0.0;

    LinearLSFit(data,dm,dk,&dR2);

    m=static_cast<float>(dm);
    k=static_cast<float>(dk);

    if (R2!=nullptr)
        *R2=static_cast<float>(dR2);

}

template <typename T, typename S> void LinearLSFit(std::vector<std::pair<T,S>> &data, double &m, double &k, double *R2)
{
    int N= static_cast<int>(data.size());
    T *x = new T[N];
    S *y = new S[N];

    int i=0;
    for (auto &point : data)
    {
        x[i]=point.first;
        y[i]=point.second;
        ++i;
    }
    LinearLSFit(x,y,N,&m,&k,R2);

    delete [] x;
    delete [] y;
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

    if (R2!=nullptr) {
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

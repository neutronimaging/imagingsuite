//<LICENSE>
#ifndef LINFIT_HPP
#define LINFIT_HPP
#include <algorithm>
#include <map>
#include <iostream>
#include <armadillo>

#if !defined(NO_QT)
#include <QDebug>
#endif
#include "../../base/KiplException.h"

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
    LinearLSFit(x,y,N,m,k,R2);


    std::map< double,std::pair<double,double> > sorted;
    if ((fraction<0.0) || (1.0<fraction))
        throw kipl::base::KiplException("Fraction outside the interval [0,1]",__FILE__,__LINE__);

    int N2=static_cast<int>(N*fraction);

    for (int i=0; i<N; ++i)
        sorted.insert(std::make_pair(fabs(y[i]-(*k*x[i]+*m)),std::make_pair(static_cast<double>(x[i]),static_cast<double>(y[i]))));


    auto it=sorted.begin();

    T *x2=new T[N2];
    S *y2=new S[N2];

    for (int i=0; i<N2; ++i, ++it) {
        x2[i]=it->second.first;
        y2[i]=it->second.second;
    }

    LinearLSFit(x2,y2,N2,m,k,R2);

    delete [] x2;
    delete [] y2;

}

template<typename T, typename S>
std::vector<double> polyFit(const std::vector<T> &xx,const std::vector<S> &yy, int polyOrder)
{
    if (yy.size()!=xx.size())
        throw kipl::base::KiplException("x and y vectors are not same lenght",__FILE__,__LINE__);

    std::vector<double> coef(polyOrder+1);
    std::fill(coef.begin(),coef.end(),0.0);

    arma::mat A(xx.size(),polyOrder+1);
    arma::vec y(yy.size());

    for (size_t i=0; i<xx.size(); ++i) {
        for (int order=0; order<=polyOrder; ++order)
            A(i,order) = std::pow(xx[i],order);
    }

    for (size_t i=0; i<yy.size(); ++i)
        y(i) = yy[i];

    arma::vec c = arma::solve(A,y);

    for (arma::uword i = 0; i<c.n_elem; ++i)
        coef[i]=c(i);

    return coef;
}

template<typename T, typename S>
std::vector<double> polyVal(const std::vector<T> &xx,const std::vector<S> &c)
{
    std::vector<double> y(xx.size());

    auto itY = y.begin();
    for (const auto & x : xx)
    {
        double res=0.0;
        double xp = 1;
        for (int i=0; i<c.size(); ++i)
        {
            res += c[i]*xp;
            xp *=x;
        }
        *itY = res;
        ++itY;
    }

    return y;
}

template<typename T>
std::vector<T> polyDeriv(const std::vector<T> &c,int deriv)
{
    if (deriv<1)
        return c;

    std::vector<T> dc;
    if (deriv==1)
    {
        for (int i=1; i<c.size(); ++i)
        {
            dc.push_back(i*c[i]);
        }
    }
    else
    {
       auto cc = polyDeriv(c,deriv-1);
       for (int i=1; i<cc.size(); ++i)
       {
           dc.push_back(i*cc[i]);
       }
    }

    return dc;

}
}}
#endif // LINFIT_HPP

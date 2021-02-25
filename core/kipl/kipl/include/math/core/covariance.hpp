#ifndef COVARIANCE_HPP
#define COVARIANCE_HPP

#include <cmath>
#include <sstream>
#include <iostream>
#include <armadillo>
#include "../../base/KiplException.h"

#include "../covariance.h"

namespace kipl {
namespace math {

template<typename T>
Covariance<T>::Covariance() :
    logger("Covariance"),
    m_pData(nullptr),
    m_nVars(0),
    m_nElements(0),
    m_eResultMatrixType(CovarianceMatrix)
{}

template<typename T>
Covariance<T>::~Covariance()
{
    DeallocateData();
}

template<typename T>
arma::mat Covariance<T>::compute(T *data, size_t nElements, size_t nVars, bool bCenter)
{
    m_nVars=nVars;
    m_pData=data;
    arma::mat cov(m_nVars,m_nVars);

    if (bCenter)
        ComputeStatistics();

    for (int i=0; i<m_nVars; i++) {
        for (int j=i; j<m_nVars; j++) {
            cov.at(j,i)=cov.at(i,j)=ComputePairSum(i,j);
        }
    }

    NormalizeMatrix(cov);
    return cov;
}

template<typename T>
arma::mat Covariance<T>::compute(T *data, const std::vector<size_t> &dims, size_t Ndims, bool bCenter)
{
    m_nVars=dims[Ndims-1];
    m_pData=data;
    arma::mat cov(m_nVars,m_nVars);

    m_nElements=dims[0];

    for (size_t i=1; i<(Ndims-1); i++)
        m_nElements*=dims[i];

    if (bCenter)
        ComputeStatistics();

    for (int i=0; i<m_nVars; i++)
    {
        for (int j=i; j<m_nVars; j++)
        {
            if (i==j)
                cov.at(i,j)=ComputePairSum(i,j,bCenter);
            else
            {
                cov.at(i,j)=ComputePairSum(i,j,bCenter);
                cov.at(j,i)=cov.at(i,j);
            }
        }
    }

    NormalizeMatrix(cov);
    return cov;
}

template <typename T>
double Covariance<T>::ComputePairSum(int a,int b, bool bCenter)
{
    double sum=0.0;

    T *pA=m_pData+m_nElements*a;
    T *pB=m_pData+m_nElements*b;

    if (bCenter) {
        double ma=m_fMean[a];
        double mb=m_fMean[b];


        for (int i=0; i<m_nElements; i++) {
            sum+=(pA[i]-ma)*(pB[i]-mb);
        }
    }
    else {
        for (int i=0; i<m_nElements; i++) {
            sum+=pA[i]*pB[i];
        }
    }

    return sum;
}

template <typename T>
void Covariance<T>::AllocateData()
{
    m_fMean.resize(m_nElements);
}

template <typename T>
void Covariance<T>::DeallocateData()
{

}


template <typename T>
void Covariance<T>::ComputeStatistics()
{
    AllocateData();

    double dElements=static_cast<double>(m_nElements);
    for (int i=0; i<m_nVars; i++)
    {
        double sum=0.0;
        T* d=m_pData+i*m_nElements;
        for (int j=0; j<m_nElements; j++) {
            double dd=static_cast<double>(d[j]);
            sum+=dd;
        }
        m_fMean[i]=sum/dElements;
    }
}

template<typename T>
void Covariance<T>::NormalizeMatrix(arma::mat &mat)
{
    double scale;
    switch (m_eResultMatrixType)
    {
    case CovarianceMatrix:
        scale=1.0/double(m_nElements);
        for (int i=0 ; i<m_nVars; i++)
        {
            for (int j=i ; j<m_nVars; j++)
            {
                if (i==j)
                    mat.at(i,j)*=scale;
                else
                {
                    mat.at(i,j)*=scale;
                    mat.at(j,i)*=scale;
                }
            }
        }
    break;

    case CorrelationMatrix:
        for (int i=0 ; i<m_nVars; i++) {
            for (int j=i ; j<m_nVars; j++) {
                if (i!=j) {
                    scale=1.0/std::sqrt(mat.at(i,i)*mat.at(j,j));
                    mat.at(i,j)*=scale;
                    mat.at(j,i)*=scale;
                }
            }
            mat.at(i,i)=1.0;
        }
    break;
    default: throw kipl::base::KiplException("Unknown matrix normalization", __FILE__,__LINE__);
    }

}

}
}
#endif // COVARIANCE_HPP

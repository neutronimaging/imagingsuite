#ifndef COVARIANCE_HPP
#define COVARIANCE_HPP

#include <cmath>
#include <sstream>
#include <iostream>

#include "../../base/KiplException.h"

namespace kipl {
namespace math {

template<typename T>
Covariance<T>::Covariance() :
    logger("Covariance"),
    m_pData(NULL),
    m_nVars(0),
    m_nElements(0),
    m_fMean(NULL),
    m_eResultMatrixType(CovarianceMatrix)
{}

template<typename T>
Covariance<T>::~Covariance()
{
    DeallocateData();
}

template<typename T>
TNT::Array2D<double> Covariance<T>::compute(T *data, size_t nElements, size_t nVars, bool bCenter)
{
    m_nVars=nVars;
    m_pData=data;
    TNT::Array2D<double> cov(m_nVars,m_nVars);

    if (bCenter)
        ComputeStatistics();

    for (int i=0; i<m_nVars; i++) {
        for (int j=i; j<m_nVars; j++) {
            cov[j][i]=cov[i][j]=ComputePairSum(i,j);
        }
    }

    NormalizeMatrix(cov);
    return cov;
}

template<typename T>
TNT::Array2D<double> Covariance<T>::compute(T *data, const size_t *dims, size_t Ndims, bool bCenter)
{
    m_nVars=dims[Ndims-1];
    m_pData=data;
    TNT::Array2D<double> cov(m_nVars,m_nVars);

    m_nElements=dims[0];

    for (size_t i=1; i<(Ndims-1); i++)
        m_nElements*=dims[i];

    if (bCenter)
        ComputeStatistics();

    for (int i=0; i<m_nVars; i++) {
        for (int j=i; j<m_nVars; j++) {
            if (i==j)
                cov[i][j]=ComputePairSum(i,j,bCenter);
            else {
                cov[i][j]=ComputePairSum(i,j,bCenter);
                cov[j][i]=cov[i][j];
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
    DeallocateData();

    m_fMean=new double[m_nElements];
}

template <typename T>
void Covariance<T>::DeallocateData()
{
    if (m_fMean!=NULL)
        delete [] m_fMean;
    m_fMean=NULL;
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
void Covariance<T>::NormalizeMatrix(TNT::Array2D<double> &mat)
{
    double scale;
    switch (m_eResultMatrixType) {
    case CovarianceMatrix:
        scale=1.0/double(m_nElements);
        for (int i=0 ; i<m_nVars; i++) {
            for (int j=i ; j<m_nVars; j++) {
                if (i==j)
                    mat[i][j]*=scale;
                else {
                    mat[i][j]*=scale;
                    mat[j][i]*=scale;
                }
            }
        }
    break;

    case CorrelationMatrix:
        for (int i=0 ; i<m_nVars; i++) {
            for (int j=i ; j<m_nVars; j++) {
                if (i!=j) {
                    scale=1/sqrt(mat[i][i]*mat[j][j]);
                    mat[i][j]*=scale;
                    mat[j][i]*=scale;
                }
            }
            mat[i][i]=1.0;
        }
    break;
    default: throw kipl::base::KiplException("Unknown matrix normalization", __FILE__,__LINE__);
    }

}

}
}
#endif // COVARIANCE_HPP

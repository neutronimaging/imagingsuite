//<LICENCE>

#ifndef __PCA_HPP
#define __PCA_HPP

#include <jama_eig.h>
#include <tnt_array2d_utils.h>

#include "../include/pca/pca.h"
#include "../include/math/covariance.h"
#include "../include/math/image_statistics.h"
#include "../include/math/tnt_utils.h"

namespace kipl {
namespace pca {

PCA::PCA() :
    logger("PCA"),
    m_bCenterData(false),
    m_bNormalizeData(false),
    m_fMean(NULL),
    m_fStdDev(NULL),
    m_eCovType(kipl::math::CorrelationMatrix),
    m_eDecompositionType(kipl::pca::PCA_Eigen)
{

}

PCA::~PCA()
{
    if (m_fMean!=NULL)
        delete [] m_fMean;

    if (m_fStdDev!=NULL)
        delete [] m_fStdDev;
}

void PCA::eigenimages(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,3> &eig)
{
    ComputeTransformMatrix(img,0);
    ComputeTransform(img,eig);
}

void PCA::filter(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,3> &filt, int level)
{
    NormalizeData(img);
    ComputeTransformMatrix(img,level);
    ComputeTransform(img,filt);
    UndoNormalizeData(filt);
}

TNT::Array2D<double> PCA::cov()
{
    return m_mCovariance;
}

TNT::Array1D<double> PCA::eigenvalues()
{
    return m_mEigenValues;
}

TNT::Array2D<double> PCA::eigenvectors()
{
    return m_mEigenVectors;
}

void PCA::setCovarianceType(kipl::math::eCovarianceType ct)
{
    m_eCovType=ct;
}

kipl::math::eCovarianceType PCA::getCovarianceType()
{
    return m_eCovType;
}

void PCA::setDecompositionType(ePCA_DecompositionType dt)
{
    m_eDecompositionType = dt;
}

pca::ePCA_DecompositionType PCA::getDecompositionType()
{
    return m_eDecompositionType;
}

void PCA::ComputeTransformMatrix(kipl::base::TImage<float,3> &img, int level)
{
    if (m_eDecompositionType == kipl::pca::PCA_SVD) {
        logger(kipl::logging::Logger::LogError, "Covariance decomposition using SVD is not implemented");
        throw kipl::base::KiplException("SVD decomposition is not implemented",__FILE__,__LINE__);
    }

    kipl::math::Covariance<float> cov;

    cov.setResultMatrixType(m_eCovType);
    m_mCovariance=cov.compute(img.GetDataPtr(),img.Dims(),3);

    JAMA::Eigenvalue<double> eig(m_mCovariance);

    eig.getV(m_mEigenVectors);
    eig.getRealEigenvalues(m_mEigenValues);

    if (0<level) { // Suppress contributions from eigen images greater than level
        TNT::Array2D<double> filterMatrix=TNT::transpose(m_mEigenVectors);

        for (int r=level; r<filterMatrix.dim1(); r++) {
                for (int c=0; c<filterMatrix.dim2(); c++) {
                    filterMatrix[r][c]=0.0;
                }
        }

        m_mTransformMatrix=TNT::matmult(m_mEigenVectors,filterMatrix);
    }
    else { // Compute eigen images
        m_mTransformMatrix=m_mEigenVectors.copy();
    }
}

void PCA::ComputeTransform(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,3> &res)
{
    res.Resize(img.Dims());
    res=0.0f;

    TNT::Array2D<double> &m=m_mTransformMatrix;

    // Basic implementation
    size_t N=img.Size(0)*img.Size(1);
    for (int c=0; c<m.dim2(); c++) {
        float *pRes=res.GetLinePtr(0,c);

        for (int r=0; r<m.dim1(); r++) {
            float *pImg=img.GetLinePtr(0,r);
            float w=static_cast<float>(m[r][c]);

            for (size_t i=0; i<N; i++) {
                pRes[i]+=w*pImg[i];
            }
        }
    }
}

void PCA::NormalizeData(base::TImage<float,3> &img)
{
    if (m_bCenterData) {
        size_t N=img.Size(0)*img.Size(1);
        int nSlices = static_cast<int>(img.Size(2));

        kipl::math::statistics(img.GetDataPtr(),m_fMean, m_fStdDev, img.Dims(), 3, true);

        for (int slice=0; slice<nSlices; slice++) {
            float *pSlice=img.GetLinePtr(0,slice);
            float fMean=static_cast<float>(m_fMean[slice]);
            float fStdDev=1.0f/static_cast<float>(m_fStdDev[slice]);

            if (m_bNormalizeData) {
                for (size_t i=0; i<N; i++) {
                    pSlice[i]=(pSlice[i]-fMean)*fStdDev;
                }
            }
            else {
                for (size_t i=0; i<N; i++) {
                    pSlice[i]=pSlice[i]-fMean;
                }
            }
        }
    }
}

void PCA::UndoNormalizeData(base::TImage<float,3> &img)
{
    if (m_bCenterData) {
        size_t N=img.Size(0)*img.Size(1);
        int nSlices = static_cast<int>(img.Size(2));

        for (int slice=0; slice<nSlices; slice++) {
            float *pSlice=img.GetLinePtr(0,slice);
            float fMean=static_cast<float>(m_fMean[slice]);
            float fStdDev=static_cast<float>(m_fStdDev[slice]);

            if (m_bNormalizeData) {
                for (size_t i=0; i<N; i++) {
                    pSlice[i]=pSlice[i]*fStdDev+fMean;
                }
            }
            else {
                for (size_t i=0; i<N; i++) {
                    pSlice[i]=pSlice[i]+fMean;
                }
            }
        }
    }
}

void PCA::setCenterNormalize(bool center, bool norm)
{
    m_bCenterData    = center;
    m_bNormalizeData = norm;
}

void PCA::getCenterNormalize(bool &center, bool &norm)
{
    center = m_bCenterData;
    norm   = m_bNormalizeData;
}

}}

std::string enum2string(kipl::pca::ePCA_DecompositionType dt)
{
    switch (dt) {
    case kipl::pca::PCA_Eigen : return "Eigen"; break;
    case kipl::pca::PCA_SVD   : return "SVD"; break;
    default: throw kipl::base::KiplException("Unknown value for PCA decomposition type",__FILE__, __LINE__);
    }

    return "no enum";
}

void string2enum(std::string str, kipl::pca::ePCA_DecompositionType &dt)
{
    if (str=="Eigen") dt=kipl::pca::PCA_Eigen;
    else if (str=="SVD") dt=kipl::pca::PCA_SVD;
    else throw kipl::base::KiplException("String does not match decomposition enum.",__FILE__,__LINE__);
}

std::ostream & operator<<(std::ostream &s, kipl::pca::ePCA_DecompositionType dt)
{
    s<<enum2string(dt);

    return s;
}

#endif

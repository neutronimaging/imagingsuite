//<LICENCE>

#ifndef PCA_H
#define PCA_H

#include "../kipl_global.h"
#include <iostream>
#include <string>

#include <armadillo>
#include "../base/timage.h"
#include "../logging/logger.h"
#include "../math/covariance.h"
#include "../base/KiplException.h"


namespace kipl {
namespace pca {

typedef enum {
    PCA_Eigen,
    PCA_SVD
} ePCA_DecompositionType;

class KIPLSHARED_EXPORT PCA {
    kipl::logging::Logger logger;
public:
    PCA();
    virtual ~PCA();

    void eigenimages(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,3> &eig);
    void filter(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,3> &filt, int level);
    arma::mat cov();
    arma::vec eigenvalues();
    arma::mat eigenvectors();

    void setCovarianceType(kipl::math::eCovarianceType ct);
    kipl::math::eCovarianceType getCovarianceType();

    void setDecompositionType(kipl::pca::ePCA_DecompositionType dt);
    kipl::pca::ePCA_DecompositionType getDecompositionType();
    void setCenterNormalize(bool center, bool norm);
    void getCenterNormalize(bool &center, bool &norm);

protected:
    void ComputeTransformMatrix(kipl::base::TImage<float,3> &img, int level);
    void ComputeTransform(kipl::base::TImage<float,3> &img, kipl::base::TImage<float,3> &res);
    bool m_bCenterData;
    bool m_bNormalizeData;
    void NormalizeData(kipl::base::TImage<float,3> &img);
    void UndoNormalizeData(kipl::base::TImage<float,3> &img);

    kipl::base::TImage<float,3> m_EigenImages;
    double * m_fMean;
    double * m_fStdDev;

    arma::mat m_mCovariance;
    kipl::math::eCovarianceType m_eCovType;

    kipl::pca::ePCA_DecompositionType m_eDecompositionType;

    arma::mat m_mEigenVectors;
    arma::vec m_mEigenValues;
    arma::mat m_mTransformMatrix;
};
}
}

std::string KIPLSHARED_EXPORT enum2string(kipl::pca::ePCA_DecompositionType dt);
void KIPLSHARED_EXPORT string2enum(std::string str, kipl::pca::ePCA_DecompositionType &dt);
std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &s, kipl::pca::ePCA_DecompositionType dt);
#endif

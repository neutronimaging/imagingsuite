//<LICENCE>

#ifndef COVARIANCE_H
#define COVARIANCE_H

#include "../kipl_global.h"
#include <tnt_array2d.h>
#include <vector>

#include "../logging/logger.h"

namespace kipl {
namespace math {

/// \brief Enum to select type of covariance matrix
typedef enum {
    CovarianceMatrix,   ///< Compute a covariance matrix
    CorrelationMatrix   ///< Compute a correlation matrix
} eCovarianceType;


/// \brief A class to compute the covariance matrix for several data sets
template<typename T>
class Covariance {
    kipl::logging::Logger logger;
public:
    Covariance();
    ~Covariance();

    /// \brief Compute the covariance matrix between different variables
    /// \param data All data is stored to beaccessed through this variable. The shall be nElements x nVars elements in the data array. Each varaible is stored blockwise.
    /// \param nElements Number of elements in each variable
    /// \param nVars number of stochastic variables
    /// \param bCenter subtract the average value from each variable before computing the covariance matrix.
    TNT::Array2D<double> compute(T *data, size_t nElements, size_t nVars, bool bCenter=true);

    /// \brief Compute the covariance matrix between different variables.
    ///
    /// This method is intended to be used with images.
    /// \param data All data is stored to beaccessed through this variable. The shall be product dims elements in the data array. Each varaible is stored blockwise.
    /// \param dims Dimensions of the entire data block. The last entry tells how many stochastic varibales the data set contains.
    /// \param nDims number of data dimensions.
    /// \param bCenter subtract the average value from each variable before computing the covariance matrix.
    TNT::Array2D<double> compute(T *data, const std::vector<size_t> &dims, size_t Ndims, bool bCenter=true);

    /// \brief Setter for the covariance matrix type
    /// \param m Type value
    void setResultMatrixType(eCovarianceType m) {m_eResultMatrixType = m;}

    /// \brief Getter for covariance matrix type
    eCovarianceType getResultMatrixType() {return m_eResultMatrixType;}
protected:
    double ComputePairSum(int a, int b, bool bCenter);
    void AllocateData();
    void DeallocateData();
    void ComputeStatistics();
    void NormalizeMatrix(TNT::Array2D<double> &mat);

    T *m_pData;
    int m_nVars;
    int m_nElements;
    double *m_fMean;
    eCovarianceType m_eResultMatrixType;
};

}
}

#include "core/covariance.hpp"

std::string KIPLSHARED_EXPORT enum2string(kipl::math::eCovarianceType ct);
void KIPLSHARED_EXPORT string2enum(std::string str, kipl::math::eCovarianceType &dt);
std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &s, kipl::math::eCovarianceType dt);

#endif // COVARIANCE_H

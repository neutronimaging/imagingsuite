#ifndef COVARIANCE_H
#define COVARIANCE_H

#include "../logging/logger.h"
#include <tnt_array2d.h>

namespace kipl {
namespace math {

typedef enum {
    CovarianceMatrix,
    CorrelationMatrix
} eCovarianceType;


template<typename T>
class Covariance {
    kipl::logging::Logger logger;
public:
    Covariance();
    ~Covariance();

    TNT::Array2D<double> compute(T *data, size_t nElements, size_t nVars, bool bCenter=true);
    TNT::Array2D<double> compute(T *data, const size_t *dims, size_t Ndims, bool bCenter=true);

    void setResultMatrixType(eCovarianceType m) {m_eResultMatrixType = m;}
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

std::string enum2string(kipl::math::eCovarianceType ct);
void string2enum(std::string str, kipl::math::eCovarianceType &dt);
std::ostream & operator<<(std::ostream &s, kipl::math::eCovarianceType dt);

#endif // COVARIANCE_H

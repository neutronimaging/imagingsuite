//<LICENCE>
#include <armadillo>
#include "../../include/segmentation/multivariateclassifyerbase.h"

#include "../include/base/KiplException.h"
namespace kipl {

namespace segmentation {
ClassDescriptor::ClassDescriptor(std::string _name)
{

}

/// \brief Create an initialized class descriptor
/// \param _m mean value vector
ClassDescriptor::ClassDescriptor(arma::vec _m,
                arma::mat _C,
                std::string _name)
{
    throw kipl::base::KiplException("Distance metrics are not implemented",__FILE__, __LINE__);

}

/// \brief Copy c'tor
/// \param cd A class descriptor object
ClassDescriptor::ClassDescriptor(ClassDescriptor &cd)
{

}

/// \brief Copy c'tor
/// \param cd A class descriptor object
ClassDescriptor & ClassDescriptor::operator=(ClassDescriptor &cd)
{

    return *this;
}

/// \brief Computes the statistical distance between two ClassDescriptors
/// \param cd A class descriptor object
/// \param metric Selects the metric to calculate the distance
double ClassDescriptor::Distance(ClassDescriptor &cd, eDistanceMetric metric)
{
    if (m.n_elem!=cd.m.n_elem)
        throw kipl::base::KiplException("Class distance: mean value matrix size mismatch",__FILE__,__LINE__);

    if ((C.n_rows!=cd.C.n_rows) || (C.n_cols!=cd.C.n_cols))
        throw kipl::base::KiplException("Class distance: Covariance matrix size mismatch",__FILE__,__LINE__);

    return 0.0;
}

double ClassDescriptor::EuclideanDistance(ClassDescriptor &cd)
{
    double val=0.0;
    double sum=0.0;

    for (int i=0; i<m.n_elem; i++) {
        //val=m[i][0]-cd.m[i][0];
        sum+=val*val;
    }

    return sqrt(sum);
}

double ClassDescriptor::MahalanobisDistance(ClassDescriptor &cd)
{
    throw kipl::base::KiplException("MahalanobisDistance is not implemented",__FILE__,__LINE__);

    return 0;
}

double ClassDescriptor::BhattacharyyaDistance(ClassDescriptor &cd)
{
//    TNT::Array2D<double> d;
//    TNT::Array1D<double> mm=m-cd.m;
//    TNT::Array2D<double> cc=C+cd.C;

//    d=TNT::matmult(cc,mm);
//    for (int i=0; i<mm.dim())

//    if ((d.num_cols()!=1) || (d.num_rows()!=1))
//        throw kipl::base::KiplException("Matrix multiplication did not result in a scalar",__FILE__,__LINE__);

//    double dist=0.125*d[0][0]-0.5*log(TNT::);


    return 0.0;
}

MultiVariateClassifyerBase::MultiVariateClassifyerBase()
{

}

}} // closing namespace

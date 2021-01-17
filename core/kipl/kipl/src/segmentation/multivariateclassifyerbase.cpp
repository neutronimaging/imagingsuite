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
                std::string _name) :
    m(_m),
    C(_C),
    className(_name)
{
    throw kipl::base::KiplException("Distance metrics are not implemented",__FILE__, __LINE__);

}

/// \brief Copy c'tor
/// \param cd A class descriptor object
ClassDescriptor::ClassDescriptor(ClassDescriptor &cd) :
    m(cd.m),
    C(cd.C),
    className(cd.className)
{

}

/// \brief Copy c'tor
/// \param cd A class descriptor object
ClassDescriptor & ClassDescriptor::operator=(ClassDescriptor &cd)
{
    m = cd.m;
    C = cd.C;
    className = cd.className;
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

    switch (metric)
    {
    case Euclidean : return EuclideanDistance(cd);
    case Euclidean2 : return EuclideanDistance2(cd);
    case Mahalanobis : return MahalanobisDistance(cd);
    case Bhattacharyya : return BhattacharyyaDistance(cd);
    }

    return 0.0;
}

double ClassDescriptor::EuclideanDistance(ClassDescriptor &cd)
{
    return sqrt(EuclideanDistance2(cd));
}

double ClassDescriptor::EuclideanDistance2(ClassDescriptor &cd)
{
    double val=0.0;
    double sum=0.0;

    for (arma::uword i=0; i<m.n_elem; i++)
    {
        sum+=val*val;
    }

    return sum;
}

double ClassDescriptor::MahalanobisDistance(ClassDescriptor &cd)
{
    throw kipl::base::KiplException("MahalanobisDistance is not implemented",__FILE__,__LINE__);

    return 0;
}

double ClassDescriptor::BhattacharyyaDistance(ClassDescriptor &cd)
{
    throw kipl::base::KiplException("BhattacharyyaDistance is not implemented",__FILE__,__LINE__);
    arma::mat d;
    arma::mat mm=m-cd.m;
    arma::mat cc=C+cd.C;

    d=cc * mm;
//    for (int i=0; i<mm.dim())

    if ((d.n_cols!=1) || (d.n_rows!=1))
        throw kipl::base::KiplException("Matrix multiplication did not result in a scalar",__FILE__,__LINE__);

    double dist=0.125*d.at(0)-0.5*log(1);


    return dist;
}

MultiVariateClassifyerBase::MultiVariateClassifyerBase()
{

}

}} // closing namespace

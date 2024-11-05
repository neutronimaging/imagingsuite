

#ifndef LAPLACIANOFGRADIENTS_H
#define LAPLACIANOFGRADIENTS_H

#include "../base/timage.h"

namespace kipl { namespace filters {

/// \brief The filter computes the Laplacian of the Gaussian
/// \param img The image to be filtered
/// \param sigma Width of the Gaussian
template <typename T>
kipl::base::TImage<float,2> LaplacianOfGaussian(kipl::base::TImage<T,2> & img, float sigma);

}}

#include "core/laplacianofgaussian.hpp"
#endif // LAPLACIANOFGRADIENTS_H

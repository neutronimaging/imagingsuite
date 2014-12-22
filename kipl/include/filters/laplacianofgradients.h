#ifndef LAPLACIANOFGRADIENTS_H
#define LAPLACIANOFGRADIENTS_H

#include "../base/timage.h"

namespace kipl { namespace filters {
template <typename T>
kipl::base::TImage<T,2> LaplacianOfGradients(kipl::base::TImage<T,2> & img, float sigma);

}}

#include "core/laplacianofgradients.hpp"
#endif // LAPLACIANOFGRADIENTS_H

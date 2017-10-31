#ifndef CIRCULARHOUGHTRANSFORM_H
#define CIRCULARHOUGHTRANSFORM_H

#include "../base/timage.h"

namespace kipl { namespace math {

class CircularHoughTransform
{
public:
    CircularHoughTransform();

    kipl::base::TImage<float,2> operator()(kipl::base::TImage<float> img, float diameter);
};
}}
#endif // CIRCULARHOUGHTRANSFORM_H

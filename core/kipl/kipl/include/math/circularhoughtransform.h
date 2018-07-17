#ifndef CIRCULARHOUGHTRANSFORM_H
#define CIRCULARHOUGHTRANSFORM_H

#include "../kipl_global.h"
#include "../base/timage.h"
#include "../logging/logger.h"

namespace kipl { namespace math {

class KIPLSHARED_EXPORT CircularHoughTransform
{
    kipl::logging::Logger logger;
public:
    CircularHoughTransform();

    kipl::base::TImage<float,2> operator()(kipl::base::TImage<float,2> img, float diameter, bool useDerivative=false);
private:
    void buildKernel(float radius);
    void absDerivative(bool useDerivative);
    kipl::base::TImage<float,2> m_Img;
    kipl::base::TImage<float,2> ringKernel;
};
}}
#endif // CIRCULARHOUGHTRANSFORM_H

#include "../../include/math/circularhoughtransform.h"
#include "../../include/drawing/drawing.h"
#include "../../include/filters/filter.h"
#include "../../include/math/sums.h"

namespace kipl { namespace math {

CircularHoughTransform::CircularHoughTransform()
{

}

kipl::base::TImage<float,2> CircularHoughTransform::operator()(kipl::base::TImage<float,2> img, float radius)
{
    size_t kerneldims[2]={static_cast<size_t>(2*ceil(radius)+1),static_cast<size_t>(2*ceil(radius)+1)};
    kipl::base::TImage<float,2> kernel;

    kipl::drawing::Circle<float> circle(radius);
    int center=kerneldims[0]/2;

    circle.Draw(kernel,center,center,1.0);
    float s=sum(kernel.GetDataPtr(), kernel.Size());

    kernel*=1.0f/s;

    kipl::filters::TFilter<float,2> cht(kernel.GetDataPtr(),kernel.Dims());

    kipl::base::TImage<float,2> chtmap=cht(img,kipl::filters::FilterBase::EdgeMirror);

    return chtmap;
}

}}

//<LICENSE>
#include <cmath>

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
    kipl::filters::TFilter<float,2> cht(ringKernel.GetDataPtr(),ringKernel.Dims());

    kipl::base::TImage<float,2> chtmap=cht(img,kipl::filters::FilterBase::EdgeMirror);

    return chtmap;
}

void CircularHoughTransform::buildKernel(float radius)
{
    int N=ceil(radius+1)*2+1;
    int C=N/2;
    size_t dims[2]={N,N};
    ringKernel.Resize(dims);

    int i=0;
    float sum=0.0f;

    for (int y=0; y<N; ++y) {
        float y2=(y-C)*(y-C);

        for (int x=0; x<N; ++x, ++i) {
            ringKernel[i]=abs(sqrt((x-C)*(x-C)+y2))<0.5f;
            sum+=ringKernel[i];
        }
    }

    for (int i=0; i<ringKernel.Size(); ++i)
        ringKernel[i]/=sum;

    return 0;
}
}}

//<LICENSE>
#include <cmath>
#include <sstream>
#include <iostream>

#include "../../include/math/circularhoughtransform.h"
#include "../../include/drawing/drawing.h"
#include "../../include/filters/filter.h"
#include "../../include/math/sums.h"
#include "../../include/io/io_tiff.h"

namespace kipl { namespace math {

CircularHoughTransform::CircularHoughTransform() :
    logger("CircularHoughTransform")
{

}

kipl::base::TImage<float,2> CircularHoughTransform::operator()(kipl::base::TImage<float,2> img, float radius, bool useDerivative)
{
    m_Img.Clone(img);
    absDerivative(useDerivative);

    buildKernel(radius);

    kipl::filters::TFilter<float,2> cht(ringKernel.GetDataPtr(),ringKernel.Dims());

    kipl::base::TImage<float,2> chtmap=cht(m_Img,kipl::filters::FilterBase::EdgeMirror);

    return chtmap;
}

void CircularHoughTransform::buildKernel(float radius)
{
    int N=ceil(radius+1)*2+1;
    int C=N/2;
    size_t dims[2]={static_cast<size_t>(N),static_cast<size_t>(N)};

    ringKernel.Resize(dims);

    int i=0;
    float sum=0.0f;

    for (int y=0; y<N; ++y) {
        float y2=(y-C)*(y-C);

        for (int x=0; x<N; ++x, ++i) {
            ringKernel[i]=abs(sqrt((x-C)*(x-C)+y2)-radius)<0.5f;
            sum+=ringKernel[i];
        }
    }

    for (size_t i=0; i<ringKernel.Size(); ++i)
        ringKernel[i]/=sum;

    std::ostringstream msg;

    msg<<"Kernel info: "<<ringKernel;
    logger(logger.LogMessage,msg.str());
}

void CircularHoughTransform::absDerivative(bool useDerivative)
{
    if (useDerivative) {
        logger(logger.LogMessage, "Using absolute derivative");

        size_t filtdims[2]={3,3};

        float vkernel[9]={-3.0f, -10.0f, -3.0f, 0.0f,0.0f,0.0f,3.0f,10.0f,3.0f};
        kipl::filters::TFilter<float,2> vert(vkernel,filtdims);
        kipl::base::TImage<float,2> dy=vert(m_Img,kipl::filters::FilterBase::EdgeMirror);

        float hkernel[9]={-3.0f, 0.0f, 3.0f,-10.0f,0.0f,10.0f,-3.0,0.0f,3.0f};
        kipl::filters::TFilter<float,2> hor(hkernel,filtdims);
        kipl::base::TImage<float,2> dx=hor(m_Img,kipl::filters::FilterBase::EdgeMirror);

        for (size_t i=0; i<m_Img.Size(); ++i)
        {
            m_Img[i]=sqrt(dx[i]*dx[i]+dy[i]*dy[i]);
        }
    }

}

}}

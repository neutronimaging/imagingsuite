//<LICENCE>

#include "../../include/math/mathfunctions.h"
#include "../../include/filters/filterbase.h"
#include "../../include/filters/filter.h"
#include "../../include/filters/stddevfilter.h"


namespace kipl { namespace filters {


StdDevFilter::StdDevFilter(size_t size, FilterBase::EdgeProcessingStyle eps) :
    m_FilterSize(size),
    m_eEdgeProcessingStyle(eps)
{

}

StdDevFilter::~StdDevFilter()
{

}

kipl::base::TImage<float,2> StdDevFilter::operator() (kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> img2=kipl::math::sqr(img);

    kipl::base::TImage<float,2> s2=BoxFilter(img2);
    kipl::base::TImage<float,2> m=BoxFilter(img);

    img2=0.0f;
    float *pRes=img2.GetDataPtr();
    float *pS2=s2.GetDataPtr();
    float *pM=m.GetDataPtr();

    const float M=1/float(m_FilterSize*m_FilterSize);
    const float M1=1/float(m_FilterSize*m_FilterSize-1);

    for (size_t i=0; i<img2.Size(); i++) {
        pRes[i]=sqrt(M1*(pS2[i]-M*pM[i]*pM[i]));
    }

    return img2;
}

kipl::base::TImage<float,2> StdDevFilter::BoxFilter(kipl::base::TImage<float,2> img)
{
    std::vector<size_t> dimsU={m_FilterSize,1};
    std::vector<size_t> dimsV={1,m_FilterSize};
    size_t N=m_FilterSize*m_FilterSize;
    std::vector<float> fKernel(N,1.0f);

    kipl::filters::TFilter<float,2> filterU(fKernel,dimsU);
    kipl::filters::TFilter<float,2> filterV(fKernel,dimsV);

    kipl::base::TImage<float,2> imgU=filterU(img, m_eEdgeProcessingStyle);

    return filterV(imgU, m_eEdgeProcessingStyle);
}
}}

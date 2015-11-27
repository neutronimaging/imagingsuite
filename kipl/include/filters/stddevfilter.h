#ifndef STDDEVFILTER_H
#define STDDEVFILTER_H

#include "../base/timage.h"
#include "filterbase.h"
namespace kipl { namespace filters {
class StdDevFilter
{
public:
    enum VarianceType {
        StandardVariance,
        MADVariance
    };

    StdDevFilter();
    ~StdDevFilter();

    kipl::base::TImage<float,2> operator() (kipl::base::TImage<float,2> img);
protected:
    kipl::base::TImage<float,2> BoxFilter(kipl::base::TImage<float,2> img);

    size_t m_FilterSize;
    FilterBase::EdgeProcessingStyle m_eEdgeProcessingStyle;
};

}}

#endif // STDDEVFILTER_H

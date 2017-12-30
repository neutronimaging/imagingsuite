#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include "../../base/timage.h"
#include "../../base/roi.h"

namespace kipl {
namespace math {

template <typename T, size_t N>
Statistics imageStatistics(kipl::base::TImage<T,N> &img, kipl::base::RectROI &roi)
{
    Statistics stats;
    const size_t *imgDims=img.Dims();
    size_t roiDims[4];
    size_t last = (N==3) ? imgDims[2] : 1;

    roi.getBox(roiDims);

    for (size_t i=0; i<last; ++i)
    {
        for (size_t j=roiDims[1]; j<roiDims[3]; ++j)
        {
            T *pImg=img.GetLinePtr(j,i);
            for (size_t k=roiDims[0]; k<roiDims[2]; ++k) {
                stats.put(static_cast<float>(pImg[k]));
            }
        }
    }
    return stats;
}

}}
#endif // STATISTICS_HPP

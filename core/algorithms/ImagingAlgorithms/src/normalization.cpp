
#include "../include/normalization.h"
#include "../include/ImagingException.h"

namespace ImagingAlgorithms {

kipl::base::TImage<float,2> IMAGINGALGORITHMSSHARED_EXPORT normalize(const kipl::base::TImage<float,2> &img,
                                                                     const kipl::base::TImage<float,2> &ob,
                                                                     const kipl::base::TImage<float,2> &dc,
                                                                     const std::vector<size_t> &doseROI)
{
    if (img.Size() != ob.Size())
        throw ImagingException("size mismatch between sample and ob images",__FILE__,__LINE__);

    if (img.Size() != dc.Size())
        throw ImagingException("size mismatch between sample and ob images",__FILE__,__LINE__);

    float dose = 1.0f;

    kipl::base::TImage<float,2> normed(img.dims());

    auto pImg = img.GetDataPtr();
    auto pNorm = normed.GetDataPtr();
    auto pOB   = ob.GetDataPtr();
    auto pDC   = dc.GetDataPtr();

    for (size_t i = 0 ; i<img.Size() ; ++i)
        pNorm[i] = dose*(pImg[i]-pDC[i])/(pOB[i]-pDC[i]);

    return normed;
}

}

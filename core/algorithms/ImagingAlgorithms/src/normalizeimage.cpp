

#include "../include/normalizeimage.h"
#include "../include/ImagingException.h"

namespace ImagingAlgorithms
{

NormalizeImage::NormalizeImage(bool useLog) :
    logger("NormalizeImage"),
    bUseLog(useLog),
    bUseDose(false),
    referenceDose(1.0f)
{

}

void NormalizeImage::setReferences(kipl::base::TImage<float, 2> &ff,
                                   kipl::base::TImage<float, 2> &dc)
{
    if (ff.Size()!=dc.Size())
        throw ImagingException("Reference image size mismatch",__FILE__,__LINE__);

    if (ff.Size(0)!=dc.Size(0))
        throw ImagingException("Reference image size mismatch",__FILE__,__LINE__);

    if (ff.Size(1)!=dc.Size(1))
        throw ImagingException("Reference image size mismatch",__FILE__,__LINE__);

    mFlatField.Clone(ff);
    mDark.Clone(dc);

    mFlatField -= mDark;

    if (bUseDose)
    {
        referenceDose = computeDose(mFlatField);
        mFlatField /= referenceDose;
    }

    for (size_t i=0 ; i< mFlatField.Size(); ++i)
    {
        if (mFlatField[i]<=0.0f)
            mFlatField[i]=1.0f;

        if (bUseLog)
            mFlatField[i]=std::logf(mFlatField[i]);
    }

}

void NormalizeImage::setDoseROI(std::vector<size_t> &roi)
{
    if (roi.size()!=4UL)
        throw ImagingException("The dose correction vector must have four elements.",__FILE__,__LINE__);

    mROI     = roi;
    bUseDose = true;
}

void NormalizeImage::process(kipl::base::TImage<float, 2> &img)
{
    float dose=1.0f;

    if (bUseDose)
    {
        dose = computeDose(img);
        img /= dose;
    }

    img -= mDark;

    if (bUseLog)
    {
        for (size_t i = 0; i<img.Size(); ++i)
        {
            if (img[i]<=0)
                img[i] = 0.0f;

            img[i] = std::logf(img[i]) - mFlatField[i];
        }
    }
    else
    {
        for (size_t i = 0; i<img.Size(); ++i)
        {
            if (img[i]<=0)
                img[i] = 1.0f;

            img[i] /= mFlatField[i];
        }
    }

}

void NormalizeImage::process(kipl::base::TImage<float, 3> &img)
{
    normalizeSlices(img,0,img.Size(2));
}

void NormalizeImage::normalizeSlices(kipl::base::TImage<float, 3> &img, size_t begin, size_t end)
{
    kipl::base::TImage<float, 2> slice(img.dims());

    for (size_t i=begin; i<end; ++i)
    {
        std::copy_n(img.GetLinePtr(0,i),slice.Size(),slice.GetDataPtr());
        process(slice);
        std::copy_n(slice.GetDataPtr(),slice.Size(),img.GetLinePtr(0,i));
    }
}

float NormalizeImage::computeDose(kipl::base::TImage<float, 2> &img)
{
    return 1.0f;
}

}

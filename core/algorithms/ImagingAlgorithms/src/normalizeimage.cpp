

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

    auto ptr = mFlatField.GetDataPtr();

    mFlatField -= mDark;
    
    std::transform(ptr, ptr+mFlatField.Size(), ptr, [](float val) {
        return (val < 1.0f) ? 1.0f : val;
        });

    if (bUseDose)
    {
        referenceDose = computeDose(mFlatField);
        std::ostringstream msg;
        // msg << "Reference dose: " << referenceDose;
        // logger.message(msg.str());
        mFlatField /= referenceDose;
    }

    if (bUseLog)
    {
        std::transform(ptr, ptr+mFlatField.Size(), ptr, [](float val) {
            return std::log(val);
        });
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

    img -= mDark;

    auto ptr = img.GetDataPtr();
    std::transform(ptr, ptr+img.Size(), ptr, [](float val) {
        return (val < 1.0f) ? 1.0f : val;
        });

    if (bUseDose)
    {
        dose = computeDose(img);
        std::ostringstream msg;
        // msg << "projection dose: " << dose;
        // logger.message(msg.str());
        img /= dose;
    }

    if (bUseLog)
    {
        for (size_t i = 0; i<img.Size(); ++i)
        {
            img[i] = mFlatField[i] - std::log(img[i]);
        }
    }
    else
    {
        for (size_t i = 0; i<img.Size(); ++i)
        {
            img[i] /= mFlatField[i];
        }
    }

}

void NormalizeImage::process(kipl::base::TImage<float, 3> &img)
{
    normalizeSlices(img,0,img.Size(2));
}

bool NormalizeImage::usingLog()
{
    return bUseLog;
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

float NormalizeImage::computeDose(kipl::base::TImage<float, 2> & img)
{
    std::vector<size_t> doses;

    for (size_t i=mROI[1]; i<mROI[3]; ++i)
    {
        float dose=0.0f;
        for (size_t j=mROI[0]; j<mROI[2]; ++j)
        {
            dose += img(j,i);
        }
        doses.push_back(dose);
    }

    size_t n=doses.size();
    size_t mid = n / 2;
    std::nth_element(doses.begin(), doses.begin() + mid, doses.end());

    if (n % 2 == 0)
    {
        float median1 = doses[mid];
        std::nth_element(doses.begin(), doses.begin() + mid - 1, doses.end());
        float median2 = doses[mid - 1];
        return (median1 + median2) / 2.0f;
    }
    else
    {
        return doses[mid];
    }
}

}

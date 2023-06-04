//<LICENSE>

#include <ImagingException.h>
#include "ImageNormalization.h"

ImageNormalization::ImageNormalization() :
    logger("ImageNormalization"),
    useLog(true)
{

}

void ImageNormalization::setDarkCurrent(const kipl::base::TImage<float,2> &img)
{
    dc.Clone(img);
}

// void setDarkCurrent(const kipl::base::TImage<float,3> &img);

void ImageNormalization::setOpenBeam(const kipl::base::TImage<float,2> &img, float dose, bool subtractDC)
{
    ob.Clone(img);

    if (subtractDC)
        ob-=dc;

    obDose = dose;
}

// void setOpenBeam(const kipl::base::TImage<float,3> &img, const std::vector<float> &dose);

void ImageNormalization::setBBOpenBeam(const kipl::base::TImage<float,2> &img, float dose)
{
    bbob.Clone(img);

    bbobDose = dose;
}

void ImageNormalization::setBBSample(  const kipl::base::TImage<float,2> &img, float dose)
{
    bbs.Clone(img);

    bbsDose = dose;
}

// void setBBSample(  const kipl::base::TImage<float,3> &img, const std::vector<float> &dose);

void ImageNormalization::useLogarithm(bool x)
{
    useLog = x;
}

kipl::base::TImage<float,2> ImageNormalization::process(const kipl::base::TImage<float,2> &img, float dose, eNormalizationMethod normMethod, float tau)
{
    kipl::base::TImage<float> res=img-dc;
    
    switch (normMethod)
    {
        case basicNormalization:
            break;
        case scatterNormalization:
            break;
        case interpolateScatterNormalization:
        case one2oneScatterNormalization:
        default: 
            ImagingException("Normalization method not implemented or supported", __FILE__, __LINE__);
    }


}

kipl::base::TImage<float,3> ImageNormalization::process(const kipl::base::TImage<float,3> &img, const std::vector<float> &dose, eNormalizationMethod normMethod, float tau)
{

}


void ImageNormalization::checkImageDims(kipl::base::TImage<float,2> &imgA, kipl::base::TImage<float,2> &imgB)
{

}

void ImageNormalization::applyLogarithm(kipl::base::TImage<float,2> &img)
{
    for (size_t i=0UL; i<img.Size(); ++i )
    {
        if (0.0f<img[i])
            img[i] = log(img[i]);
        else
            img[i]=-3; // Transmission less than 0.001 is unrealistic 
    }
}

kipl::base::TImage<float,2> ImageNormalization::processBasicNormalization(const kipl::base::TImage<float,2> &img, float dose)
{

}
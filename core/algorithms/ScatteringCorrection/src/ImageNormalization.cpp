//<LICENSE>

#include <ImagingException.h>
#include "ImageNormalization.h"

ImageNormalization::ImageNormalization() :
    logger("ImageNormalization"),
    useLog(true)
{

}

void ImageNormalization::setDarkCurrent(const kipl::base::TImage<float,2> &img, float dose)
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

kipl::base::TImage<float,2> ImageNormalization::process(kipl::base::TImage<float,2> &img, float dose, eNormalizationMethod normMethod, float tau)
{
    if ( ! kipl::base::checkEqualSize(img,dc) )
        throw ImagingException("image - dc size missmatch",__FILE__,__LINE__);

    if ( ! kipl::base::checkEqualSize(img,ob) )
        throw ImagingException("image - ob size missmatch",__FILE__,__LINE__);

    kipl::base::TImage<float> res=img-dc;
    
    switch (normMethod)
    {
        case basicNormalization:
            processBasicNormalization(res,dose);
            break;
        case scatterNormalization:
            break;
        case interpolateScatterNormalization:
        case one2oneScatterNormalization:
        default: 
            ImagingException("Normalization method not implemented or supported", __FILE__, __LINE__);
    }

    if (useLog)
        applyLogarithm(res);

    return res;
}

void ImageNormalization::process(kipl::base::TImage<float,3> &img, const std::vector<float> &dose, eNormalizationMethod normMethod, float tau)
{
    if (dose.size() != img.Size(2))
        throw ImagingException("#images and #doses don't match",__FILE__,__LINE__);

    kipl::base::TImage<float,2> slice(img.dims());
    for (size_t i=0; i<img.Size(2); ++i)
    {
        std::copy_n(img.GetLinePtr(0,i),slice.Size(), slice.GetDataPtr());
        slice = process(slice,dose[i],normMethod,tau);
        std::copy_n(slice.GetDataPtr(),slice.Size(),img.GetLinePtr(0,i));
    }


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

void ImageNormalization::processBasicNormalization(kipl::base::TImage<float,2> &img, float dose)
{
    float doseFactor = obDose/dose;
    for (size_t i=0UL; i<img.Size(); ++i) 
    {
        img[i]=doseFactor*img[i]/ob[i];
    }
}

void ImageNormalization::processScatterNormalization(kipl::base::TImage<float,2> &img, float dose, float tau)
{
    float correctedSampleDose = (dose - (1.0f-1.0f/tau)*bbsDose)*tau;

    float correctedOBDose     = (obDose - (1.0f-1.0f/tau)*bbobDose)*tau;

    float doseFactor          = correctedOBDose / correctedSampleDose ;

    float doseFactorSample    = dose / correctedSampleDose ;

    float doseFactorOB        = obDose / correctedOBDose ; 

    for (size_t i=0UL; i<img.Size(); ++i) 
    {
        img[i]=doseFactor*(img[i]-bbs[i]*doseFactorSample)/(ob[i]-bbob[i]*doseFactorOB);
    }
}
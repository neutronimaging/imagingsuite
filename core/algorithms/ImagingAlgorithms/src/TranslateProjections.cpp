//<LICENSE>

#include "../include/TranslateProjections.h"
#include "../include/ImagingException.h"


namespace ImagingAlgorithms {
TranslateProjections::TranslateProjections()
{
}

void TranslateProjections::translate(kipl::base::TImage<float, 3> &img, float slope, float intercept, eTranslateSampling sampler, float parameter)
{
    int N=static_cast<int>(img.Size(2));
    kipl::base::TImage<float,2> proj(img.Dims());

    for (int i=0; i<N; i++) {
        memcpy(proj.GetDataPtr(),img.GetLinePtr(0,i), proj.Size()*sizeof(float));
        float shift=i*slope+intercept;

        switch (sampler) {
        case TranslateNearest  : translateNearestNeighbor(proj,shift); break;
        case TranslateLinear   : translateLinear(proj,shift); break;
        case TranslateGaussian : translateGaussian(proj,shift,parameter); break;
        }

        memcpy(img.GetLinePtr(0,i), proj.GetDataPtr(), proj.Size()*sizeof(float));
    }
}

void TranslateProjections::translateNearestNeighbor(kipl::base::TImage<float,2> &img, float shift)
{
    throw ImagingException("Nearest neighborhood sampling is not implemented",__FILE__,__LINE__);

}

void TranslateProjections::translateLinear(kipl::base::TImage<float,2> &img, float shift)
{
    if (shift==0.0f)
        return ;

    kipl::base::TImage<float,2> res(img.Dims());
    res=0.0f;
    int nShift      = floor(shift);
    int nStart      = 0;
    int nStop       = 0;
    int nShiftLo    = 0;
    int nShiftHi    = 0;
    float fWeightLo = 0.0;
    float fWeightHi = 0.0;

    if (0<=shift) {
        nShiftLo  = nShift;
        nShiftHi  = nShiftLo + 1;

        nStart    = 0 ;
        nStop     = img.Size(0) - nShiftHi ;

        fWeightHi = shift - nShiftLo;
        fWeightLo = 1.0f  - fWeightHi;
    }
    else {

        nShiftLo  = nShift;
        nShiftHi  = nShiftLo + 1;

        nStart    = -nShiftLo;
        nStop     = img.Size(0);

        fWeightHi = shift - nShiftLo;
        fWeightLo = 1.0f  - fWeightHi;
    }



    for (int j=0 ; j<static_cast<int>(img.Size(1)) ; j++) {
        float *pImg=img.GetLinePtr(j);
        float *pRes=res.GetLinePtr(j);

        for (int i=nStart; i<nStop; i++) {
            pRes[i]=pImg[i+nShiftLo]*fWeightLo+pImg[i+nShiftHi]*fWeightHi;
        }
    }

    img=res;
}

void TranslateProjections::translateGaussian(kipl::base::TImage<float,2> &img, float shift, float width)
{
    throw ImagingException("Gaussian sampling is not implemented",__FILE__,__LINE__);
}

}

std::string enum2string(ImagingAlgorithms::eTranslateSampling e)
{
    switch (e) {
        case ImagingAlgorithms::TranslateNearest  : return "TranslateNearest";
        case ImagingAlgorithms::TranslateLinear   : return "TranslateLinear";
        case ImagingAlgorithms::TranslateGaussian : return "TranslateGaussian";
        default : throw ImagingException("Unknown enum for eTranslateSampling",__FILE__, __LINE__);
    }

    return "UnknownTranslation";
}

void string2enum(std::string s, ImagingAlgorithms::eTranslateSampling &e)
{
    if (s=="TranslateNearest")  e=ImagingAlgorithms::TranslateNearest; else
        if (s=="TranslateLinear")   e=ImagingAlgorithms::TranslateLinear; else
            if (s=="TranslateGaussian") e=ImagingAlgorithms::TranslateGaussian; else
                throw ImagingException("Unknown translation sampling type string",__FILE__, __LINE__);

}

std::ostream & operator<<(std::ostream &s, ImagingAlgorithms::eTranslateSampling e)
{
    s<<enum2string(e);

    return s;
}



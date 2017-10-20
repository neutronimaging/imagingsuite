#ifndef ROBUSTLOGNORM_H_
#define ROBUSTLOGNORM_H_

#include "StdPreprocModules_global.h"

#include <logging/logger.h>
#include <base/timage.h>
#include <math/LUTCollection.h>

#include <ReferenceImageCorrection.h>

#include <PreprocModuleBase.h>
#include <ReconConfig.h>

#include "PreprocEnums.h"
#include "ImagingAlgorithms_global.h"
#include <averageimage.h>
#include <ReferenceImageCorrection.h>

//#include "../include/NormPlugins.h"

class STDPREPROCMODULESSHARED_EXPORT RobustLogNorm : public PreprocModuleBase
{
public:
    RobustLogNorm();
    virtual ~RobustLogNorm();

    virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters); /// Configure all parameters and calls PrepareBBData
    virtual std::map<std::string, std::string> GetParameters();
    virtual void LoadReferenceImages(size_t *roi); /// load all images that are needed for referencing in the current roi
    virtual bool SetROI(size_t *roi); /// set the current roi to be processed

    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    virtual void SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat); /// set references images
    virtual float GetInterpolationError(kipl::base::TImage<float,2> &mask); /// computes and returns interpolation error and mask on OB image with BBs
    virtual kipl::base::TImage<float, 2> GetMaskImage();
    virtual void PrepareBBData(); /// read all data (entire projection) that I need and prepare them for the BB correction, it is now called in LoadReferenceImages
    virtual void LoadExternalBBData(size_t *roi); /// load BB images pre-processed elsewhere

protected:
    ReconConfig m_Config;
    std::string path; /// path, maybe not used
    std::string flatname; /// name mask for OB image
    std::string darkname; /// name mask for DC image
    std::string blackbodyname; /// name mask for OB image with BBs
    std::string blackbodysamplename; /// name mask for sample image with BBs

    std::string blackbodyexternalname;
    std::string blackbodysampleexternalname;

    size_t nBBextCount; /// number of preprocessed BB images;
    size_t nBBextFirstIndex; /// first index in filneame for preprocessed BB images

    size_t nOBCount; /// number of OB images
    size_t nOBFirstIndex; /// first index in filename for OB images
    size_t nDCCount; /// number of DC images
    size_t nDCFirstIndex; /// first index in filename for DC images
    size_t nBBCount; /// number of open beam images with BB
    size_t nBBFirstIndex; /// first index in filename for OB images with BB
    size_t nBBSampleCount; /// number of sample images with BB
    size_t nBBSampleFirstIndex; /// first index in filename for sample images with BB

    float fFlatDose; /// dose value in open beam images in dose roi
    float fDarkDose; /// dose value in dark current images in dose roi
    float fBlackDose; /// dose value in black body images in BB dose roi
    float fBlackDoseSample; /// dose values in black body images with sample in BB dose roi
    float fdarkBBdose; /// dose value in dark current images within BB dose roi
    float fFlatBBdose; /// dose value in open beam image within BB dose roi

    float tau; /// mean pattern transmission, default 0.99

    bool bUseNormROI; /// boolean value on the use of the norm roi
    bool bUseLUT; /// boolean value on the use of LUT (not used)
    bool bUseWeightedMean;
    bool bUseBB; /// boolean value on the use of BBs, to be set when calling PrepareBBData
    bool bUseExternalBB; /// boolean value on the use of externally produced BBs
    bool bUseNormROIBB; /// boolean value on the use of the norm roi on BBs
    bool bSameMask; /// boolean value on the use of the same mask computed for OB images with BB also for sample image with BB, in case of false the mask is recomputed on the first available image with BB. This implies that exists an image with the sample and without BB and the sample is in the exact same position

    bool bPBvariante;

    size_t nNormRegion[4];
    size_t nOriginalNormRegion[4];
    size_t BBroi[4]; /// region of interest to be set for BB segmentation
    size_t doseBBroi[4]; /// region of interest for dose computation in BB images

    size_t radius; /// radius used to select circular region within the BBs to be used for interpolation
    size_t min_area; /// minimal area to be used for BB segmentation: if smaller, segmented BB is considered as noise and not considered for background computation
    float ferror; /// interpolation error, computed on the open beam with BBs image
    float ffirstAngle; /// first angle for BB sample image, used for BB interpolation option
    float flastAngle; /// last angle for BB sample image, used for BB interpolation option

    float *ob_bb_param;
    float *sample_bb_param;

    int GetnProjwithAngle(float angle); /// compute the index of projection data at a given angle


    kipl::base::TImage<float,2> mMaskBB;
    kipl::base::TImage<float,2> mdark;
    kipl::base::TImage<float,2 > mflat;

    virtual kipl::base::TImage<float,2> ReferenceLoader(std::string fname,
                                                        int firstIndex,
                                                        int N,
                                                        size_t *roi,
                                                        float initialDose,
                                                        float doseBias,
                                                        ReconConfig &config, float &dose); /// Loader function and dose computation for standard reference images (OB and DC)


    virtual kipl::base::TImage<float,2> BBLoader(std::string fname,
                                                        int firstIndex,
                                                        int N,
                                                        float initialDose,
                                                        float doseBias,
                                                        ReconConfig &config, float &dose); /// Loader function and dose computation for BB reference images (OB with BB and sample with BB)

    virtual float DoseBBLoader(std::string fname,
                                 int firstIndex,
                                 float initialDose,
                                 float doseBias,
                                 ReconConfig &config); /// Loader function that only compute Dose in the BB dose roi , whitout loading the entire image

    virtual kipl::base::TImage<float,2> BBExternalLoader(std::string fname,
                                                         ReconConfig &config,
                                                         size_t *roi,
                                                         float &dose); /// Loader function for externally created BB, open beam case (only 1 image)
    virtual kipl::base::TImage<float,3> BBExternalLoader(std::string fname,
                                                         int N,
                                                         size_t *roi,
                                                       int firstIndex,
                                                       ReconConfig &config, float *doselist); /// Loader function for externally created BB, sample image case (nProj images with filemask)


    float computedose(kipl::base::TImage<float,2>&img);

    void PreparePolynomialInterpolationParameters(); /// get the interpolation parameters for the polynomial case
    int PrepareSplinesInterpolationParameters(); /// get the interpolation parameters for the thin plates spline case

private:
    int m_nWindow; /// apparentely not used
    ImagingAlgorithms::AverageImage::eAverageMethod m_ReferenceAverageMethod; /// method chosen for averaging Referencing images
    ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod m_ReferenceMethod;/// method chosen for Referencing (BBLogNorm or LogNorm, only BBLogNorm is implemented at the moment)
    ImagingAlgorithms::ReferenceImageCorrection::eBBOptions m_BBOptions; /// options for BB image reference correction (Interpolate, Average, OneToOne)
    ImagingAlgorithms::ReferenceImageCorrection m_corrector; /// instance of ReferenceImageCorrection
    ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX m_xInterpOrder; /// order chosen for interpolation along the X direction
    ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY m_yInterpOrder; /// order chosen for interpolation along the Y direction
    ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod m_InterpMethod; /// interpolation method

};



#endif // ROBUSTLOGNORM_H

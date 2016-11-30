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

    virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
    virtual void LoadReferenceImages(size_t *roi); /// load all images that are needed for referencing in the current roi
    virtual bool SetROI(size_t *roi); /// set the current roi to be processed

    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    virtual void SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat); /// set references images

protected:
    ReconConfig m_Config;
    std::string path;
    std::string flatname;
    std::string darkname;
    std::string blackbodyname;
    std::string blackbodysamplename;

    size_t nOBCount;
    size_t nOBFirstIndex;
    size_t nDCCount;
    size_t nDCFirstIndex;
    size_t nBBCount;
    size_t nBBFirstIndex;
    size_t nBBSampleCount;
    size_t nBBSampleFirstIndex;

    float fFlatDose; /// dose value in open beam images
    float fDarkDose; /// dose value in dark current images
    float fBlackDose; /// dose value in black body images

    float tau; /// mean pattern transmission, default 0.99

    bool bUseNormROI; /// boolean value on the use of the norm roi
    bool bUseLUT; /// boolean value on the use of LUT (not used)
    bool bUseWeightedMean;
    bool bUseBB; /// boolean value on the use of BBs, to be set when calling PrepareBBData

    size_t nNormRegion[4];
    size_t nOriginalNormRegion[4];
    size_t BBroi[4]; /// region of interest to be set for BB segmentation

    size_t radius; /// radius used to select circular region within the BBs to be used for interpolation

    //not sure i use all those things
    kipl::base::TImage<float,2> mFlatField;
    kipl::base::TImage<float,2> mDark;
    kipl::base::TImage<float,2> mBlack;

    kipl::base::TImage<float,2> mMaskBB;

    void PrepareBBData(); /// read all data (entire projection) that I need and prepare them for the BB correction, it is called in Configure

private:
    int m_nWindow;
    ImagingAlgorithms::AverageImage::eAverageMethod m_ReferenceAverageMethod;
    ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod m_ReferenceMethod;
    ImagingAlgorithms::ReferenceImageCorrection m_corrector;

};

#endif // ROBUSTLOGNORM_H

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
    virtual void LoadReferenceImages(size_t *roi);
    virtual bool SetROI(size_t *roi);

    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    virtual void SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat);

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

    float fFlatDose;
    float fDarkDose;
    float fBlackDose;

    bool bUseNormROI;
    bool bUseLUT;
    bool bUseWeightedMean;
    bool bUseBB; // when there are BB images

    size_t nNormRegion[4];
    size_t nOriginalNormRegion[4];

    //not sure i use all those things
    kipl::base::TImage<float,2> mFlatField;
    kipl::base::TImage<float,2> mDark;
    kipl::base::TImage<float,2> mBlack;

private:
    int m_nWindow;
    ImagingAlgorithms::AverageImage::eAverageMethod m_ReferenceAverageMethod;
    ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod m_ReferenceMethod;
    ImagingAlgorithms::ReferenceImageCorrection m_corrector;

};

#endif // ROBUSTLOGNORM_H

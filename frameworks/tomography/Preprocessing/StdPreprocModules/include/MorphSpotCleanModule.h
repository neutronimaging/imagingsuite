//<LICENSE>

#ifndef MORPHSPOTCLEANMODULE_H
#define MORPHSPOTCLEANMODULE_H

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <MorphSpotClean.h>
#include <morphology/morphology.h>

class STDPREPROCMODULESSHARED_EXPORT MorphSpotCleanModule: public PreprocModuleBase {
public:
    MorphSpotCleanModule(kipl::interactors::InteractionBase *interactor=nullptr);
    virtual ~MorphSpotCleanModule();

    virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
    virtual bool SetROI(const std::vector<size_t> &roi);

    pair<kipl::base::TImage<float,2>,kipl::base::TImage<float,2>> DetectionImage(kipl::base::TImage<float,2> img, ImagingAlgorithms::eMorphDetectionMethod dm, bool removeBias);

protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
    int ProcessSingle(kipl::base::TImage<float,3> & img);
    int ProcessParallel(kipl::base::TImage<float,3> & img);
    int ProcessParallelStd(kipl::base::TImage<float,3> & img);
    int ProcessParallelStdBlock(size_t tid, kipl::base::TImage<float, 3> *img, size_t firstSlice, size_t N);
    kipl::base::eConnectivity m_eConnectivity;
    ImagingAlgorithms::eMorphDetectionMethod m_eDetectionMethod;
    ImagingAlgorithms::eMorphCleanMethod m_eCleanMethod;
    std::vector<float> m_fThreshold;
    std::vector<float> m_fSigma;
    bool m_bThresholdByFraction;
    int m_nEdgeSmoothLength;
    int m_nMaxArea;
    bool m_bRemoveInfNaN;
    bool m_bClampData;
    float m_fMinLevel;
    float m_fMaxLevel;
    bool m_bTranspose;
};

#endif // MORPHSPOTCLEANMODULE_H

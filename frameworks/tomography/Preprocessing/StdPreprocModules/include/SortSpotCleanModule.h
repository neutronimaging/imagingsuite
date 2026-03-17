//<LICENSE>

#ifndef SORTSPOTCLEANMODULE_H
#define SORTSPOTCLEANMODULE_H

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <sortspotclean.h>
#include <morphology/morphology.h>

class STDPREPROCMODULESSHARED_EXPORT SortSpotCleanModule: public PreprocModuleBase {
public:
    SortSpotCleanModule(kipl::interactors::InteractionBase *interactor=nullptr);
    virtual ~SortSpotCleanModule();

    virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
    virtual bool SetROI(const std::vector<size_t> &roi);

    // pair<kipl::base::TImage<float,2>,kipl::base::TImage<float,2>> DetectionImage(kipl::base::TImage<float,2> img, ImagingAlgorithms::eMorphDetectionMethod dm, bool removeBias);

protected:
    using ProcessModuleBase::Configure;
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);

    ImagingAlgorithms::SortSpotClean m_cleaner;

    float  m_fThreshold;
    float  m_fQuantile;
    size_t m_nPatchSize;
    ImagingAlgorithms::eSortSpotQuantile m_eQuantileMethod;
    kipl::base::eConnectivity m_eConnectivity;
    bool   m_bRemoveInfNaN;
    bool   m_bClampData;
    float  m_fMinLevel;
    float  m_fMaxLevel;
};

#endif // SortSpotCleanModule_H

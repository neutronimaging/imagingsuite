//<LICENSE>

#ifndef GAMMASPOTCLEANMODULE_H
#define GAMMASPOTCLEANMODULE_H

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <gammaclean.h>
#include <morphology/morphology.h>

class STDPREPROCMODULESSHARED_EXPORT GammaSpotCleanModule: public PreprocModuleBase {
public:
    using PreprocModuleBase::Configure;
    GammaSpotCleanModule();
    ~GammaSpotCleanModule();

    int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
    std::map<std::string, std::string> GetParameters() override;
    bool SetROI(const std::vector<size_t> &roi) override;

    kipl::base::TImage<float,2> DetectionImage(kipl::base::TImage<float,2> img);

protected:
    using PreprocModuleBase::ProcessCore;
    int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) override;
    int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
    int ProcessSingle(kipl::base::TImage<float,3> & img);
    int ProcessParallel(kipl::base::TImage<float,3> & img);

    float m_fSigma;
    float m_fThreshold3;
    float m_fThreshold5;
    float m_fThreshold7;
    size_t m_nMedianSize;
};

#endif // GAMMASPOTCLEANMODULE_H

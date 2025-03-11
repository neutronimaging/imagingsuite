#ifndef CAMERASTRIPECLEAN_H
#define CAMERASTRIPECLEAN_H

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>

class STDPREPROCMODULESSHARED_EXPORT CameraStripeClean : public PreprocModuleBase
{
public:
    CameraStripeClean();
    virtual ~CameraStripeClean();

    using PreprocModuleBase::Configure;
    int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
    using PreprocModuleBase::GetParameters;
    std::map<std::string, std::string> GetParameters() override;
    using PreprocModuleBase::SetROI;
    bool SetROI(const std::vector<size_t> &roi) override;
protected:
    using PreprocModuleBase::ProcessCore;
    int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
    int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) override;

    float m_fThreshold;
};

#endif // CAMERASTRIPECLEAN_H

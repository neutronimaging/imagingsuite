#ifndef TRANSLATEPROJECTIONSMODULE_H
#define TRANSLATEPROJECTIONSMODULE_H

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <ReconConfig.h>
#include <TranslateProjections.h>


class STDPREPROCMODULESSHARED_EXPORT TranslateProjectionsModule: public PreprocModuleBase
{
public:
    using PreprocModuleBase::Configure;
    TranslateProjectionsModule();
    ~TranslateProjectionsModule();

    int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
    std::map<std::string, std::string> GetParameters() override;
    bool SetROI(const std::vector<size_t> &roi) override;
protected:
    using PreprocModuleBase::ProcessCore;
    int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) override;

    float m_fSlope;
    float m_fIntercept;
    float m_fWidth;
    ImagingAlgorithms::eTranslateSampling m_eSampler;

};

#endif // TRANSLATEPROJECTIONS_H

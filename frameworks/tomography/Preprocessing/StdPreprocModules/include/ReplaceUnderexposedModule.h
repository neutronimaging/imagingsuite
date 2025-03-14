//<LICENSE>

#ifndef ReplaceUnderexposedMODULE_H
#define ReplaceUnderexposedMODULE_H

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <ReplaceUnderexposed.h>
#include <interactors/interactionbase.h>

class ReplaceUnderexposedModule : public PreprocModuleBase
{
public:
    using PreprocModuleBase::Configure;
    ReplaceUnderexposedModule(kipl::interactors::InteractionBase *interactor=nullptr);

    int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
    std::map<std::string, std::string> GetParameters() override;
    bool SetROI(const std::vector<size_t> &roi) override;
protected:
    using PreprocModuleBase::ProcessCore;
    int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
    int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) override;

    ImagingAlgorithms::ReplaceUnderexposed psc;
    float threshold;
};

#endif // ReplaceUnderexposedMODULE_H

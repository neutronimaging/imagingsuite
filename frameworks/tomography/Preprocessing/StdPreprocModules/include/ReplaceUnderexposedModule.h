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
    ReplaceUnderexposedModule(kipl::interactors::InteractionBase *interactor=nullptr);

    virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
    virtual bool SetROI(const std::vector<size_t> &roi);
protected:
    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

    ImagingAlgorithms::ReplaceUnderexposed psc;
    float threshold;
};

#endif // ReplaceUnderexposedMODULE_H

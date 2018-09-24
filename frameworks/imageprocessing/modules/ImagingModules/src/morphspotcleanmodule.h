#ifndef MORPHSPOTCLEANMODULE_H
#define MORPHSPOTCLEANMODULE_H

#include "ImagingModules_global.h"
#include <string>
#include <map>

#include <base/timage.h>
#include <base/kiplenums.h>

#include <KiplProcessModuleBase.h>

class IMAGINGMODULESSHARED_EXPORT MorphSpotCleanModule : public KiplProcessModuleBase
{
public:
    MorphSpotCleanModule();
    virtual ~MorphSpotCleanModule();

    virtual int Configure(std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
};

#endif // MORPHSPOTCLEANMODULE_H

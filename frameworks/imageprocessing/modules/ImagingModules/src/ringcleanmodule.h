//<LICENSE>
#ifndef RINGCLEANMODULE_H
#define RINGCLEANMODULE_H

#include "ImagingModules_global.h"
#include <string>
#include <map>

#include <base/timage.h>
#include <base/kiplenums.h>

#include <KiplProcessModuleBase.h>
#include <KiplProcessConfig.h>

#include <StripeFilter.h>

class IMAGINGMODULESSHARED_EXPORT RingCleanModule: public KiplProcessModuleBase {
public:
    RingCleanModule();
    virtual ~RingCleanModule();

    virtual int Configure(KiplProcessConfig config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    ImagingAlgorithms::StripeFilter *m_StripeFilter;
    KiplProcessConfig m_Config;
    std::string wname;
    size_t scale;
    float sigma;
    ImagingAlgorithms::eStripeFilterOperation op;
    kipl::base::eImagePlanes plane;
};
#endif // WAVELETRINGCLEANMODULE_H

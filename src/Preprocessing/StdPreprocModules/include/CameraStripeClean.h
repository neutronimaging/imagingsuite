#ifndef CAMERASTRIPECLEAN_H
#define CAMERASTRIPECLEAN_H

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>

class STDPREPROCMODULESSHARED_EXPORT CameraStripeClean : public PreprocModuleBase
{
public:
    CameraStripeClean();
    virtual ~CameraStripeClean();

    virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
    virtual bool SetROI(size_t *roi);
protected:
    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

    float m_fThreshold;
};

#endif // CAMERASTRIPECLEAN_H

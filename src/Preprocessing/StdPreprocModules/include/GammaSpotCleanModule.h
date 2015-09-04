#ifndef GAMMASPOTCLEANMODULE_H
#define GAMMASPOTCLEANMODULE_H

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <gammaclean.h>
#include <morphology/morphology.h>

class STDPREPROCMODULESSHARED_EXPORT GammaSpotCleanModule: public PreprocModuleBase {
public:
    GammaSpotCleanModule();
    virtual ~GammaSpotCleanModule();

    virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
    virtual bool SetROI(size_t *roi);

    kipl::base::TImage<float,2> DetectionImage(kipl::base::TImage<float,2> img);

protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
    int ProcessSingle(kipl::base::TImage<float,3> & img);
    int ProcessParallel(kipl::base::TImage<float,3> & img);

    float m_fSigma;
    float m_fThreshold3;
    float m_fThreshold5;
    float m_fThreshold7;
    size_t m_nMedianSize;
};

#endif // GAMMASPOTCLEANMODULE_H

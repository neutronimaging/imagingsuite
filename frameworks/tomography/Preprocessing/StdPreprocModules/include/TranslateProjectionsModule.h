#ifndef TRANSLATEPROJECTIONSMODULE_H
#define TRANSLATEPROJECTIONSMODULE_H

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <ReconConfig.h>
#include <TranslateProjections.h>


class STDPREPROCMODULESSHARED_EXPORT TranslateProjectionsModule: public PreprocModuleBase
{
public:
    TranslateProjectionsModule();
    virtual ~TranslateProjectionsModule();

    virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
    virtual bool SetROI(size_t *roi);
protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

    float m_fSlope;
    float m_fIntercept;
    float m_fWidth;
    ImagingAlgorithms::eTranslateSampling m_eSampler;

};

#endif // TRANSLATEPROJECTIONS_H

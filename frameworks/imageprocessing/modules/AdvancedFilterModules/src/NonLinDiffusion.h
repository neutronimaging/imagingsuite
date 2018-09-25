//<LICENSE>

#ifndef NONLINDIFFUSION_H
#define NONLINDIFFUSION_H

#include "AdvancedFilterModules_global.h"
#include <KiplProcessModuleBase.h>
#include <scalespace/filterenums.h>
#include <KiplProcessConfig.h>

class ADVANCEDFILTERMODULES_EXPORT NonLinDiffusionModule: public KiplProcessModuleBase {
public:
    NonLinDiffusionModule();
    virtual ~NonLinDiffusionModule();

    virtual int Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

    void ScaleImage(kipl::base::TImage<float,3> & img, bool forward);

    bool m_bAutoScale;
    float m_fSlope;
    float m_fIntercept;

    float m_fTau;
    float m_fSigma;
    float m_fLambda;

    int m_nIterations;
    std::string m_sIterationPath;
    bool m_bSaveIterations;
};
#endif // NONLINDIFFUSION_H

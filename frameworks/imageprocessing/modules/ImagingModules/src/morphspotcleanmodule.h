#ifndef MORPHSPOTCLEANMODULE_H
#define MORPHSPOTCLEANMODULE_H

#include "ImagingModules_global.h"
#include <string>
#include <map>

#include <base/timage.h>
#include <base/kiplenums.h>

#include <KiplProcessModuleBase.h>
#include <MorphSpotClean.h>
#include <morphology/morphology.h>

class IMAGINGMODULESSHARED_EXPORT MorphSpotCleanModule : public KiplProcessModuleBase
{
public:
    MorphSpotCleanModule(kipl::interactors::InteractionBase *interactor);
    virtual ~MorphSpotCleanModule();

    virtual int Configure(std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

    kipl::morphology::MorphConnect m_eConnectivity;
    ImagingAlgorithms::eMorphDetectionMethod m_eDetectionMethod;
    ImagingAlgorithms::eMorphCleanMethod m_eCleanMethod;
    double m_fThreshold;
    double m_fSigma;
    int m_nEdgeSmoothLength;
    int m_nMaxArea;
    double m_fMinLevel;
    double m_fMaxLevel;
    bool m_bThreading;
};

#endif // MORPHSPOTCLEANMODULE_H

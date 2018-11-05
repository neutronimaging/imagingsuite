#ifndef MORPHSPOTCLEANMODULE_H
#define MORPHSPOTCLEANMODULE_H

#include "ImagingModules_global.h"
#include <string>
#include <map>

#include <base/timage.h>
#include <base/kiplenums.h>

#include <KiplProcessModuleBase.h>
#include <KiplProcessConfig.h>
#include <MorphSpotClean.h>
#include <morphology/morphology.h>

class IMAGINGMODULESSHARED_EXPORT MorphSpotCleanModule : public KiplProcessModuleBase
{
public:
    MorphSpotCleanModule(kipl::interactors::InteractionBase *interactor=nullptr);
    virtual ~MorphSpotCleanModule();

    virtual int Configure(KiplProcessConfig config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
    kipl::base::TImage<float,2> DetectionImage(kipl::base::TImage<float,2> img);

protected:
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

//    int ProcessSingle(kipl::base::TImage<float,3> & img);
//    int ProcessParallel(kipl::base::TImage<float,3> & img);
//    int ProcessParallelStd(kipl::base::TImage<float,3> & img);
//    int ProcessParallelStd(size_t tid, float *pImg, const size_t *dims, size_t N);

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

private:
    bool updateStatus(float val, std::string msg);

};

#endif // MORPHSPOTCLEANMODULE_H

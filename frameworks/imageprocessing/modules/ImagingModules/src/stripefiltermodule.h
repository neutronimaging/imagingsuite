#ifndef STRIPEFILTERMODULE_H
#define STRIPEFILTERMODULE_H

#include "ImagingModules_global.h"
#include <string>
#include <map>

#include <base/timage.h>
#include <base/kiplenums.h>
#include <fft/fftbase.h>

#include <KiplProcessModuleBase.h>
#include <KiplProcessConfig.h>

#include <StripeFilter.h>

class IMAGINGMODULESSHARED_EXPORT StripeFilterModule: public KiplProcessModuleBase {
public:
    StripeFilterModule(kipl::interactors::InteractionBase *interactor=nullptr);
    virtual ~StripeFilterModule();

    virtual int Configure(KiplProcessConfig config, std::map<std::string, std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

protected:
    KiplProcessConfig m_Config;

//    int ProcessSingle(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
//	int ProcessParallel(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

    ImagingAlgorithms::StripeFilter *m_StripeFilter;

    std::string m_sWaveletName;
//    size_t scale;
    float m_fSigma;
    size_t m_nLevels;
    bool m_bParallelProcessing;
    ImagingAlgorithms::StripeFilterOperation op;
    kipl::base::eImagePlanes plane;

private:
    bool updateStatus(float val, std::string msg);
};
#endif // WAVELETRINGCLEANMODULE_H

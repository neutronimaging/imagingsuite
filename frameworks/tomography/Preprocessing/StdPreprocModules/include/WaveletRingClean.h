//<LICENSE>

#ifndef WAVELETRINGCLEAN_H_
#define WAVELETRINGCLEAN_H_

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <ReconConfig.h>
#include <fft/fftbase.h>
#include <StripeFilter.h>

class STDPREPROCMODULESSHARED_EXPORT WaveletRingClean: public PreprocModuleBase {
public:
    WaveletRingClean(kipl::interactors::InteractionBase *interactor=nullptr);
	virtual ~WaveletRingClean();

	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
        virtual bool SetROI(const std::vector<size_t> &roi);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	int ProcessSingle(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
	int ProcessParallel(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
        int ProcessParallelStd(kipl::base::TImage<float,3> & img);
        int ProcessParallelStdBlock(size_t tid, kipl::base::TImage<float, 3> *img, size_t firstSlice, size_t N);

	std::string m_sWName;
	float m_fSigma;
	size_t m_nDecNum;
    ImagingAlgorithms::eStripeFilterOperation m_eCleanMethod;
};

#endif /* WAVELETRINGCLEAN_H_ */

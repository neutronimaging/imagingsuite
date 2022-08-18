//<LICENSE>

#ifndef PREPROCMODULE_H_
#define PREPROCMODULE_H_

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>

class STDPREPROCMODULESSHARED_EXPORT VoStripeCleanModule: public PreprocModuleBase {
public:
    VoStripeCleanModule(kipl::interactors::InteractionBase *interactor=nullptr);
    virtual ~VoStripeCleanModule();
	
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
	virtual bool SetROI(size_t *roi);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    int processSequential(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    int processParallel(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);


    bool   useUnresponsiveStripes;
    bool   useStripeSorting;
    bool   useLargeStripes;

    size_t filterSize_unresponsive;
    size_t filterSize_sorting;
    size_t filterSize_large;
    float  snr_unresponsive;
    float  snr_large;

};

#endif /* PREPROCMODULE_H_ */

//<LICENSE>

#ifndef DETECTORLAGMODULE_H_
#define DETECTORLAGMODULE_H_

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>
#include <detectorlag.h>

class STDPREPROCMODULESSHARED_EXPORT DetectorLagModule: public PreprocModuleBase {
public:
    DetectorLagModule();
    virtual ~DetectorLagModule();
	
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
	virtual bool SetROI(size_t *roi);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float fOffset; // Example parameters
	float fSlope;

	ImagingAlgorithms::DetectorLag dl_correction;
};

#endif // DETECTORLAGMODULE_H_ 

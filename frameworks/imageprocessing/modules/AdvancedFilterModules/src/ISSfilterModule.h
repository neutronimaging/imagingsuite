//<LICENSE>

#ifndef ISSFILTERMODULE_H_
#define ISSFILTERMODULE_H_

#include "AdvancedFilterModules_global.h"
#include <KiplProcessModuleBase.h>
#include <scalespace/filterenums.h>
#include <KiplProcessConfig.h>

class ADVANCEDFILTERMODULES_EXPORT ISSfilterModule: public KiplProcessModuleBase {
public:
    ISSfilterModule();
    virtual ~ISSfilterModule();
	
    virtual int Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	void ScaleImage(kipl::base::TImage<float,3> & img, bool forward);

	bool m_bAutoScale;
	float m_fSlope;
	float m_fIntercept;

	float m_fTau;
	float m_fLambda;
	float m_fAlpha;
	int m_nIterations;
	std::string m_sIterationPath;
	bool m_bSaveIterations;
    akipl::scalespace::eRegularizationType m_eRegularization;
    akipl::scalespace::eInitialImageType m_eInitialImage;
};

#endif /* DATASCALER_H_ */

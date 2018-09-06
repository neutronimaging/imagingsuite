//<LICENSE>

#ifndef DOSECORRECTION_H_
#define DOSECORRECTION_H_

#include "BaseModules_global.h"
#include <KiplProcessModuleBase.h>
#include <KiplProcessConfig.h>

class BASEMODULES_EXPORT DoseCorrection: public KiplProcessModuleBase {
public:
	DoseCorrection();
	virtual ~DoseCorrection();
	
    virtual int Configure(KiplProcessConfig m_Config,std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float m_fSlope;
	float m_fIntercept;

	int m_nROI[4];
};

#endif /* DATASCALER_H_ */

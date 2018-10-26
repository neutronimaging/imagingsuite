//<LICENSE>

#ifndef CLAMPDATA_H_
#define CLAMPDATA_H_

#include "BaseModules_global.h"
#include <KiplProcessModuleBase.h>
#include <KiplProcessConfig.h>

class BASEMODULES_EXPORT ClampData: public KiplProcessModuleBase {
public:
	ClampData();
	virtual ~ClampData();
	
    virtual int Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float m_fMin;
	float m_fMax;
};

#endif /* DATASCALER_H_ */

// LICENSE 

#ifndef ISSFILTER_H_
#define ISSFILTER_H_

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>

class STDPREPROCMODULESSHARED_EXPORT ISSfilter: public PreprocModuleBase {
public:
	ISSfilter();
	virtual ~ISSfilter();

	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
	std::map<std::string, std::string> GetParameters() override;
	bool SetROI(const std::vector<size_t> &roi) override;
	void ErrorCurve(bool bErrorCurve) {m_bErrorCurve=bErrorCurve;}
    float const * ErrorCurve() {return m_fErrorCurve;}
protected:
	int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
	int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) override;

	int ScaleData(kipl::base::TImage<float,2> &img);
	int RescaleData(kipl::base::TImage<float,2> &img);
	float m_fTau;
	int m_nN;
	float m_fLambda;
	float m_fAlpha;

	bool m_bAutoScale;
	bool m_bScaleData;
	float m_fSlope;
	float m_fIntercept;


	bool m_bErrorCurve;
	float *m_fErrorCurve;

};

#endif /* ISSFILTER_H_ */

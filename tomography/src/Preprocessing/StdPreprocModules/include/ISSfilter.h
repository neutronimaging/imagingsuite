/*
 * ISSfilter.h
 *
 *  Created on: Aug 5, 2011
 *      Author: kaestner
 */

#ifndef ISSFILTER_H_
#define ISSFILTER_H_

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>

class STDPREPROCMODULESSHARED_EXPORT ISSfilter: public PreprocModuleBase {
public:
	ISSfilter();
	virtual ~ISSfilter();

	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
	virtual bool SetROI(size_t *roi);
	void ErrorCurve(bool bErrorCurve) {m_bErrorCurve=bErrorCurve;}
    float const * ErrorCurve() {return m_fErrorCurve;}
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

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

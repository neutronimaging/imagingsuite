/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef SCALEDATA_H_
#define SCALEDATA_H_

#include <KiplProcessModuleBase.h>

class ScaleData: public KiplProcessModuleBase {
public:
	ScaleData();
	virtual ~ScaleData();
	
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float m_fSlope;
	float m_fIntercept;

	bool m_bAutoScale;
};

#endif /* DATASCALER_H_ */

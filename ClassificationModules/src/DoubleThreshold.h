/*
 * DoubleThreshold.h
 *
 *  Created on: Nov 11, 2012
 *      Author: anders
 */

#ifndef DOUBLETHRESHOLD_H_
#define DOUBLETHRESHOLD_H_

#include <KiplProcessModuleBase.h>
#include <segmentation/thresholds.h>

class DoubleThreshold: public KiplProcessModuleBase {
public:
	DoubleThreshold();
	virtual ~DoubleThreshold();

	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();

protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float m_fHighThreshold;
	float m_fLowThreshold;

	float m_fBackgroundValue;
	bool  m_bUseBackgroundValue;
};

#endif /* DOUBLETHRESHOLD_H_ */

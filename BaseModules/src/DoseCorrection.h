/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef DOSECORRECTION_H_
#define DOSECORRECTION_H_

#include <KiplProcessModuleBase.h>

class DoseCorrection: public KiplProcessModuleBase {
public:
	DoseCorrection();
	virtual ~DoseCorrection();
	
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float m_fSlope;
	float m_fIntercept;

	int m_nROI[4];
};

#endif /* DATASCALER_H_ */

/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef BASICTHRESHOLD_H_
#define BASICTHRESHOLD_H_

#include <KiplProcessModuleBase.h>

class BasicThreshold: public KiplProcessModuleBase {
public:
	BasicThreshold();
	virtual ~BasicThreshold();
	
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float m_fThreshold;

};

#endif /* DATASCALER_H_ */

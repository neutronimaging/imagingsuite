/*
 * MaskedHistogram.h
 *
 *  Created on: Jan 7, 2013
 *      Author: kaestner
 */

#ifndef DISTANCESTATS_H_
#define DISTANCESTATS_H_

#include <KiplProcessModuleBase.h>

class DistanceStatistics: public KiplProcessModuleBase {
public:
	DistanceStatistics();
	virtual ~DistanceStatistics();

	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	float m_fMaskValue;
	std::string m_sFileName;
};

#endif /* MASKEDHISTOGRAM_H_ */

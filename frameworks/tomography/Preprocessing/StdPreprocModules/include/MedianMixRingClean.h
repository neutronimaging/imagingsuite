/*
 * MedianMixRingClean.h
 *
 *  Created on: Aug 4, 2011
 *      Author: kaestner
 */

#ifndef MEDIANMIXRINGCLEAN_H_
#define MEDIANMIXRINGCLEAN_H_

#include "StdPreprocModules_global.h"
#include <PreprocModuleBase.h>

class STDPREPROCMODULESSHARED_EXPORT MedianMixRingClean: public PreprocModuleBase {
public:
	MedianMixRingClean();
	virtual ~MedianMixRingClean();

	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
	virtual bool SetROI(size_t *roi);
	void ProcessSinogram(kipl::base::TImage<float,2> &img, float *profile);
	kipl::base::TImage<float,2> profile;
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
	int ComputeProfile(kipl::base::TImage<float,3> & img);


	kipl::base::TImage<float,2> weights;
	float m_fSigma;
	float m_fLambda;

};

#endif /* MEDIANMIXRINGCLEAN_H_ */

/*
 * RemoveBackground.h
 *
 *  Created on: Nov 11, 2012
 *      Author: anders
 */

#ifndef REMOVEBACKGROUND_H_
#define REMOVEBACKGROUND_H_

#include "ClassificationModules_global.h"
#include <KiplProcessModuleBase.h>
#include <segmentation/thresholds.h>
#include <KiplProcessConfig.h>

class CLASSIFICATIONMODULES_EXPORT RemoveBackground: public KiplProcessModuleBase {
public:
	RemoveBackground();
	virtual ~RemoveBackground();

    virtual int Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
	int Threshold(float * data,
			char *result,
			size_t N,
			float th,
			kipl::segmentation::CmpType cmp=kipl::segmentation::cmp_greater);
	float m_fThreshold;
	bool  m_bDoMasking;
	float m_fPruneRadius;
	float m_fMaskValue;
};

#endif /* REMOVEBACKGROUND_H_ */

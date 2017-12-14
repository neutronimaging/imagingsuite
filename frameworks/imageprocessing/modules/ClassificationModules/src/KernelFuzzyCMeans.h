/*
 * KernelFuzzyCMeans.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef KernelFuzzyCMeans_H_
#define KernelFuzzyCMeans_H_

#include "ClassificationModules_global.h"
#include <KiplProcessModuleBase.h>

class CLASSIFICATIONMODULES_EXPORT KernelFuzzyCMeans: public KiplProcessModuleBase {
public:
	KernelFuzzyCMeans();
	virtual ~KernelFuzzyCMeans();
	
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	int m_nNClasses;
	float m_fFuzziness;
	float m_fSigma;
	int m_nMaxIt;
	bool m_bParallel;
};

#endif // KernelFuzzyCMeans_H_ 

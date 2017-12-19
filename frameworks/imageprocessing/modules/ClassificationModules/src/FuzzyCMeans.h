/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef FuzzyCMeans_H_
#define FuzzyCMeans_H_

#include "ClassificationModules_global.h"
#include <KiplProcessModuleBase.h>

class CLASSIFICATIONMODULES_EXPORT FuzzyCMeans: public KiplProcessModuleBase {
public:
	FuzzyCMeans();
	virtual ~FuzzyCMeans();
	
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	int m_nNClasses;
	float m_fFuzziness;
	int m_nMaxIt;
	bool m_bParallel;
	bool m_bSaveIterations;
	std::string m_sIterationFileName;

};

#endif /* DATASCALER_H_ */

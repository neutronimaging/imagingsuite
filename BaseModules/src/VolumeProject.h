/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef VOLUMEPROJECT_H_
#define VOLUMEPROJECT_H_

#include "BaseModules_global.h"

#include <KiplProcessModuleBase.h>
#include <base/kiplenums.h>

enum eProjectionMethod {
	ProjectMean,
	ProjectMedian,
	ProjectMin,
	ProjectMax
};

class BASEMODULES_EXPORT VolumeProject: public KiplProcessModuleBase {
public:
	VolumeProject();
	virtual ~VolumeProject();
	
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	kipl::base::TImage<float,2> MeanProject(kipl::base::TImage<float,3> & img);
	kipl::base::TImage<float,2> MedianProject(kipl::base::TImage<float,3> & img);
	kipl::base::TImage<float,2> MinProject(kipl::base::TImage<float,3> & img);
	kipl::base::TImage<float,2> MaxProject(kipl::base::TImage<float,3> & img);

	int GetZLine(float * pSrc, float *pDst, size_t N, size_t stride);
	kipl::base::eImagePlanes m_Plane;
	std::string m_sFileName;
	eProjectionMethod m_ProjectionMethod;

};

std::string enum2string(eProjectionMethod method);
void string2enum(std::string str, eProjectionMethod & method);
#endif /* VOLUMEPROJECT_H_ */

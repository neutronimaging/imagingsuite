/*
 * BuildScene.h
 *
 *  Created on: Nov 8, 2012
 *      Author: anders
 */

#ifndef BUILDSCENE_H_
#define BUILDSCENE_H_

#include "BaseModules_global.h"
#include <KiplProcessModuleBase.h>

class BASEMODULES_EXPORT BuildScene: public KiplProcessModuleBase {
public:
	BuildScene();
	virtual ~BuildScene();

	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

};

#endif /* BUILDSCENE_H_ */

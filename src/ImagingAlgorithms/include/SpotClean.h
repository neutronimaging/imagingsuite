/*
 * SpotClean.h
 *
 *  Created on: Jan 17, 2013
 *      Author: anders
 */

#ifndef SPOTCLEAN_H_
#define SPOTCLEAN_H_

#include "../src/stdafx.h"
#include <logging/Logger.h>

namespace imagingalgorithms
{

class DLL_EXPORT SpotClean {
protected:
	kipl::logging::Logger logger;
public:
	SpotClean();
	virtual ~SpotClean();
};

}
#endif /* SPOTCLEAN_H_ */

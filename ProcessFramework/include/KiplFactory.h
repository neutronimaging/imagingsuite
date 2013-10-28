//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2008-11-11 21:09:49 +0100 (Di, 11 Nov 2008) $
// $Rev: 13 $
//

#ifndef __KIPLFACTORY_H
#define __KIPLFACTORY_H

#include "KiplEngine.h"
#include "KiplProcessConfig.h"

class DLL_EXPORT KiplFactory
{
public:
	KiplFactory(void);
	~KiplFactory(void);

	KiplEngine * BuildEngine(KiplProcessConfig &config);

};

#endif
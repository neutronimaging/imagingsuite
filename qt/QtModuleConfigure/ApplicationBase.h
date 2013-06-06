//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:24:22 +0200 (Mon, 21 May 2012) $
// $Rev: 1318 $
// $Id: ApplicationBase.h 1318 2012-05-21 14:24:22Z kaestner $
//


#ifndef APPLICATIONBASE_H_
#define APPLICATIONBASE_H_

#include "../src/stdafx.h"
class DLL_EXPORT ApplicationBase {
public:
	ApplicationBase();
	virtual ~ApplicationBase();

	virtual void UpdateDialog()=0;
	virtual void UpdateConfig()=0;
};

#endif /* APPLICATIONBASE_H_ */

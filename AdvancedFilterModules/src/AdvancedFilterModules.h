//
// This file is part of the i KIPL image processing tool by Anders Kaestner
// (c) 2008,2009,2010,2011,2012,2013 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2013-08-15 21:58:23 +0200 (Thu, 15 Aug 2013) $
// $Rev: 1481 $
//

#ifndef _ADVANCEDFILTERMODULES_H
#define _ADVANCEDFILTERMODULES_H

#include "AdvancedFilterModules_global.h"

extern "C" {
ADVANCEDFILTERMODULES_EXPORT void * GetModule(const char *application, const char * name);
ADVANCEDFILTERMODULES_EXPORT int Destroy(const char *application, void *obj);
ADVANCEDFILTERMODULES_EXPORT int LibVersion();
ADVANCEDFILTERMODULES_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif

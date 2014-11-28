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
#ifndef IMAGINGMODULES_H
#define IMAGINGMODULES_H

#include "ImagingAlgorithms_global.h"

extern "C" {
IMAGINGALGORITHMSSHARED_EXPORT void * GetModule(const char *application, const char * name);
IMAGINGALGORITHMSSHARED_EXPORT int Destroy(const char *application, void *obj);
IMAGINGALGORITHMSSHARED_EXPORT int LibVersion();
IMAGINGALGORITHMSSHARED_EXPORT int GetModuleList(const char *application, void *listptr);
}

#endif

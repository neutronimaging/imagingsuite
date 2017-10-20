//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//
#include "stdafx.h"
#include "DummmyModules.h"
#include "ModuleA.h"
#include "ModuleB.h"
#include <cstdlib>
#include <string>


// This is an example of an exported function.
DLL_EXPORT void * GetModule(const char * name)
{
	if (name!=NULL) {
		std::string sName=name;

		if (sName=="ModuleA")
			return new ModuleA;

		if (sName=="ModuleB")
			return new ModuleB;
	}

	return NULL;
}

DLL_EXPORT int Destroy(void *obj)
{
	delete reinterpret_cast<PreprocModuleBase *>(obj);

	return 0;
}

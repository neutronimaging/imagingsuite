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


// MoreDummies.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "MoreDummies.h"
#include "ModuleA.h"
#include "ModuleB.h"
#include <cstdlib>
#include <string>


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

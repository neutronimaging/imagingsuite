//
// This file is part of the ModuleConfig library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//

#ifndef MODULEITEMBASE_H_
#define MODULEITEMBASE_H_

#ifdef _MSC_VER // Shared object specific for msvc
typedef void * (__cdecl *FACTORY)(const char *,const char *);
typedef int (__cdecl *DESTROYER)(const char *,void *);
#else // Shared object specific for gcc
typedef void * (*FACTORY)(const char *, const char *) ;
typedef int (*DESTROYER)(const char *,void *);
typedef  void * HINSTANCE;
#endif

#include "ModuleConfig_global.h"
#include <string>
#include <logging/logger.h>
#include "ProcessModuleBase.h"

class MODULECONFIGSHARED_EXPORT ModuleItemBase
{
protected:
	kipl::logging::Logger logger;
public:
	ModuleItemBase(std::string application, std::string sharedobject, std::string modulename);
	ModuleItemBase(ModuleItemBase & item);
	ModuleItemBase & operator=(ModuleItemBase & item);

	virtual ProcessModuleBase * GetModule();
	bool Valid() {return m_Module!=NULL;}

	virtual ~ModuleItemBase();

protected:
	void Destroy();

private:
	void LoadModuleObject();

	HINSTANCE hinstLib;
    FACTORY m_fnModuleFactory;
	DESTROYER m_fnDestroyer;

	std::string m_sApplication;
	std::string m_sSharedObject;
	std::string  m_sModuleName;

	ProcessModuleBase *m_Module;

};


#endif /* MODULEITEMBASE_H_ */

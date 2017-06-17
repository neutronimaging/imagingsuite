//<LICENSE>
#include "stdafx.h"
#include <string>
#include <sstream>
#include <strings/miscstring.h>
#ifndef _MSC_VER
#include <dlfcn.h>
#endif

#include "../include/ModuleItem.h"
#include "../include/ReconException.h"

//--------------------------------------------------
// Module item
ModuleItem::ModuleItem(std::string application,std::string sharedobject, std::string modulename, kipl::interactors::InteractionBase *interactor):
	ModuleItemBase(application,sharedobject,modulename,interactor)
{
}

ModuleItem::~ModuleItem()
{
	logger(kipl::logging::Logger::LogVerbose,"Destroying");
}

///-------------------------------------------------------

BackProjItem::BackProjItem(std::string application, std::string sharedobject, std::string modulename, kipl::interactors::InteractionBase *interactor)	:
	logger("BackProjItem"),
	hinstLib(NULL),
	m_fnModuleFactory(NULL),
	m_fnDestroyer(NULL),
	m_Module(NULL)
{
	m_sApplication=application;
	m_sModuleName=modulename;
	m_sSharedObject=sharedobject;

	LoadModuleObject(interactor);
}

BackProjItem::BackProjItem(BackProjItem & item):
	logger("ModuleItem")
{
	hinstLib=item.hinstLib;
	m_fnModuleFactory=item.m_fnModuleFactory; 
	m_fnDestroyer=item.m_fnDestroyer;

	m_sApplication=item.m_sApplication;
	m_sSharedObject=item.m_sSharedObject;
	m_sModuleName=item.m_sModuleName;

	m_Module=item.m_Module;
}

BackProjItem & BackProjItem::operator=(BackProjItem & item)
{
	hinstLib=item.hinstLib;
	m_fnModuleFactory=item.m_fnModuleFactory; 
	m_fnDestroyer=item.m_fnDestroyer;

	m_sApplication=item.m_sApplication;
	m_sSharedObject=item.m_sSharedObject;
	m_sModuleName=item.m_sModuleName;

	m_Module=item.m_Module;

	return *this;
}

BackProjectorModuleBase * BackProjItem::GetModule()
{
	return m_Module;
}

	
BackProjItem::~BackProjItem()
{
	logger(kipl::logging::Logger::LogVerbose,"D'tor");
	Destroy();
}

void BackProjItem::LoadModuleObject(kipl::interactors::InteractionBase *interactor)
{
	std::ostringstream msg;


#ifdef _MSC_VER
	std::wstring so(m_sSharedObject.length(),' ');

	copy(m_sSharedObject.begin(),m_sSharedObject.end(),so.begin());
	hinstLib = LoadLibrary(so.c_str());
#else
	hinstLib = dlopen(m_sSharedObject.c_str(), RTLD_LAZY);
#endif

	if (hinstLib != NULL) 
    { 
#ifdef _MSC_VER
		m_fnModuleFactory = reinterpret_cast<BP_FACTORY>(GetProcAddress(hinstLib, "GetModule"));
#else
		m_fnModuleFactory = reinterpret_cast<BP_FACTORY>(dlsym(hinstLib, "GetModule"));
#endif
		 // If the function address is valid, call the function.
		if (NULL != m_fnModuleFactory) 
        {
			m_Module=reinterpret_cast<BackProjectorModuleBase *>(m_fnModuleFactory(m_sApplication.c_str(), m_sModuleName.c_str(), reinterpret_cast<void *>(interactor)));
			if (m_Module==NULL) {
				msg.str("");
				msg<<"Failed to create "<<m_sModuleName<<" from "<<m_sSharedObject;
				throw ReconException(msg.str(),__FILE__,__LINE__);
			}
        }
		else
		{
			msg.str("");
			msg<<"Failed to get the factory from "<<m_sSharedObject<<" (Error: "
#ifdef _MSC_VER
					<<GetLastError()<<")";
#else
					<<dlerror()<<")";
#endif

			throw ReconException(msg.str(),__FILE__,__LINE__);
		}


#ifdef _MSC_VER
		m_fnDestroyer = reinterpret_cast<DESTROYER>(GetProcAddress(hinstLib, "Destroy"));
#else
		m_fnDestroyer = reinterpret_cast<DESTROYER>(dlsym(hinstLib, "Destroy"));
#endif

		if (m_fnDestroyer==NULL) {
			msg.str("");
			msg<<"Failed to get the destroyer from "<<m_sSharedObject<<" (Error: "
#ifdef _MSC_VER
					<<GetLastError()<<")";
#else
					<<dlerror()<<")";
#endif
			throw ReconException(msg.str(),__FILE__,__LINE__);
		}
	}
	else {
		msg.str("");
		msg<<"Failed to open object file "<<m_sSharedObject<<" to load module "<<m_sModuleName<<" (Error: "
#ifdef _MSC_VER
					<<GetLastError()<<")";
#else
					<<dlerror()<<")";
#endif

		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
}

void BackProjItem::Destroy()
{
	logger(kipl::logging::Logger::LogVerbose,"Destroying");
	
	m_fnDestroyer(m_sApplication.c_str(),reinterpret_cast<void *>(m_Module));

#ifdef _MSC_VER
	FreeLibrary(hinstLib);
#else
	 dlclose(hinstLib);
#endif
}

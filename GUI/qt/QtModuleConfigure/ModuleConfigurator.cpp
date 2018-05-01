//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-10-18 21:06:50 +0200 (Thu, 18 Oct 2012) $
// $Rev: 1380 $
// $Id: ModuleConfigurator.cpp 1380 2012-10-18 19:06:50Z kaestner $
//


// ModuleConfigurator.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "QtModuleConfigure_global.h"
#include "ModuleConfigurator.h"
#include <ModuleException.h>
#include <QMessageBox>
#ifndef _MSC_VER
#include <dlfcn.h>
#endif

// This is the constructor of a class that has been exported.
// see ModuleConfigurator.h for the class definition
ModuleConfigurator::ModuleConfigurator() :
	logger("ModuleConfigurator"),
	hinstLib(NULL),
    m_fnModuleFactory(NULL),
	m_fnDestroyer(NULL),
	m_Dialog(NULL),
	m_Config(NULL)
{

}

ModuleConfigurator::~ModuleConfigurator()
{
	Destroy();
}
	
bool ModuleConfigurator::configure(std::string application, std::string SharedObjectName, std::string ModuleName, std::map<std::string, std::string> &parameters)
{

    ostringstream msg;

    std::string str = m_Config->SanitySlicesCheck();
    if(!str.empty())
    {
        QMessageBox mbox;
        mbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Abort);
        mbox.setDefaultButton(QMessageBox::Abort);
        mbox.setText(QString::fromStdString(str));
        mbox.setWindowTitle("Sanity check warning");
        int res_msg=mbox.exec();
        if (res_msg==QMessageBox::Abort)
            return false;
    }

	try {
		GetDialog(application,SharedObjectName,ModuleName);
	}
	catch (ModuleException &e)
	{
		msg<<"Failed to load dialog."<<e.what();
	}
	catch (kipl::base::KiplException & e) {
		msg<<"Failed to load dialog."<<e.what();
	}
	catch (std::exception &e) {
		msg<<"Failed to load dialog."<<e.what();
	}
	catch (...) {
		msg<<"Failed to load dialog.";
	}
	if (!msg.str().empty())
		throw ModuleException(msg.str(),__FILE__,__LINE__);

	bool res=false;
	if (m_Dialog!=NULL) {
		msg.str("");

         try {
            if (m_Dialog->NeedImages())
                GetImage(ModuleName);
        }
        catch (ModuleException &e)
        {
            msg<<"Module exception: Failed to load projection data for "<<ModuleName<<".\n"<<e.what();
        }
        catch (kipl::base::KiplException & e) {
            msg<<"kipl exception: Failed to load projection data for "<<ModuleName<<".\n"<<e.what();
        }
        catch (std::exception &e) {
            msg<<"STL exception: Failed to load projection data for "<<ModuleName<<".\n"<<e.what();
        }
        catch (...) {
            msg<<"Failed to load projection data with unhandled exception for "<<ModuleName<<".";
        }
        if (!msg.str().empty())
            throw ModuleException(msg.str(),__FILE__,__LINE__);

        res=m_Dialog->exec(m_Config, parameters, m_Image);

		m_Image.FreeImage();
	}
	else {
		msg.str("");
		msg<<"Could not load dialog module "<<ModuleName<<" from "<<SharedObjectName;
		logger(kipl::logging::Logger::LogMessage,msg.str());
	}

	return res;
}

void ModuleConfigurator::GetDialog(std::string application, std::string sharedobjectname, std::string objectname)
{
	std::ostringstream msg;

	m_sApplication=application;
	m_sSharedObject=sharedobjectname;
	m_sModuleName=objectname;
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
		m_fnModuleFactory = reinterpret_cast<FACTORY>(GetProcAddress(hinstLib, "GetGUIModule"));
#else
		m_fnModuleFactory = reinterpret_cast<FACTORY>(dlsym(hinstLib, "GetGUIModule"));
#endif
		 // If the function address is valid, call the function.
		if (NULL != m_fnModuleFactory)
        {
            m_Dialog=reinterpret_cast<ConfiguratorDialogBase *>(m_fnModuleFactory(m_sApplication.c_str(),m_sModuleName.c_str(),nullptr));
			if (m_Dialog==NULL) {
				msg.str("");
				msg<<"Failed to create "<<m_sModuleName<<" from "<<m_sSharedObject;
				throw ModuleException(msg.str(),__FILE__,__LINE__);
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

			throw ModuleException(msg.str(),__FILE__,__LINE__);
		}


#ifdef _MSC_VER
		m_fnDestroyer = reinterpret_cast<DESTROYER>(GetProcAddress(hinstLib, "DestroyGUIModule"));
#else
		m_fnDestroyer = reinterpret_cast<DESTROYER>(dlsym(hinstLib, "DestroyGUIModule"));
#endif

		if (m_fnDestroyer==NULL) {
			msg.str("");
			msg<<"Failed to get the destroyer from "<<m_sSharedObject<<" (Error: "
#ifdef _MSC_VER
					<<GetLastError()<<")";
#else
					<<dlerror()<<")";
#endif
			throw ModuleException(msg.str(),__FILE__,__LINE__);
		}
	}
	else {
		msg.str("");
		msg<<"Failed to load "<<m_sSharedObject<<" (Error: "
#ifdef _MSC_VER
					<<GetLastError()<<")";
#else
					<<dlerror()<<")";
#endif

		throw ModuleException(msg.str(),__FILE__,__LINE__);
	}
}

void ModuleConfigurator::Destroy()
{
	logger(kipl::logging::Logger::LogMessage,"Destroying");

	if (m_Dialog!=NULL)
		m_fnDestroyer(m_sApplication.c_str(),reinterpret_cast<void *>(m_Dialog));

	if (hinstLib!=NULL) {
	#ifdef _MSC_VER
		FreeLibrary(hinstLib);
	#else
		 dlclose(hinstLib);
	#endif
	}

}

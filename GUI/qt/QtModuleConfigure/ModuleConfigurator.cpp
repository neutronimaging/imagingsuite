//<LICENSE>

// ModuleConfigurator.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "QtModuleConfigure_global.h"
#include "ModuleConfigurator.h"
#include <ModuleException.h>
#include <QMessageBox>
#include <QDebug>
#ifndef _MSC_VER
#include <dlfcn.h>
#endif

// This is the constructor of a class that has been exported.
// see ModuleConfigurator.h for the class definition
ModuleConfigurator::ModuleConfigurator(kipl::interactors::InteractionBase *interactor) :
	logger("ModuleConfigurator"),
    hinstLib(nullptr),
    m_fnModuleFactory(nullptr),
    m_fnDestroyer(nullptr),
    m_Dialog(nullptr),
    m_Config(nullptr),
    m_Interactor(interactor)
{
//    if (m_Interactor!=nullptr)
//        qDebug() << "Have interactor for moduleConfigurator";
//    else
//        qDebug() << "Don't have interactor for moduleConfigurator";
}

ModuleConfigurator::~ModuleConfigurator()
{
   // destroy();
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
        loadDialog(application,SharedObjectName,ModuleName);
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
    if (m_Dialog!=nullptr) {
		msg.str("");

         try {
            if (m_Dialog->NeedImages())
            {
                if (GetImage(ModuleName,m_Interactor))
                    res=m_Dialog->exec(m_Config, parameters, m_Image);
            }
            else
            {
                res=m_Dialog->exec(m_Config, parameters, m_Image);
            }

            m_Image.FreeImage();
            destroy();
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
            throw ModuleException(msg.str(),__FILE__,__LINE__);;

	}
	else {
		msg.str("");
		msg<<"Could not load dialog module "<<ModuleName<<" from "<<SharedObjectName;
		logger(kipl::logging::Logger::LogMessage,msg.str());
	}

	return res;
}

void ModuleConfigurator::loadDialog(std::string application, std::string sharedobjectname, std::string objectname)
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

    if (hinstLib != nullptr)
    {
#ifdef _MSC_VER
		m_fnModuleFactory = reinterpret_cast<FACTORY>(GetProcAddress(hinstLib, "GetGUIModule"));
#else
		m_fnModuleFactory = reinterpret_cast<FACTORY>(dlsym(hinstLib, "GetGUIModule"));
#endif
		 // If the function address is valid, call the function.
        if (nullptr != m_fnModuleFactory)
        {
            m_Dialog=reinterpret_cast<ConfiguratorDialogBase *>(m_fnModuleFactory(m_sApplication.c_str(),m_sModuleName.c_str(),nullptr));
            if (m_Dialog==nullptr) {
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

        if (m_fnDestroyer==nullptr) {
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

void ModuleConfigurator::destroy()
{
	logger(kipl::logging::Logger::LogMessage,"Destroying");

    if (m_Dialog!=nullptr) {
        int res=m_fnDestroyer(m_sApplication.c_str(),reinterpret_cast<void *>(m_Dialog));

        if (res!=0) {
            logger.error("Something went really wrong");
            return;
        }
    }
    m_Dialog=nullptr;

    if (hinstLib!=nullptr)
    {
#ifdef _MSC_VER
		FreeLibrary(hinstLib);
#else
        dlclose(hinstLib);
#endif
	}

    hinstLib=nullptr;

}

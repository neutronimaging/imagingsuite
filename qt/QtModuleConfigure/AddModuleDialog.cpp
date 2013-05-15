//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-12-27 12:16:26 +0100 (Thu, 27 Dec 2012) $
// $Rev: 1420 $
// $Id: AddModuleDialog.cpp 1420 2012-12-27 11:16:26Z kaestner $
//


#include "stdafx.h"
#undef max
#include <iostream>
#include <sstream>
#include "AddModuleDialog.h"
#include <strings/filenames.h>
#include <ModuleException.h>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#ifndef _MSC_VER
#include <dlfcn.h>
#endif

#ifdef _MSC_VER // Shared object specific for msvc
typedef int (__cdecl *MODULELIST)(const char *,void *);
#else // Shared object specific for gcc
typedef int (*MODULELIST)(const char *,void *);
typedef  void * HINSTANCE;
#endif

AddModuleDialog::AddModuleDialog(QWidget * parent) :
    QDialog(parent),
	logger("AddModuleDialog"),
    m_Label_file("Module library"),
    m_Label_module("Module name"),
    m_Filechooser_button("Browse"),
    m_Button_OK("OK"),
    m_Button_Cancel("Cancel"),
    m_sApplication(""),
    m_sApplicationPath("")
{
    setWindowTitle(tr("Add a module"));
    setLayout(&m_Dlg_layout);
    m_Dlg_layout.addLayout(&m_Filechooser_layout);
    m_Dlg_layout.addLayout(&m_Moduleselect_layout);
    m_Dlg_layout.addLayout(&m_Layout_Controls);

    m_Label_file.setText(tr("Module library"));
    m_Label_module.setText(tr("Module"));
    m_Filechooser_layout.addWidget(&m_Label_file);
    m_Filechooser_layout.addWidget(&m_Modulefile_edit);
    m_Filechooser_layout.addWidget(&m_Filechooser_button);

    m_Moduleselect_layout.addWidget(&m_Label_module);
    m_Moduleselect_layout.addWidget(&m_Combobox_modules);
    m_Moduleselect_layout.addStretch();

    m_Layout_Controls.addStretch();
    m_Layout_Controls.addWidget(&m_Button_Cancel);
    m_Layout_Controls.addWidget(&m_Button_OK);
    setFixedHeight(sizeHint().height());

    connect(&m_Button_OK,SIGNAL(clicked()),this,SLOT(accept()));
    connect(&m_Button_Cancel,SIGNAL(clicked()),this,SLOT(reject()));
//    connect(&m_Filechooser_button,SIGNAL(clicked()),this,SLOT(on_change_objectfile()));
}

//AddModuleDialog::~AddModuleDialog()
//{

//}

int Initialize(std::string application, std::string application_path="")
{

    return 0;
}

int AddModuleDialog::exec(std::string application, ModuleConfig &module)
{
    int res=0;
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),tr("libs (*.dylib)"));

    if (fileName.isEmpty()) {
        logger(kipl::logging::Logger::LogMessage,"No file selected");
        return 0;
    }

    logger(kipl::logging::Logger::LogMessage,fileName.toStdString());
    std::string libname=fileName.toStdString();

    libname = libname.substr(libname.find_last_of('/'));

    m_Modulefile_edit.setText(fileName);

    module.m_sSharedObject=fileName.toStdString();

    if (UpdateModuleCombobox(fileName)!=0)
        return QDialog::Rejected;


//		module.m_sSharedObject=filechooser_objectfile.get_filename();

//		module.m_sModule=combobox_modules.get_active_text();
//		module.m_bActive=true;
//		module.parameters=parameter_manager.GetParameters();
//	}

    return QDialog::exec();
}

int AddModuleDialog::UpdateModuleCombobox(QString fname)
{
	std::ostringstream msg;

    m_Combobox_modules.clear();
	
    try {
        modulelist.clear();
        modulelist=GetModuleList(fname.toStdString());
    }
    catch (ModuleException &e) {
        msg<<"Failed to generate module list from "<<fname.toStdString()<<"\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
    }

    if (!msg.str().empty()) {
        QMessageBox msgBox;

        msgBox.setWindowTitle("Error");
        msgBox.setText("Could not create a module list from the selected library.");
        msgBox.setDetailedText(QString::fromStdString(msg.str()));
        msgBox.exec();
        return -1;
    }

    std::map<std::string, std::map<std::string, std::string> >::iterator it;


    for (it=modulelist.begin(); it!=modulelist.end(); it++) {
        m_Combobox_modules.addItem(QString::fromStdString(it->first));
    }
//		Glib::ustring str=it->first;
//#if _MSC_VER
//		combobox_modules.append_text(str);
//#else
//		combobox_modules.append(str);
//#endif
//	}

//	combobox_modules.set_active(0);
//	table_moduleselector.show_all_children();

	return 0;
}

std::map<std::string, std::map<std::string, std::string> > AddModuleDialog::GetModuleList(std::string filename)
{
	std::ostringstream msg;
	HINSTANCE hinstLib;
#ifdef _MSC_VER
	std::wstring so(filename.length(),' ');

	copy(filename.begin(),filename.end(),so.begin());
	hinstLib = LoadLibrary(so.c_str());
#else
	hinstLib = dlopen(filename.c_str(), RTLD_LAZY);
#endif

	if (hinstLib != NULL) 
    { 
		MODULELIST fnGetModuleList;
#ifdef _MSC_VER
		fnGetModuleList = reinterpret_cast<MODULELIST>(GetProcAddress(hinstLib, "GetModuleList"));
#else
		fnGetModuleList = reinterpret_cast<MODULELIST>(dlsym(hinstLib, "GetModuleList"));
#endif
		 // If the function address is valid, call the function.
		if (NULL != fnGetModuleList) 
        {
			if (fnGetModuleList(m_sApplication.c_str(),&modulelist)!=0) {
				msg.str("");
				msg<<"Shared object file "<<filename<<" does not contain modules for "<<m_sApplication;
				throw ModuleException(msg.str(),__FILE__,__LINE__);
			}

        }
		else
		{
			msg.str("");
			msg<<"Failed to get the module list from "<<filename<<" (Error: "
#ifdef _MSC_VER
					<<GetLastError()<<")";
#else
					<<dlerror()<<")";
#endif

			throw ModuleException(msg.str(),__FILE__,__LINE__);
		}
	}

	return modulelist;
}

void AddModuleDialog::on_changed_module()
{
//#ifdef _MSC_VER
//	std::string selection=combobox_modules.get_active_text();
//#else
//	Glib::ustring selection=combobox_modules.get_active_text();
//#endif

//	if (!selection.empty()) {
//		parameter_manager.SetParameters(modulelist[selection]);
//	}
}

void AddModuleDialog::on_change_objectfile()
{
//	UpdateModuleCombobox(filechooser_objectfile.get_filename());
}


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
#include "QtModuleConfigure_global.h"
#undef max
#include <iostream>
#include <sstream>
#include "AddModuleDialog.h"
#include <strings/filenames.h>
#include <ModuleException.h>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
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
    m_Button_Browse("Browse"),
    m_Button_OK("OK"),
    m_Button_Cancel("Cancel"),
    m_sApplication("muhrec"),
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
    m_Filechooser_layout.addWidget(&m_Button_Browse);

    m_Moduleselect_layout.addWidget(&m_Label_module);
    m_Moduleselect_layout.addWidget(&m_Combobox_modules);
    m_Moduleselect_layout.addStretch();

    m_Layout_Controls.addStretch();
    m_Layout_Controls.addWidget(&m_Button_Cancel);
    m_Layout_Controls.addWidget(&m_Button_OK);
    setFixedHeight(sizeHint().height());

    connect(&m_Button_OK,SIGNAL(clicked()),this,SLOT(accept()));
    connect(&m_Button_Cancel,SIGNAL(clicked()),this,SLOT(reject()));
    connect(&m_Button_Browse,SIGNAL(clicked()),this,SLOT(on_change_objectfile()));
}

int AddModuleDialog::configure(std::string application, std::string defaultsource, std::string application_path)
{
    m_sApplication         = application;
    m_sApplicationPath     = application_path;
    m_sDefaultModuleSource = defaultsource;

    return 0;
}

int AddModuleDialog::exec()
{

    QString appPath = QCoreApplication::applicationDirPath();
    std::ostringstream msg;

    msg<<"appPath "<<appPath.toStdString();
    logger(kipl::logging::Logger::LogMessage,msg.str());

    QString fileName=QString::fromStdString(m_sDefaultModuleSource);
    msg.str(""); msg<<"default module "<<m_sDefaultModuleSource;


    logger(kipl::logging::Logger::LogMessage,msg.str());

    if (fileName.isEmpty()) {
        #ifdef Q_OS_WIN
            fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),appPath,tr("libs (*.dll)"));
        #else
            appPath += +"/../Frameworks";
            fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),appPath,tr("libs (*.dylib | *.so)"));
        #endif
    }

    logger(kipl::logging::Logger::LogMessage,fileName.toStdString());
    if (fileName.isEmpty()) {
        logger(kipl::logging::Logger::LogError,"No file selected");
        return 0;
    }

    m_Modulefile_edit.setText(fileName);

    if (UpdateModuleCombobox(fileName)!=0)
    {
        return QDialog::Rejected;
    }

    m_ModuleConfig.m_sSharedObject=fileName.toStdString();
    m_ModuleConfig.m_sModule=modulelist.begin()->first;
    m_ModuleConfig.parameters=modulelist.begin()->second;
    m_ModuleConfig.m_bActive=true;

    return QDialog::exec();
}

int AddModuleDialog::UpdateModuleCombobox(QString fname)
{
    std::ostringstream msg;
    try {
        modulelist.clear();
        modulelist=GetModuleList(fname.toStdString());
    }
    catch (ModuleException &e) {
        msg<<"Failed to generate module list from "<<fname.toStdString()<<"\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Failed to generate module list from "<<fname.toStdString()<<" with a KiplException\n"<<e.what();
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

    m_Combobox_modules.clear();
    std::map<std::string, std::map<std::string, std::string> >::iterator it;
    msg.str("");
    msg<<"The library has "<<modulelist.size()<<" modules";
    logger(kipl::logging::Logger::LogVerbose,msg.str());
    for (it=modulelist.begin(); it!=modulelist.end(); it++) {
        m_Combobox_modules.addItem(QString::fromStdString(it->first));
    }

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

    if (hinstLib != nullptr)
    {
        MODULELIST fnGetModuleList;
#ifdef _MSC_VER
        fnGetModuleList = reinterpret_cast<MODULELIST>(GetProcAddress(hinstLib, "GetModuleList"));
#else
        fnGetModuleList = reinterpret_cast<MODULELIST>(dlsym(hinstLib, "GetModuleList"));
#endif
        msg.str("");
        msg<<"Got functions from "<<filename<<" success="<<(fnGetModuleList == nullptr ? "no" : "yes");
        logger(kipl::logging::Logger::LogMessage,msg.str());
         // If the function address is valid, call the function.
        if (fnGetModuleList !=nullptr)
        {

            if (fnGetModuleList(m_sApplication.c_str(),&modulelist)!=0) {
                msg.str("");
                msg<<"Shared object file "<<filename<<" does not contain modules for "<<m_sApplication;
                logger(kipl::logging::Logger::LogError,msg.str());
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
    else {
        msg.str("");
        msg<<"Failed to open library file "<<filename;
        logger(kipl::logging::Logger::LogMessage,msg.str());

        msg.str("");
        msg<<"Failed to open library file "<<filename<<" (Error: "
#ifdef _MSC_VER
                <<GetLastError()<<")";
#else
                <<dlerror()<<")";
#endif

        throw ModuleException(msg.str(),__FILE__,__LINE__);
    }

    return modulelist;
}

void AddModuleDialog::on_change_objectfile()
{
    std::ostringstream msg;

    QString appPath = QCoreApplication::applicationDirPath();

    QString fileName;

    #ifdef Q_OS_WIN
        fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),appPath,tr("libs (*.dll)"));
    #else
        appPath+="/../Frameworks";
        fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),appPath,tr("libs (*.dylib | *.so)"));
    #endif

    logger(kipl::logging::Logger::LogMessage,fileName.toStdString());
    if (fileName.isEmpty()) {
        logger(kipl::logging::Logger::LogError,"No file selected");
        return;
    }

    if (fileName.toStdString() == m_ModuleConfig.m_sSharedObject ) {
        logger(kipl::logging::Logger::LogMessage,"The same library file was selected.");
        return;
    }

    if (UpdateModuleCombobox(fileName)!=0)
        return;

    m_Modulefile_edit.setText(fileName);
}

void AddModuleDialog::accept()
{
    logger(kipl::logging::Logger::LogVerbose,"OK");
    m_ModuleConfig.m_sSharedObject=m_Modulefile_edit.text().toStdString();
    m_ModuleConfig.m_sModule=m_Combobox_modules.currentText().toStdString();
    m_ModuleConfig.parameters=modulelist[m_ModuleConfig.m_sModule];
    m_ModuleConfig.m_bActive=true;

    QDialog::accept();
}

void AddModuleDialog::reject()
{
    logger(kipl::logging::Logger::LogMessage,"Cancel");
    QDialog::reject();
}

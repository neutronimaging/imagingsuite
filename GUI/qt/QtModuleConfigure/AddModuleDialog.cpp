//<LICENSE>

#include "stdafx.h"
#include "QtModuleConfigure_global.h"
#undef max
#include <iostream>
#include <sstream>
#include "AddModuleDialog.h"
#include <modulelibnamemanager.h>
#include <strings/filenames.h>
#include <ModuleException.h>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
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
    m_sApplicationPath(""),
    m_sPreprocessorsPath(""),
    m_ModuleConfig("","")
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

    m_sApplicationPath = QCoreApplication::applicationDirPath().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_sApplicationPath,true);

    ModuleLibNameManger mlnm(m_sApplicationPath, false,"Preprocessors");
    m_sPreprocessorsPath = mlnm.generateLibPath();

    kipl::strings::filenames::CheckPathSlashes(m_sPreprocessorsPath,true);
}

int AddModuleDialog::configure( const std::string &application, 
                                const std::string &defaultsource, 
                                const std::string &application_path, 
                                const std::string &sCategory)
{
    m_sApplication         = application;
    m_sApplicationPath     = application_path;
    m_sDefaultModuleSource = defaultsource;
    

    return 0;
}

int AddModuleDialog::exec()
{

    std::ostringstream msg;

    msg<<"ApplicationPath "<<m_sApplicationPath;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    std::string fileName=m_sDefaultModuleSource;
    QString qfileName = QString::fromStdString(fileName);
    msg.str(""); msg<<"default module "<<m_sDefaultModuleSource;

    logger(kipl::logging::Logger::LogMessage,msg.str());

    if (fileName.empty()) {
        auto os = kipl::base::getOperatingSystem();
        std::string filterstr;
        switch (os)
        {
            case kipl::base::OSUnknown : throw kipl::base::KiplException("OS not recognized",__FILE__,__LINE__);
            case kipl::base::OSWindows : filterstr = "libs (*.dll)"; break;
            case kipl::base::OSMacOS   : filterstr = "libs (*.dylib)"; break;
            case kipl::base::OSLinux   : filterstr = "libs (*.so)"; break;
        }

        qfileName = QFileDialog::getOpenFileName(this,tr("Open module library"),QString::fromStdString(m_sPreprocessorsPath),tr(filterstr.c_str()));
        // #if defined(Q_OS_WIN)
        //     qfileName = QFileDialog::getOpenFileName(this,tr("Open module library"),QString::fromStdString(m_sPreprocessorsPath),tr("libs (*.dll)"));
        // #elif defined(Q_OS_MAC)
        //     qfileName = QFileDialog::getOpenFileName(this,tr("Open module library"),QString::fromStdString(m_sPreprocessorsPath),tr("libs (*.dylib)"));
        // #elif defined(Q_OS_LINUX)
        //     qfileName = QFileDialog::getOpenFileName(this,tr("Open module library"),QString::fromStdString(m_sPreprocessorsPath),tr("libs (*.so)"));
        // #endif
    }

    logger(kipl::logging::Logger::LogMessage,qfileName.toStdString());
    if (qfileName.isEmpty()) {
        QMessageBox::warning(this,"Warning","No module library file was chosen");

        logger(kipl::logging::Logger::LogWarning,"No file selected");
        return 0;
    }

    if (UpdateModuleCombobox(qfileName)!=0)
    {
        return QDialog::Rejected;
    }
    m_Modulefile_edit.setText(qfileName);

    fileName = qfileName.toStdString();
    kipl::strings::filenames::CheckPathSlashes(fileName,false);
    m_ModuleConfig.m_sSharedObject=fileName;

    m_ModuleConfig.m_sModule  = modulelist.begin()->first;
    m_ModuleConfig.parameters = modulelist.begin()->second;
    m_ModuleConfig.m_bActive=true;

    return QDialog::exec();
}

int AddModuleDialog::UpdateModuleCombobox(QString &fname)
{
    QDir dir;

    if (!dir.exists(fname))
    {
        logger.warning("Module file doesn't exist");

        QFileDialog dlg(this,"Select a module file",QString::fromStdString(m_sPreprocessorsPath));

        auto res = dlg.exec();

        if (res!=QDialog::Accepted)
            return 0;

        fname = dlg.selectedFiles().first();
    }

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
    std::wstring so(filename.begin(),filename.end());

    hinstLib = LoadLibraryW(so.c_str());
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

    QString fileName;

    #if defined(Q_OS_WIN)
        fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),QString::fromStdString(m_sPreprocessorsPath),tr("libs (*.dll)"));
    #elif defined(Q_OS_MAC)
        fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),QString::fromStdString(m_sPreprocessorsPath),tr("libs (*.dylib)"));
    #elif defined(Q_OS_LINUX)
        fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),QString::fromStdString(m_sPreprocessorsPath),tr("libs (*.so)"));
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

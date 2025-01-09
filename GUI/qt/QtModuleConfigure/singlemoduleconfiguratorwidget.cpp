//<LICENSE>
#include "stdafx.h"
#include "QtModuleConfigure_global.h"
#include "singlemoduleconfiguratorwidget.h"
#include "singlemodulesettingsdialog.h"
#include "QListWidgetModuleItem.h"

#include <iostream>
#include <sstream>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <ModuleException.h>
#include <base/KiplException.h>

#include <QDebug>

#ifndef _MSC_VER
#include <dlfcn.h>
#endif

#ifdef _MSC_VER // Shared object specific for msvc
typedef int (__cdecl *MODULELIST)(const char *,void *);
#else // Shared object specific for gcc
typedef int (*MODULELIST)(const char *,void *);
typedef  void * HINSTANCE;
#endif


SingleModuleConfiguratorWidget::SingleModuleConfiguratorWidget(QWidget *parent) :
    QFrame(parent),
    logger("SingleModuleConfiguratorWidget"),
    m_ModuleConfig("")
{
    this->setFrameStyle(6);
    this->setLayout(&m_LayoutMain);
    m_LayoutMain.addWidget(&m_LabelDescription);
    m_LayoutMain.addWidget(&m_LabelModuleName);
    m_LayoutMain.addWidget(&m_ButtonConfigure);

    m_LabelDescription.setText(tr("Module name: "));
    m_LabelDescription.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    m_LabelModuleName.setText(QString::fromStdString(m_ModuleConfig.m_sModule));
    m_ButtonConfigure.setText(tr("Configure"));
    m_ButtonConfigure.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    connect(&m_ButtonConfigure, SIGNAL(clicked()),this,SLOT(on_ButtonConfigure_Clicked()));

    show();

}

void SingleModuleConfiguratorWidget::Configure(std::string sApplicationName, std::string sDefaultModuleSource, std::string sApplicationPath)
{
    m_sApplication         = sApplicationName;
    m_sApplicationPath     = sApplicationPath;
    m_sDefaultModuleSource = sDefaultModuleSource;
    m_ModuleConfig.m_sSharedObject = sDefaultModuleSource;
    m_ModuleConfig.setAppPath(sApplicationPath);
}

void SingleModuleConfiguratorWidget::setDescriptionLabel(QString lbl)
{
    m_LabelDescription.setText(lbl);
}

QSize SingleModuleConfiguratorWidget::minimumSizeHint() const
{
    return QSize(300, 50);
}

QSize SingleModuleConfiguratorWidget::sizeHint() const
{
    return QSize(300, 50);
}

void SingleModuleConfiguratorWidget::SetModule(ModuleConfig &module)
{
    m_ModuleConfig=module;
    m_LabelModuleName.setText(QString::fromStdString(m_ModuleConfig.m_sModule));
}

ModuleConfig SingleModuleConfiguratorWidget::GetModule()
{
    return m_ModuleConfig;
}


void SingleModuleConfiguratorWidget::SetDescription(QString name)
{
    m_LabelDescription.setText(name);
}


void SingleModuleConfiguratorWidget::on_ButtonConfigure_Clicked()
{
    SingleModuleSettingsDialog dlg(m_sApplication, m_sApplicationPath,m_sDefaultModuleSource,this);

    int res=dlg.exec(m_ModuleConfig);

    if (res==QDialog::Accepted)
    {
        m_ModuleConfig=dlg.getModule();
        m_LabelModuleName.setText(QString::fromStdString(m_ModuleConfig.m_sModule));
    }
}

namespace oldGUI {
SingleModuleSettingsDialog::SingleModuleSettingsDialog(const std::string &sApplicationName,
                                                       const std::string &sApplicationPath,
                                                       const std::string &sDefaultModuleSource,
                                                       QWidget *parent) :
    QDialog(parent),
    logger("SingleModuleSettingsDialog"),
    m_sApplication(sApplicationName),
    m_sApplicationPath(sApplicationPath),
    m_sDefaultModuleSource(sDefaultModuleSource),
    m_ModuleConfig("")
{
    BuildDialog();
    connect(&m_ButtonBrowse,SIGNAL(clicked()),this,SLOT(on_ButtonBrowse_Clicked()));
    connect(&m_Buttons,SIGNAL(clicked(QAbstractButton*)),this,SLOT(on_ButtonBox_Clicked(QAbstractButton*)));
    connect(&m_ComboModules,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_ComboBox_Changed(QString)));
    show();

}

int SingleModuleSettingsDialog::exec(ModuleConfig &config)
{
    std::ostringstream msg;
    m_ModuleConfig=config;
    m_ModuleConfig.setAppPath(QCoreApplication::applicationDirPath().toStdString());

    QDir dir;
    if (dir.exists(QString::fromStdString(m_ModuleConfig.m_sSharedObject)))
    {
        UpdateModuleCombobox(QString::fromStdString(m_ModuleConfig.m_sSharedObject),false);
        msg<<"Index for "<<config.m_sModule<<" set to "<<m_ComboModules.findText(QString::fromStdString(config.m_sModule));
        logger(kipl::logging::Logger::LogMessage,msg.str());

        m_ComboModules.setCurrentIndex(m_ComboModules.findText(QString::fromStdString(config.m_sModule)));
        m_ModuleConfig=config;
        UpdateCurrentModuleParameters();
    }
    else
    {
        UpdateModuleCombobox(QString::fromStdString(m_sDefaultModuleSource),false);
    }

    return exec();
}

int SingleModuleSettingsDialog::exec()
{
    return QDialog::exec();
}

void SingleModuleSettingsDialog::on_ButtonBox_Clicked(QAbstractButton *button)
{
    std::ostringstream msg;
    QDialogButtonBox::StandardButton standardButton = m_Buttons.standardButton(button);
    switch(standardButton)
    {
        // Standard buttons:
        case QDialogButtonBox::Ok:
            accept();
            break;
        case QDialogButtonBox::Cancel:
            reject();
            break;

        // Non-standard buttons:
        case QDialogButtonBox::NoButton:
        {
           int actionRole = button->property("ActionRole").toInt();
           QString s_Value=button->text();
           msg<<"Got NoButton: "<<actionRole<<", text:"<<button->text().toStdString();

           logger(kipl::logging::Logger::LogMessage,msg.str());

           if (s_Value.toLower()=="add")
           {
               logger(kipl::logging::Logger::LogMessage,"Add parameter");
           }

           if (s_Value.toLower()=="delete")
           {
               logger(kipl::logging::Logger::LogMessage,"Delete parameter");
           }
           break;
        }

        case QDialogButtonBox::Apply: break;
        case QDialogButtonBox::Reset: break;
        case QDialogButtonBox::Save: break;
        case QDialogButtonBox::SaveAll: break;
        case QDialogButtonBox::Open: break;
        case QDialogButtonBox::Close: break;
        case QDialogButtonBox::Discard: break;
        case QDialogButtonBox::Ignore: break;
        case QDialogButtonBox::No: break;
        case QDialogButtonBox::YesToAll: break;
        case QDialogButtonBox::NoToAll: break;
        case QDialogButtonBox::Yes: break;
        case QDialogButtonBox::Retry: break;
        case QDialogButtonBox::Abort: break;
        case QDialogButtonBox::Help: break;
        case QDialogButtonBox::RestoreDefaults: break;
        }

}

void  SingleModuleSettingsDialog::on_ButtonBrowse_Clicked()
{      
    logger(kipl::logging::Logger::LogMessage,"browse");

    QString appPath = QCoreApplication::applicationDirPath();
    QString fileName;

    #if defined(Q_OS_WIN)
        appPath+="/Preprocessors";
        fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),appPath,tr("libs (*.dll)"));
    #elif defined(Q_OS_MAC)
        appPath+="/../Preprocessors";
        fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),appPath,tr("libs (*.dylib)"));
    #elif defined(Q_OS_LINUX)
        appPath+="/../Preprocessors";
        fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),appPath,tr("libs (*.so)"));
    #endif

    logger(kipl::logging::Logger::LogMessage,appPath.toStdString());

    if ( fileName.isNull() )
    {
        logger(kipl::logging::Logger::LogError,"File selection was cancelled");
        return;
    }

    if ( fileName.isEmpty() )
    {
        logger(kipl::logging::Logger::LogError,"No file selected");
        return;
    }

    logger(kipl::logging::Logger::LogMessage,fileName.toStdString());

    if (fileName.toStdString() == m_ModuleConfig.m_sSharedObject )
    {
        logger(kipl::logging::Logger::LogMessage,"The same library file was selected.");
        return;
    }

    if (UpdateModuleCombobox(fileName)==0)
        m_ModuleConfig.m_sSharedObject=fileName.toStdString();
}

void SingleModuleSettingsDialog::BuildDialog()
{
    this->setLayout(&m_LayoutMain);
    this->setWindowTitle("Select and configure module");
    m_LayoutMain.addLayout(&m_LayoutModule);
    m_LayoutModule.addWidget(&m_LabelModuleName);
    m_LabelModuleName.setText(tr("Module"));
    m_LabelModuleName.setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
    m_LayoutModule.addWidget(&m_ComboModules);
    m_LayoutModule.addWidget(&m_ButtonBrowse);
    m_ButtonBrowse.setText(tr("Browse"));
    m_ButtonBrowse.setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
    m_LayoutMain.addWidget(&m_ParameterListView);
    m_ParameterListView.setColumnCount(2);
    m_ParameterListView.setHeaderLabels(QStringList()<<tr("Parameter")<<tr("Value"));
    m_LayoutMain.addWidget(&m_Buttons);

    m_Buttons.addButton(QDialogButtonBox::Ok);
    m_Buttons.addButton(QDialogButtonBox::Cancel);
}

ModuleConfig SingleModuleSettingsDialog::getModule()
{
    m_ModuleConfig.parameters=GetParameterList();
    return m_ModuleConfig;
}

int SingleModuleSettingsDialog::UpdateModuleCombobox(QString fname, bool bSetFirstIndex)
{
    std::ostringstream msg;

    std::string currentFileName = fname.toStdString();

    m_ModuleList.clear();
    while (m_ModuleList.empty())
    {
        try
        {

            m_ModuleList=GetModuleList(currentFileName);
        }
        catch (ModuleException &e)
        {
            msg<<"Failed to generate module list from "<<fname.toStdString()<<"\n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
        }
        catch (kipl::base::KiplException &e)
        {
            msg<<"Failed to generate module list from "<<fname.toStdString()<<" with a KiplException\n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
        }

        if (m_ModuleList.empty())
        {
            currentFileName = QFileDialog::getOpenFileName(this,"Select a module file",QString::fromStdString(m_sApplicationPath)).toStdString();
            m_ModuleConfig.m_sSharedObject = currentFileName;
        }
    }


    m_ComboModules.clear();
    msg.str("");
    msg<<"The library has "<<m_ModuleList.size()<<" modules";
    logger(kipl::logging::Logger::LogVerbose,msg.str());
    for (auto &module : m_ModuleList)
    {
        m_ComboModules.addItem(QString::fromStdString(module.first));
        msg.str("");
        msg<<"Module "<<module.first<<" has " << module.second.size() << " parmeters.";
        logger.message(msg.str());
    }

    if (bSetFirstIndex)
        m_ComboModules.setCurrentIndex(0);

    return 0;
}

std::map<std::string, std::map<std::string, std::string> > SingleModuleSettingsDialog::GetModuleList(std::string filename)
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
        if (nullptr != fnGetModuleList)
        {

            if (fnGetModuleList(m_sApplication.c_str(),&m_ModuleList)!=0) {
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
    else
    {
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

    return m_ModuleList;
}
void SingleModuleSettingsDialog::on_ComboBox_Changed(QString value)
{
    m_ModuleConfig.m_sModule  = value.toStdString();
    m_ModuleConfig.parameters = m_ModuleList[m_ModuleConfig.m_sModule];

    std::ostringstream msg;

    msg<<"Module"<< m_ModuleConfig.m_sModule << " has " << m_ModuleConfig.parameters.size();
    logger.message(msg.str());
    UpdateCurrentModuleParameters();

}


std::map<std::string, std::string> SingleModuleSettingsDialog::GetParameterList()
{
    std::map<std::string, std::string> parlist;

    for( int i = 0; i < m_ParameterListView.topLevelItemCount(); ++i )
    {
       QTreeWidgetItem *item = m_ParameterListView.topLevelItem( i );
       parlist[item->text(0).toStdString()]=item->text(1).toStdString();
    }

    return parlist;
}

void SingleModuleSettingsDialog::UpdateCurrentModuleParameters()
{
    m_ParameterListView.clear();
    QTreeWidgetItem *parent = m_ParameterListView.invisibleRootItem();
    QTreeWidgetItem *item = nullptr;

    std::ostringstream msg;
    msg<<"Updating parameter list for "<<m_ModuleConfig.m_sModule<<" ("<<m_ModuleConfig.parameters.size()<<" parameters)";
    logger.message(msg.str());
    m_ParameterListView.setColumnWidth(0,this->width()/2);
    if (!m_ModuleConfig.m_sModule.empty())
    {
//        std::map<std::string,std::string>::iterator parameters;

        for (const auto  & parameters : m_ModuleConfig.parameters)
        {
            item=new QTreeWidgetItem(parent);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            item->setText(0,QString::fromStdString(parameters.first));
            item->setText(1,QString::fromStdString(parameters.second));
            msg.str("");
            msg<<parameters.first<<"="<<parameters.second;
            logger.message(msg.str());
        }
    }
}
}

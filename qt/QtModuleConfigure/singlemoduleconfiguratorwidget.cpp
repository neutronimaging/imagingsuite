#include "singlemoduleconfiguratorwidget.h"
#include <iostream>
#include <sstream>
#include <QFileDialog>
#include <QMessageBox>
#include <ModuleException.h>
#include <base/KiplException.h>

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
    logger("SingleModuleConfiguratorWidget")
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

void SingleModuleConfiguratorWidget::Configure(std::string sApplicationName)
{
    m_sApplication=sApplicationName;
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
void SingleModuleConfiguratorWidget::SetModule(ModuleConfig module)
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
    SingleModuleSettingsDialog dlg(m_sApplication,this);

    logger(kipl::logging::Logger::LogMessage,"Clicked configure");
    int res=dlg.exec(m_ModuleConfig);

    if (res==QDialog::Accepted) {
        logger(kipl::logging::Logger::LogMessage,"Accepted");
        m_ModuleConfig=dlg.getModule();
        m_LabelModuleName.setText(QString::fromStdString(m_ModuleConfig.m_sModule));
    }
    else {
            logger(kipl::logging::Logger::LogMessage,"Rejected");
    }
}

SingleModuleSettingsDialog::SingleModuleSettingsDialog(std::string sApplicationName, QWidget *parent) :
    QDialog(parent),
    logger("SingleModuleSettingsDialog"),
    m_sApplication(sApplicationName)
{
    BuildDialog();
    connect(&m_ButtonBrowse,SIGNAL(clicked()),this,SLOT(on_ButtonBrowse_Clicked()));
    connect(&m_Buttons,SIGNAL(clicked(QAbstractButton*)),this,SLOT(on_ButtonBox_Clicked(QAbstractButton*)));

    show();

}

int SingleModuleSettingsDialog::exec(ModuleConfig &config)
{
    return exec();
}

int SingleModuleSettingsDialog::exec()
{ return QDialog::exec(); }

void SingleModuleSettingsDialog::on_ButtonBox_Clicked(QAbstractButton *button)
{
    std::ostringstream msg;
    QDialogButtonBox::StandardButton standardButton = m_Buttons.standardButton(button);
    switch(standardButton) {
        // Standard buttons:
        case QDialogButtonBox::Ok:
            accept();
            break;
        case QDialogButtonBox::Cancel:
            reject();
            break;

        // Non-standard buttons:
        case QDialogButtonBox::NoButton:
           int actionRole = button->property("ActionRole").toInt();
           QString s_Value=button->text();
           msg<<"Got NoButton: "<<actionRole<<", text:"<<button->text().toStdString();

           logger(kipl::logging::Logger::LogMessage,msg.str());

           if (s_Value.toLower()=="add") {
               logger(kipl::logging::Logger::LogMessage,"Add parameter");
           }

           if (s_Value.toLower()=="delete") {
               logger(kipl::logging::Logger::LogMessage,"Delete parameter");
           }

        }
}

void  SingleModuleSettingsDialog::on_ButtonBrowse_Clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),tr("libs (*.dylib)"));

    if (fileName.isEmpty()) {
        logger(kipl::logging::Logger::LogError,"No file selected");
        return;
    }

    if (fileName.toStdString() == m_ModuleConfig.m_sSharedObject ) {
        logger(kipl::logging::Logger::LogMessage,"The same library file was selected.");
        return;
    }

    logger(kipl::logging::Logger::LogMessage,fileName.toStdString());

    if (UpdateModuleCombobox(fileName)!=0)
        return;
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
    m_Buttons.addButton("Delete",QDialogButtonBox::ActionRole);
    m_Buttons.addButton("Add",QDialogButtonBox::ActionRole);

}

int SingleModuleSettingsDialog::UpdateModuleCombobox(QString fname)
{
    std::ostringstream msg;

    try {
        m_ModuleList.clear();
        m_ModuleList=GetModuleList(fname.toStdString());
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

    m_ComboModules.clear();
    std::map<std::string, std::map<std::string, std::string> >::iterator it;
    msg.str("");
    msg<<"The library has "<<m_ModuleList.size()<<" modules";
    logger(kipl::logging::Logger::LogVerbose,msg.str());
    for (it=m_ModuleList.begin(); it!=m_ModuleList.end(); it++) {
        m_ComboModules.addItem(QString::fromStdString(it->first));
    }

    return 0;
}

std::map<std::string, std::map<std::string, std::string> > SingleModuleSettingsDialog::GetModuleList(std::string filename)
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
        msg.str("");
        msg<<"Got functions from "<<filename<<" success="<<(fnGetModuleList == NULL ? "no" : "yes");
        logger(kipl::logging::Logger::LogMessage,msg.str());
         // If the function address is valid, call the function.
        if (NULL != fnGetModuleList)
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

    return m_ModuleList;
}


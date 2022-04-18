//<LICENSE>
#include "singlemodulesettingsdialog.h"
#include "ui_singlemodulesettingsdialog.h"

#include <sstream>
#include <string>

#include <QDir>
#include <QString>
#include <QAbstractButton>
#include <QFileDialog>

#include <ModuleException.h>

#ifdef _MSC_VER // Shared object specific for msvc
typedef int (__cdecl *MODULELIST)(const char *,void *);
#else // Shared object specific for gcc
#include <dlfcn.h>
typedef int (*MODULELIST)(const char *,void *);
typedef  void * HINSTANCE;
#endif

SingleModuleSettingsDialog::SingleModuleSettingsDialog(const std::string &sApplicationName,
                                                       const std::string &sApplicationPath,
                                                       const std::string & sDefaultModuleSource,
                                                       QWidget *parent) :
    QDialog(parent),
    logger("SingleModuleSettingsDialog"),
    ui(new Ui::SingleModuleSettingsDialog),
    m_sApplication(sApplicationName),
    m_sApplicationPath(sApplicationPath),
    m_sDefaultModuleSource(sDefaultModuleSource),
    m_ModuleConfig("")
{
    ui->setupUi(this);
}

SingleModuleSettingsDialog::~SingleModuleSettingsDialog()
{
    delete ui;
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
        msg<<"Index for "<<config.m_sModule
           <<" set to "<<ui->comboBox_Modules->findText(QString::fromStdString(config.m_sModule));
        logger.message(msg.str());

        ui->comboBox_Modules->setCurrentIndex(ui->comboBox_Modules->findText(QString::fromStdString(config.m_sModule)));
        m_ModuleConfig=config;
        UpdateCurrentModuleParameters();
    }
    else
    {
        UpdateModuleCombobox(QString::fromStdString(m_sDefaultModuleSource),false);
    }

    return exec();
}

ModuleConfig SingleModuleSettingsDialog::getModule()
{
    m_ModuleConfig.parameters=GetParameterList();
    return m_ModuleConfig;
}

int SingleModuleSettingsDialog::exec()
{
    QDialog::exec();
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
            logger.error(msg.str());
        }
        catch (kipl::base::KiplException &e)
        {
            msg<<"Failed to generate module list from "<<fname.toStdString()<<" with a KiplException\n"<<e.what();
            logger.error(msg.str());
        }

        if (m_ModuleList.empty())
        {
            currentFileName = QFileDialog::getOpenFileName(this,"Select a module file",QString::fromStdString(m_sApplicationPath)).toStdString();
            m_ModuleConfig.m_sSharedObject = currentFileName;
        }
    }

    ui->comboBox_Modules->clear();
    msg.str("");
    msg<<"The library has "<<m_ModuleList.size()<<" modules";
    logger(kipl::logging::Logger::LogVerbose,msg.str());
    for (auto &module : m_ModuleList)
    {
        ui->comboBox_Modules->addItem(QString::fromStdString(module.first));
        msg.str("");
        msg<<"Module "<<module.first<<" has " << module.second.size() << " parmeters.";
        logger.message(msg.str());
    }

    if (bSetFirstIndex)
        ui->comboBox_Modules->setCurrentIndex(0);

    UpdateCurrentModuleParameters();

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
        logger.message(msg.str());
         // If the function address is valid, call the function.
        if (nullptr != fnGetModuleList)
        {

            if (fnGetModuleList(m_sApplication.c_str(),&m_ModuleList)!=0)
            {
                msg.str("");
                msg<<"Shared object file "<<filename<<" does not contain modules for "<<m_sApplication;
                logger.error(msg.str());
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
        logger.message(msg.str());

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

std::map<std::string, std::string> SingleModuleSettingsDialog::GetParameterList()
{
    std::map<std::string, std::string> parlist;

    for( int i = 0; i < ui->treeWidget_ParameterList->topLevelItemCount(); ++i )
    {
       QTreeWidgetItem *item = ui->treeWidget_ParameterList->topLevelItem( i );
       parlist[item->text(0).toStdString()]=item->text(1).toStdString();
    }

    return parlist;
}

void SingleModuleSettingsDialog::UpdateCurrentModuleParameters()
{
    ui->treeWidget_ParameterList->clear();
    QTreeWidgetItem *parent = ui->treeWidget_ParameterList->invisibleRootItem();
    QTreeWidgetItem *item = nullptr;

    std::ostringstream msg;
    msg<<"Updating parameter list for "<<m_ModuleConfig.m_sModule<<" ("<<m_ModuleConfig.parameters.size()<<" parameters)";
    logger.message(msg.str());
    ui->treeWidget_ParameterList->setColumnWidth(0,this->width()/2);
    if (!m_ModuleConfig.m_sModule.empty())
    {
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

// Slots
void SingleModuleSettingsDialog::on_comboBox_Modules_currentTextChanged(const QString &module_name)
{
    std::ostringstream msg;

    m_ModuleConfig.m_sModule  = module_name.toStdString();
    m_ModuleConfig.parameters = m_ModuleList[m_ModuleConfig.m_sModule];

    msg<<"Module"<< m_ModuleConfig.m_sModule << " has " << m_ModuleConfig.parameters.size();
    logger.message(msg.str());
    UpdateCurrentModuleParameters();
}

void SingleModuleSettingsDialog::on_pushButton_Browse_clicked()
{
    logger(kipl::logging::Logger::LogMessage,"browse");

    QString fileName;
    #ifdef Q_OS_WIN
        QString appPath = QCoreApplication::applicationDirPath()+"\\";
        fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),appPath,tr("libs (*.dll)"));
    #else
        QString appPath = QCoreApplication::applicationDirPath()+"/../Frameworks/";
        fileName = QFileDialog::getOpenFileName(this,tr("Open module library"),appPath,tr("libs (*.dylib | *.so)"));
    #endif
    logger(kipl::logging::Logger::LogMessage,appPath.toStdString());

    if ( fileName.isNull() )
    {
        logger.error("File selection was cancelled");
        return;
    }

    if ( fileName.isEmpty() )
    {
        logger.error("No file selected");
        return;
    }

    logger.message(fileName.toStdString());

    if (fileName.toStdString() == m_ModuleConfig.m_sSharedObject )
    {
        logger(kipl::logging::Logger::LogMessage,"The same library file was selected.");
        return;
    }

    if (UpdateModuleCombobox(fileName)==0)
        m_ModuleConfig.m_sSharedObject=fileName.toStdString();
}

void SingleModuleSettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    std::ostringstream msg;
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);
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

        }
}


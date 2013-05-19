#include "modulechainconfiguratorwidget.h"
#include "AddModuleDialog.h"
#include <QIcon>

ModuleChainConfiguratorWidget::ModuleChainConfiguratorWidget(QWidget *parent) :
    QWidget(parent),
    logger("ModuleChainConfiguratorWidget"),
    m_sApplication("muhrec"),
    m_sApplicationPath("")
{
    this->setLayout(&m_MainBox);
    m_MainBox.addLayout(&m_ModuleBox);
    BuildModuleManager();
    m_MainBox.addLayout(&m_ParameterBox);
    BuildParameterManager();

    connect(&m_ModuleAdd,SIGNAL(clicked()),this,SLOT(on_Button_ModuleAdd()));
    connect(&m_ModuleDelete,SIGNAL(clicked()),this,SLOT(on_Button_ModuleDelete()));
    connect(&m_ModuleConfigure,SIGNAL(clicked()),this,SLOT(on_Button_ConfigureModule()));

    connect(&m_ParameterAdd,SIGNAL(clicked()),this,SLOT(on_Button_ParameterAdd()));
    connect(&m_ParameterDelete,SIGNAL(clicked()),this,SLOT(on_Button_ParameterDelete()));

    show();
}

void ModuleChainConfiguratorWidget::configure(std::string application, std::string applicationpath, ModuleConfigurator *pConfigurator)
{
    m_sApplication     = application;
    m_sApplicationPath = applicationpath;
    m_pConfigurator    = pConfigurator;

}

void ModuleChainConfiguratorWidget::on_Button_ModuleAdd()
{
    ostringstream msg;
    logger(kipl::logging::Logger::LogMessage,"Add module");

    AddModuleDialog dlg(this);

    ModuleConfig mcfg;
    dlg.configure(m_sApplication,m_sApplicationPath);
    if (dlg.exec()==QDialog::Accepted) {
        mcfg=dlg.GetModuleConfig();
        msg.str("");
        msg<<"Got module "<<mcfg.m_sModule<<" from library "<<mcfg.m_sSharedObject;
        logger(kipl::logging::Logger::LogMessage,msg.str());
    }
}

void ModuleChainConfiguratorWidget::on_Button_ModuleDelete()
{
    logger(kipl::logging::Logger::LogMessage,"Delete module");
}

void ModuleChainConfiguratorWidget::on_Button_ConfigureModule()
{
    logger(kipl::logging::Logger::LogMessage,"Configure module");
}

void ModuleChainConfiguratorWidget::on_Button_ParameterAdd()
{
    logger(kipl::logging::Logger::LogMessage,"Add parameter");
}

void ModuleChainConfiguratorWidget::on_Button_ParameterDelete()
{
    logger(kipl::logging::Logger::LogMessage,"Delete parameter");
}
void ModuleChainConfiguratorWidget::on_Selected_Module()
{}

void ModuleChainConfiguratorWidget::SetModules(std::list<ModuleConfig> &modules)
{}

std::list<ModuleConfig> ModuleChainConfiguratorWidget::GetModules()
{
    std::list<ModuleConfig> modulelist;
    QMap<QString, ModuleConfig>::iterator it;

    for (it=m_ModuleList.begin(); it!=m_ModuleList.end(); it++)
        modulelist.push_back(it.value());
    return modulelist;
}

void ModuleChainConfiguratorWidget::SetApplicationObject(ApplicationBase * app )
{}

// Builders
void ModuleChainConfiguratorWidget::BuildModuleManager()
{
    m_ModuleAdd.setText("Add");
    m_ModuleAdd.setToolTip(tr("Add a new module to the list"));
    m_ModuleAdd.setFont(QFont("Helvetic",10));
    m_ModuleDelete.setText("Delete");
    m_ModuleDelete.setToolTip(tr("Delete the selected module from the list"));
    m_ModuleDelete.setFont(QFont("Helvetic",10));
    m_ModuleConfigure.setText("Config");
    m_ModuleConfigure.setToolTip(tr("Open the configure dialog of the selected module"));
    m_ModuleConfigure.setFont(QFont("Helvetic",10));

    m_ModuleBox.addWidget(&m_ModuleListView);
    m_ModuleBox.addLayout(&m_ModuleButtonBox);
    m_ModuleButtonBox.addStretch();
    m_ModuleButtonBox.addWidget(&m_ModuleAdd);
    m_ModuleButtonBox.addWidget(&m_ModuleDelete);
    m_ModuleButtonBox.addWidget(&m_ModuleConfigure);
}

void ModuleChainConfiguratorWidget::BuildParameterManager()
{
    m_ParameterAdd.setText("Add");
    m_ParameterAdd.setToolTip(tr("Add a module parameter"));
    m_ParameterAdd.setFont(QFont("Helvetic",10));
    m_ParameterDelete.setText("Delete");
    m_ParameterDelete.setFont(QFont("Helvetic",10));
    m_ParameterDelete.setToolTip(tr("Delete a module parameter"));

    m_ParameterBox.addWidget(&m_ParameterListView);
    m_ParameterBox.addLayout(&m_ParameterButtonBox);
    m_ParameterButtonBox.addWidget(&m_ParameterAdd);
    m_ParameterButtonBox.addWidget(&m_ParameterDelete);
}

void ModuleChainConfiguratorWidget::UpdateCurrentModuleParameters()
{}
//void ModuleChainConfiguratorWidget::UpdateParameterTable(Gtk::TreeModel::iterator iter);

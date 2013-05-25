#include "modulechainconfiguratorwidget.h"
#include "AddModuleDialog.h"
#include <QIcon>
#include <QListWidgetItem>

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

    AddModuleDialog dlg(this);

    ModuleConfig mcfg;
    dlg.configure(m_sApplication,m_sApplicationPath);
    if (dlg.exec()==QDialog::Accepted) {
        mcfg=dlg.GetModuleConfig();
        msg.str("");
        msg<<"Got module "<<mcfg.m_sModule<<" from library "<<mcfg.m_sSharedObject;
        logger(kipl::logging::Logger::LogMessage,msg.str());
        InsertModuleAfter(mcfg);
    }
}

void ModuleChainConfiguratorWidget::on_Button_ModuleDelete()
{
    ostringstream msg;
    int currentRow=m_ModuleListView.currentRow();
    msg<<"Current row="<<currentRow;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    if (0<=currentRow) {
        msg.str("");
        msg<<"Deleting row "<<currentRow;
        logger(kipl::logging::Logger::LogMessage,msg.str());
        delete m_ModuleListView.takeItem(currentRow);
    }
    else {
        logger(kipl::logging::Logger::LogMessage,"No module selected");
    }

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

void ModuleChainConfiguratorWidget::InsertModuleAfter(ModuleConfig &module)
{
    logger(kipl::logging::Logger::LogMessage,"inserting");
    QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(module.m_sModule));

    item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    item->setCheckState(Qt::Checked); // AND initialize check state
    m_ModuleListView.insertItem(m_ModuleListView.currentRow()+1,item);

}

void ModuleChainConfiguratorWidget::SetApplicationObject(ApplicationBase * app )
{}

// Builders
void ModuleChainConfiguratorWidget::BuildModuleManager()
{
    m_ModuleAdd.setText("Add");
    m_ModuleAdd.setToolTip(tr("Add a new module to the list"));
 //   m_ModuleAdd.setFont(QFont("Helvetic",10));
    m_ModuleDelete.setText("Delete");
    m_ModuleDelete.setToolTip(tr("Delete the selected module from the list"));
 //   m_ModuleDelete.setFont(QFont("Helvetic",10));
    m_ModuleConfigure.setText("Config");
    m_ModuleConfigure.setToolTip(tr("Open the configure dialog of the selected module"));
 //   m_ModuleConfigure.setFont(QFont("Helvetic",10));

    m_ModuleListView.setDragDropMode(QAbstractItemView::DragDrop);
    m_ModuleListView.setDefaultDropAction(Qt::MoveAction);
    m_ModuleListView.setSelectionMode(QAbstractItemView::SingleSelection);

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
//    m_ParameterAdd.setFont(QFont("Helvetic",10));
    m_ParameterDelete.setText("Delete");
//    m_ParameterDelete.setFont(QFont("Helvetic",10));
    m_ParameterDelete.setToolTip(tr("Delete a module parameter"));

    m_ParameterBox.addWidget(&m_ParameterListView);
    m_ParameterBox.addLayout(&m_ParameterButtonBox);
    m_ParameterButtonBox.addWidget(&m_ParameterAdd);
    m_ParameterButtonBox.addWidget(&m_ParameterDelete);
}

void ModuleChainConfiguratorWidget::UpdateCurrentModuleParameters()
{}
//void ModuleChainConfiguratorWidget::UpdateParameterTable(Gtk::TreeModel::iterator iter);

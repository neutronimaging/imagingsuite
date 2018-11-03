//<LICENSE>

#include "modulechainconfiguratorwidget.h"
#include "QListWidgetModuleItem.h"

#include "AddModuleDialog.h"
#include <QIcon>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <list>
#include <string>
#include <ModuleException.h>



ModuleChainConfiguratorWidget::ModuleChainConfiguratorWidget(QWidget *parent) :
    QWidget(parent),
    logger("ModuleChainConfiguratorWidget"),
    m_pCurrentModule(nullptr),
    m_sApplication("muhrec"),
    m_sApplicationPath(""),
    m_pApplication(nullptr),
    m_pConfigurator(nullptr)
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

    connect(&m_ModuleListView,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(on_Selected_Module(QListWidgetItem*,QListWidgetItem*)));

    show();
}

void ModuleChainConfiguratorWidget::configure(std::string application, std::string applicationpath, ModuleConfigurator *pConfigurator)
{
    m_sApplication     = application;
    m_sApplicationPath = applicationpath;
    m_pConfigurator    = pConfigurator;

//    std:cout << m_sApplication << std::endl;
//    std::cout << m_sApplicationPath << std::endl;

}

QSize ModuleChainConfiguratorWidget::minimumSizeHint() const
{
    return QSize(400, 150);
}

QSize ModuleChainConfiguratorWidget::sizeHint() const
{
    return QSize(450, 150);
}

void ModuleChainConfiguratorWidget::on_Button_ModuleAdd()
{
    ostringstream msg;

    AddModuleDialog dlg(this);

    ModuleConfig mcfg;
//    msg.str(""); msg<<"Add:Preconf: "<<m_sDefaultModuleSource;
//    logger(kipl::logging::Logger::LogMessage,msg.str());

    dlg.configure(m_sApplication,m_sDefaultModuleSource,m_sApplicationPath);
    if (dlg.exec()==QDialog::Accepted) {
        mcfg=dlg.GetModuleConfig();
        msg.str("");
        msg<<"Got module "<<mcfg.m_sModule<<" from library "<<mcfg.m_sSharedObject;
        logger(kipl::logging::Logger::LogMessage,msg.str());
        InsertModuleAfter(mcfg);
    }
    m_pCurrentModule=m_ModuleListView.currentItem();
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

    UpdateCurrentModuleParameters();

}

void ModuleChainConfiguratorWidget::on_Button_ConfigureModule()
{

    if (m_pConfigurator!=nullptr) {
        if (m_pCurrentModule!=nullptr) {
            std::ostringstream msg;
            QListWidgetModuleItem *pCurrentModule=dynamic_cast<QListWidgetModuleItem *>(m_pCurrentModule);
            //UpdateCurrentModuleParameters();

            m_pApplication->UpdateConfig();
            std::string modulename = pCurrentModule->m_Module.m_sModule;
            std::string soname     = pCurrentModule->m_Module.m_sSharedObject;
            size_t      pos        = soname.find_last_of('.');
            pos = soname.find_last_not_of("0123456789.",pos);
            std::string guisoname  = soname.substr(0,pos+1)+"GUI"+soname.substr(pos+1);

            msg.str(""); msg<<"Configuring "<<modulename<<" from "<<soname<<" with "<<guisoname<<std::endl;
            logger(kipl::logging::Logger::LogMessage,msg.str());

            try {
                std::map<std::string, std::string> parameters=GetParameterList();

                int res=m_pConfigurator->configure(m_sApplication,guisoname,modulename,parameters);
                if (res==QDialog::Accepted) {
                    logger(kipl::logging::Logger::LogMessage,"using parameters");
                    pCurrentModule->m_Module.parameters=parameters;
                    UpdateCurrentModuleParameters();
                }
            }
            catch (ModuleException &e) {
                msg.str("");
                msg<<"Failed to open dialog: "<<e.what();
                logger(kipl::logging::Logger::LogWarning,msg.str());
            }
        }
        else {
            logger.warning("Current module variable void.");
        }
    }
}

void ModuleChainConfiguratorWidget::on_Button_ParameterAdd()
{
    logger(kipl::logging::Logger::LogMessage,"Add parameter");

    if (m_ModuleListView.currentItem()!=NULL) {
        QTreeWidgetItem *parent = m_ParameterListView.invisibleRootItem();
        QTreeWidgetItem *item = NULL;

        item=new QTreeWidgetItem(parent);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setText(0,"NewParameter");
        item->setText(1,"Value");
    }
}

void ModuleChainConfiguratorWidget::on_Button_ParameterDelete()
{
    logger(kipl::logging::Logger::LogMessage,"Delete parameter");
    QTreeWidgetItem *item=m_ParameterListView.currentItem();
    if (item) {
        delete m_ParameterListView.takeTopLevelItem(m_ParameterListView.indexOfTopLevelItem(item));
    }
}

void ModuleChainConfiguratorWidget::on_Selected_Module(QListWidgetItem* current,QListWidgetItem* previous)
{
    if (previous!=NULL) {
        dynamic_cast<QListWidgetModuleItem *>(previous)->m_Module.parameters=GetParameterList();
        m_pCurrentModule=current;
    }
    UpdateCurrentModuleParameters();
}

void ModuleChainConfiguratorWidget::SetModules(std::list<ModuleConfig> &modules)
{
  std::list<ModuleConfig>::iterator it;

  m_ModuleListView.clear();

  for (it=modules.begin(); it!=modules.end(); it++) {
    InsertModuleAfter(*it);
  }
}

std::list<ModuleConfig> ModuleChainConfiguratorWidget::GetModules()
{
    std::list<ModuleConfig> modulelist;

    QListWidgetModuleItem *item=dynamic_cast<QListWidgetModuleItem *>(m_ModuleListView.currentItem());
    if (item!=nullptr)
        item->m_Module.parameters=GetParameterList();

    for (int i=0; i<m_ModuleListView.count(); i++) {
        item=dynamic_cast<QListWidgetModuleItem *>(m_ModuleListView.item(i));

        item->m_Module.m_bActive=item->checkState();
        modulelist.push_back(item->m_Module);
    }
    return modulelist;
}

void ModuleChainConfiguratorWidget::InsertModuleAfter(ModuleConfig &module)
{
    QListWidgetItem *item = new QListWidgetModuleItem(module);

    item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    item->setCheckState(module.m_bActive ? Qt::Checked : Qt::Unchecked); // AND initialize check state

    m_ModuleListView.insertItem(m_ModuleListView.currentRow()+1,item);
    m_ModuleListView.setCurrentItem(item);

    UpdateCurrentModuleParameters();
    m_pCurrentModule=item;
}

void ModuleChainConfiguratorWidget::SetApplicationObject(ApplicationBase * app )
{
    m_pApplication=app;
}

void ModuleChainConfiguratorWidget::SetDefaultModuleSource(string file)
{
    m_sDefaultModuleSource = file;
}

// Builders
void ModuleChainConfiguratorWidget::BuildModuleManager()
{
    m_ModuleAdd.setText(tr("Add"));
    m_ModuleAdd.setToolTip(tr("Add a new module to the list"));

    m_ModuleDelete.setText(tr("Delete"));
    m_ModuleDelete.setToolTip(tr("Delete the selected module from the list"));
    m_ModuleConfigure.setText(tr("Config"));
    m_ModuleConfigure.setToolTip(tr("Open the configure dialog of the selected module"));

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
    m_ParameterAdd.setText(tr("Add"));
    m_ParameterAdd.setToolTip(tr("Add a module parameter"));
    m_ParameterDelete.setText(tr("Delete"));
    m_ParameterDelete.setToolTip(tr("Delete a module parameter"));

    m_ParameterBox.addWidget(&m_ParameterListView);
    m_ParameterBox.addLayout(&m_ParameterButtonBox);
    m_ParameterButtonBox.addWidget(&m_ParameterAdd);
    m_ParameterButtonBox.addWidget(&m_ParameterDelete);
    m_ParameterListView.setColumnCount(2);
    m_ParameterListView.setHeaderLabels(QStringList()<<tr("Parameter")<<tr("Value"));
}

void ModuleChainConfiguratorWidget::UpdateCurrentModuleParameters()
{
    m_ParameterListView.clear();

    QListWidgetModuleItem *moduleitem=dynamic_cast<QListWidgetModuleItem *>(m_ModuleListView.currentItem());

    QTreeWidgetItem *parent = m_ParameterListView.invisibleRootItem();
    QTreeWidgetItem *item = nullptr;

    if (moduleitem!=nullptr) {
        std::map<std::string,std::string>::iterator it;

        for (it=moduleitem->m_Module.parameters.begin();
             it!=moduleitem->m_Module.parameters.end(); it++) {
            item=new QTreeWidgetItem(parent);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            item->setText(0,QString::fromStdString(it->first));
            item->setText(1,QString::fromStdString(it->second));

        }
    }

    m_ParameterListView.resizeColumnToContents(0);
}

std::map<std::string, std::string> ModuleChainConfiguratorWidget::GetParameterList()
{
    std::map<std::string, std::string> parlist;

    for( int i = 0; i < m_ParameterListView.topLevelItemCount(); ++i )
    {
       QTreeWidgetItem *item = m_ParameterListView.topLevelItem( i );
       parlist[item->text(0).toStdString()]=item->text(1).toStdString();
    }

    return parlist;
}


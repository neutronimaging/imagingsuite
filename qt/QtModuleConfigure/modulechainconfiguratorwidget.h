#ifndef MODULECHAINCONFIGURATORWIDGET_H
#define MODULECHAINCONFIGURATORWIDGET_H

#include <QWidget>
#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTreeWidget>

#include <QPushButton>

#include <logging/logger.h>
#include <ModuleConfig.h>
#include "ModuleConfigurator.h"
#include "ApplicationBase.h"

#include <map>
#include <string>

class ModuleChainConfiguratorWidget : public QWidget
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ModuleChainConfiguratorWidget(QWidget *parent = 0);
    void configure(std::string application, std::string applicationpath="", ModuleConfigurator *pConfigurator=NULL);
    std::list<ModuleConfig> GetModules();
    void SetModules(std::list<ModuleConfig> &modules);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
signals:
    
public slots:
    virtual void on_Button_ModuleAdd();
    virtual void on_Button_ModuleDelete();
    virtual void on_Button_ConfigureModule();
    virtual void on_Button_ParameterAdd();
    virtual void on_Button_ParameterDelete();
    virtual void on_Selected_Module(QListWidgetItem* current,QListWidgetItem* previous);

protected:
    void SetApplicationObject(ApplicationBase * app );

    // Builders
    void BuildModuleManager();
    void BuildParameterManager();
    void UpdateCurrentModuleParameters();
    void InsertModuleAfter(ModuleConfig &module);
    std::map<std::string, std::string> GetParameterList();

    QHBoxLayout m_MainBox;
    QVBoxLayout m_ModuleBox;
    QListWidget m_ModuleListView;
    QHBoxLayout m_ModuleButtonBox;
    QPushButton m_ModuleAdd;
    QPushButton m_ModuleDelete;
    QPushButton m_ModuleConfigure;

    QVBoxLayout m_ParameterBox;
    QTreeWidget m_ParameterListView;
    QHBoxLayout m_ParameterButtonBox;
    QPushButton m_ParameterAdd;
    QPushButton m_ParameterDelete;

    QListWidgetItem *m_pCurrentModule;


    std::string m_sApplication;
    std::string m_sApplicationPath;
    ApplicationBase * m_pApplication ;
    ModuleConfigurator * m_pConfigurator;
};

#endif // MODULECHAINCONFIGURATORWIDGET_H

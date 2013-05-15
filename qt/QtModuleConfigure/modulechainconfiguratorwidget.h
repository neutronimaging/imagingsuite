#ifndef MODULECHAINCONFIGURATORWIDGET_H
#define MODULECHAINCONFIGURATORWIDGET_H

#include <QWidget>
#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QPushButton>

#include <logging/logger.h>
#include <ModuleConfig.h>
#include "ModuleConfigurator.h"
#include "ApplicationBase.h"

#include <list>

class ModuleChainConfiguratorWidget : public QWidget
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ModuleChainConfiguratorWidget(QWidget *parent = 0);
    void configure(std::string application, std::string applicationpath="", ModuleConfigurator *pConfigurator=NULL);
    
signals:
    
public slots:
    virtual void on_Button_ModuleAdd();
    virtual void on_Button_ModuleDelete();
    virtual void on_Button_ConfigureModule();
    virtual void on_Button_ParameterAdd();
    virtual void on_Button_ParameterDelete();
    virtual void on_Selected_Module();

protected:
    void SetModules(std::list<ModuleConfig> &modules);
    std::list<ModuleConfig> GetModules();
    void SetApplicationObject(ApplicationBase * app );

    // Builders
    void BuildModuleManager();
    void BuildParameterManager();
    void UpdateCurrentModuleParameters();
//	void UpdateParameterTable(Gtk::TreeModel::iterator iter);



//	//Tree model modules:
//	class ModelColumns : public Gtk::TreeModel::ColumnRecord
//	{
//	public:
//		ModelColumns()
//		{ add(m_col_id); add(m_col_active); add(m_col_name); add(m_col_shared_object); add(m_col_parameters); }

//		Gtk::TreeModelColumn<unsigned int> m_col_id;
//		Gtk::TreeModelColumn<bool> m_col_active;
//		Gtk::TreeModelColumn<Glib::ustring> m_col_name;
//		Gtk::TreeModelColumn<Glib::ustring> m_col_shared_object;
//		Gtk::TreeModelColumn<std::map<std::string, std::string> > m_col_parameters;

//	};

//	//Tree model parameters:
//	class ParameterColumns : public Gtk::TreeModel::ColumnRecord
//	{
//	public:
//		ParameterColumns()
//		{ add(m_col_id); add(m_col_name); add(m_col_value); }

//		Gtk::TreeModelColumn<unsigned int> m_col_id;;
//		Gtk::TreeModelColumn<Glib::ustring> m_col_name;
//		Gtk::TreeModelColumn<Glib::ustring> m_col_value;
//	};

    QHBoxLayout m_MainBox;
    QVBoxLayout m_ModuleBox;
    QListView   m_ModuleListView;
    QHBoxLayout m_ModuleButtonBox;
    QPushButton m_ModuleAdd;
    QPushButton m_ModuleDelete;
    QPushButton m_ModuleConfigure;

    QVBoxLayout m_ParameterBox;
    QListView   m_ParameterListView;
    QHBoxLayout m_ParameterButtonBox;
    QPushButton m_ParameterAdd;
    QPushButton m_ParameterDelete;


//	ModelColumns m_ModuleColumns;
//	ParameterColumns m_ParameterColumns;

//	Gtk::HButtonBox m_ModuleButtonBox;
//	Gtk::Button m_Button_ModuleAdd;
//	Gtk::Button m_Button_ModuleDelete;
//	Gtk::Button m_Button_ConfigureModule;


//	Gtk::ScrolledWindow m_ModuleScrolledWindow;
//	Gtk::TreeView m_ModuleTreeView;
//	Glib::RefPtr<Gtk::ListStore> m_refModuleTreeModel;

//	Gtk::HButtonBox m_ParameterButtonBox;
//	Gtk::Button m_Button_ParameterAdd;
//	Gtk::Button m_Button_ParameterDelete;

//	Gtk::ScrolledWindow m_ParameterScrolledWindow;
//	Gtk::TreeView m_ParameterTreeView;
//	Glib::RefPtr<Gtk::ListStore> m_refParameterTreeModel;

//	Gtk::TreeModel::iterator m_CurrentRow;
    std::string m_sApplication;
    std::string m_sApplicationPath;
    ApplicationBase * m_pApplication ;
    ModuleConfigurator * m_pConfigurator;
    QMap<QString, ModuleConfig> m_ModuleList;

};

#endif // MODULECHAINCONFIGURATORWIDGET_H

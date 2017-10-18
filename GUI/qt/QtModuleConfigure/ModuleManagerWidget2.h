//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-11-11 13:54:35 +0100 (Sun, 11 Nov 2012) $
// $Rev: 1400 $
// $Id: ModuleManagerWidget2.h 1400 2012-11-11 12:54:35Z kaestner $
//


#ifndef __MODULEMANAGERWIDGET_H
#define __MODULEMANAGERWIDGET_H

#include "../src/stdafx.h"
#include <gtkmm.h>
#include <map>
#include <list>
#include <string>
#include <ModuleConfig.h>
#include "AddModuleDialog.h"
#include "ModuleConfigurator.h"
#include "ApplicationBase.h"

class DLL_EXPORT ModuleManagerWidget2 : public Gtk::HPaned
{
	kipl::logging::Logger logger;
public:
	ModuleManagerWidget2(std::string application, std::string applicationpath="", ModuleConfigurator *pConfigurator=NULL);
	virtual ~ModuleManagerWidget2(void);

	void SetModules(std::list<ModuleConfig> &modules);
	std::list<ModuleConfig> GetModules();
	void SetApplicationObject(ApplicationBase * app );
//	Glib::SignalProxy0<void> signal_module_selected();

protected:
	// Builders
	void BuildModuleManager();
	void BuildParameterManager();
	void UpdateCurrentModuleParameters();
	void UpdateParameterTable(Gtk::TreeModel::iterator iter);

	//Signal handlers:
	virtual void on_Button_ModuleAdd();
	virtual void on_Button_ModuleDelete();
	virtual void on_Button_ConfigureModule();
	virtual void on_Button_ParameterAdd();
	virtual void on_Button_ParameterDelete();
	virtual void on_Selected_Module();
	
	//Tree model modules:
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns()
		{ add(m_col_id); add(m_col_active); add(m_col_name); add(m_col_shared_object); add(m_col_parameters); }

		Gtk::TreeModelColumn<unsigned int> m_col_id;
		Gtk::TreeModelColumn<bool> m_col_active;
		Gtk::TreeModelColumn<Glib::ustring> m_col_name;
		Gtk::TreeModelColumn<Glib::ustring> m_col_shared_object;
		Gtk::TreeModelColumn<std::map<std::string, std::string> > m_col_parameters;

	};

	//Tree model parameters:
	class ParameterColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ParameterColumns()
		{ add(m_col_id); add(m_col_name); add(m_col_value); }

		Gtk::TreeModelColumn<unsigned int> m_col_id;;
		Gtk::TreeModelColumn<Glib::ustring> m_col_name;
		Gtk::TreeModelColumn<Glib::ustring> m_col_value;
	};

	Gtk::VBox m_ModuleBox;
	Gtk::VBox m_ParameterBox;

	ModelColumns m_ModuleColumns;
	ParameterColumns m_ParameterColumns;

	Gtk::HButtonBox m_ModuleButtonBox;
	Gtk::Button m_Button_ModuleAdd;
	Gtk::Button m_Button_ModuleDelete;
	Gtk::Button m_Button_ConfigureModule;


	Gtk::ScrolledWindow m_ModuleScrolledWindow;
	Gtk::TreeView m_ModuleTreeView;
	Glib::RefPtr<Gtk::ListStore> m_refModuleTreeModel;

	Gtk::HButtonBox m_ParameterButtonBox;
	Gtk::Button m_Button_ParameterAdd;
	Gtk::Button m_Button_ParameterDelete;

	Gtk::ScrolledWindow m_ParameterScrolledWindow;
	Gtk::TreeView m_ParameterTreeView;
	Glib::RefPtr<Gtk::ListStore> m_refParameterTreeModel;

	Gtk::TreeModel::iterator m_CurrentRow;
	std::string m_sApplication;
	std::string m_sApplicationPath;
	ApplicationBase * m_pApplication ;
	ModuleConfigurator * m_pConfigurator;

};

#endif


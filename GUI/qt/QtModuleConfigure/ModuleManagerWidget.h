//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:24:22 +0200 (Mon, 21 May 2012) $
// $Rev: 1318 $
// $Id: ModuleManagerWidget.h 1318 2012-05-21 14:24:22Z kaestner $
//


#ifndef __MODULEMANAGERWIDGET_H
#define __MODULEMANAGERWIDGET_H

#include <gtkmm.h>
#include <map>
#include <list>
#include <string>
#include <ModuleConfig.h>
#include "AddModuleDialog.h"

class ModuleManagerWidget :
	public Gtk::VBox
{
public:
	ModuleManagerWidget();
	virtual ~ModuleManagerWidget(void);

	void SetModules(std::list<ModuleConfig> &modules);
	std::list<ModuleConfig> GetModules();

protected:
	//Signal handlers:
	virtual void on_Button_Add();
	virtual void on_Button_Delete();
	virtual void on_Button_Properties();
	
	//Tree model columns:
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

	ModelColumns m_Columns;

	Gtk::HButtonBox m_ButtonBox;
	Gtk::Button m_Button_Add;
	Gtk::Button m_Button_Delete;
	Gtk::Button m_Button_Properties;

	Gtk::ScrolledWindow m_ScrolledWindow;
	Gtk::TreeView m_TreeView;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModel;


};

#endif


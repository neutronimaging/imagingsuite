//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:24:22 +0200 (Mon, 21 May 2012) $
// $Rev: 1318 $
// $Id: ModuleManagerWidget.cpp 1318 2012-05-21 14:24:22Z kaestner $
//


#include "stdafx.h"
#undef max
#include "../include/ModuleManagerWidget.h"
#include "../include/ParameterManagerWidget.h"
#include "../include/AddModuleDialog.h"

#include <iostream>
#include <sstream>

ModuleManagerWidget::ModuleManagerWidget() :
Glib::ObjectBase("ModuleManagerWidget"),
m_Button_Add(Gtk::Stock::ADD),
m_Button_Delete(Gtk::Stock::DELETE), 
m_Button_Properties(Gtk::Stock::PROPERTIES)
{
  m_ScrolledWindow.add(m_TreeView);

  //Only show the scrollbars when they are necessary:
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  pack_start(m_ScrolledWindow);
  
  pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

  m_ButtonBox.pack_start(m_Button_Properties, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_Add, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_Delete, Gtk::PACK_SHRINK);
  
  m_ButtonBox.set_border_width(5);
  m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
  m_Button_Add.signal_clicked().connect( sigc::mem_fun(*this,
              &ModuleManagerWidget::on_Button_Add) );
  m_Button_Delete.signal_clicked().connect( sigc::mem_fun(*this,
              &ModuleManagerWidget::on_Button_Delete) );
  m_Button_Properties.signal_clicked().connect( sigc::mem_fun(*this,
              &ModuleManagerWidget::on_Button_Properties) );
  
  //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);

  //Fill the TreeView's model
  //std::map<std::string, std::string> parameters;
  //Gtk::TreeModel::Row row = *(m_refTreeModel->append());
  //row[m_Columns.m_col_id] = 1;
  //row[m_Columns.m_col_active] = true;
  //row[m_Columns.m_col_name] = "LogNorm";
  //row[m_Columns.m_col_parameters] = parameters;

  //row = *(m_refTreeModel->append());
  //row[m_Columns.m_col_id] = 2;
  //row[m_Columns.m_col_name] = "Filter";
  //row[m_Columns.m_col_active] = true;

  //parameters["Parameter1"]="1.0";
  //parameters["Parameter2"]="3.14";
  //row[m_Columns.m_col_parameters] = parameters;

  //Add the TreeView's view columns:
  //We use the *_editable convenience methods for most of these,
  //because the default functionality is enoughu
  m_TreeView.set_reorderable(true);
 // m_TreeView.append_column("ID", m_Columns.m_col_id);
  m_TreeView.append_column_editable("Active", m_Columns.m_col_active);
  m_TreeView.append_column("Module name", m_Columns.m_col_name);
}

ModuleManagerWidget::~ModuleManagerWidget(void)
{
}

void ModuleManagerWidget::on_Button_Add()
{
	ModuleConfig module;
	AddModuleDialog add_module_dlg("");
	int res=add_module_dlg.run("",module);

	if (res==Gtk::RESPONSE_APPLY) { 
		Gtk::TreeModel::Row row = *(m_refTreeModel->append());
		row[m_Columns.m_col_id] = 0;
		row[m_Columns.m_col_active] = true;
		row[m_Columns.m_col_name] = module.m_sModule;
		row[m_Columns.m_col_shared_object] = module.m_sSharedObject;
		row[m_Columns.m_col_parameters] = module.parameters;
	}
}

void ModuleManagerWidget::on_Button_Delete()
{
	Glib::RefPtr<Gtk::TreeSelection> selection=m_TreeView.get_selection();

	Gtk::TreeModel::iterator iter = selection->get_selected();
	if(iter) //If anything is selected
	{
	//	Gtk::TreeModel::Row row = *iter;
		m_refTreeModel->erase(iter);
	}
}

void ModuleManagerWidget::on_Button_Properties() 
{
	std::ostringstream msg;
	Glib::RefPtr<Gtk::TreeSelection> selection=m_TreeView.get_selection();

	Gtk::TreeModel::iterator iter = selection->get_selected();
	if(iter) 
	{
		Gtk::TreeModel::Row row = *iter;
		std::map<std::string, std::string> parameters=row[m_Columns.m_col_parameters];
		if (!parameters.empty()) {
			Gtk::Dialog dlg;
			
			dlg.add_button(Gtk::Stock::OK,Gtk::RESPONSE_OK);
			dlg.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
			ParameterManagerWidget pm;
			
			pm.SetParameters(parameters);
			dlg.get_vbox()->pack_start(pm);
			dlg.show_all_children();
			msg<<"Edit parameters for "<<row[m_Columns.m_col_name];
			dlg.set_title(msg.str());
			gint response=dlg.run();

			if (response==Gtk::RESPONSE_OK) {
				parameters=pm.GetParameters();
				row[m_Columns.m_col_parameters]=parameters;
			}
		}
		else {
			msg<<row[m_Columns.m_col_name]<<" has no parameters.";
			Gtk::MessageDialog dlg(msg.str());
			dlg.run();	
		}
	}
	else 
	{
		Gtk::MessageDialog dlg("Please select a module");
		dlg.run();
	}
}

void ModuleManagerWidget::SetModules(std::list<ModuleConfig> &modules)
{
	m_refTreeModel->clear();
	Gtk::TreeModel::Row row;
	
	std::list<ModuleConfig>::iterator it;
	int i=0;
	for (it=modules.begin(); it!=modules.end(); it++) {
		row = *(m_refTreeModel->append());
		row[m_Columns.m_col_id] = i++;
		row[m_Columns.m_col_active] = it->m_bActive;
		row[m_Columns.m_col_name] = it->m_sModule;
		row[m_Columns.m_col_shared_object] = it->m_sSharedObject;
		row[m_Columns.m_col_parameters] = it->parameters;
	}
}

std::list<ModuleConfig> ModuleManagerWidget::GetModules()
{
	std::list<ModuleConfig> modules;

	typedef Gtk::TreeModel::Children type_children; //minimise code length.
	type_children children = m_refTreeModel->children();
	for(type_children::iterator iter = children.begin();
		iter != children.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *iter;

		ModuleConfig module;
		Glib::ustring str;
		module.m_bActive=row[m_Columns.m_col_active];
		
		str=row[m_Columns.m_col_name]; module.m_sModule=str;
		str=row[m_Columns.m_col_shared_object]; module.m_sSharedObject=str;
		module.parameters=row[m_Columns.m_col_parameters];

		modules.push_back(module);
	}

	return modules;
}

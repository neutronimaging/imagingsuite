//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-12-16 19:59:43 +0100 (Sun, 16 Dec 2012) $
// $Rev: 1414 $
// $Id: ModuleManagerWidget2.cpp 1414 2012-12-16 18:59:43Z kaestner $
//

#include "stdafx.h"
#undef max
#include "../include/ModuleManagerWidget2.h"
#include <gtkmm.h>
#include <iostream>
#include <sstream>
#include "../include/ModuleConfigurator.h"
#include <ModuleException.h>

ModuleManagerWidget2::ModuleManagerWidget2(std::string application, std::string applicationpath, ModuleConfigurator *pConfigurator) :
Glib::ObjectBase("ModuleManagerWidget2"),
logger("ModuleManagerWidget2"),
m_Button_ModuleAdd(Gtk::Stock::ADD),
m_Button_ModuleDelete(Gtk::Stock::DELETE),
m_Button_ConfigureModule(Gtk::Stock::PROPERTIES),
m_Button_ParameterAdd(Gtk::Stock::ADD),
m_Button_ParameterDelete(Gtk::Stock::DELETE),
m_CurrentRow(NULL),
m_sApplication(application),
m_sApplicationPath(applicationpath),
m_pApplication(NULL),
m_pConfigurator(pConfigurator)
{
	BuildModuleManager();
	BuildParameterManager();
	add1(m_ModuleBox);
	add2(m_ParameterBox);
	show_all_children();
	std::ostringstream msg;
	msg<<"Managing application: "<<m_sApplication<<" located at "<<m_sApplicationPath;
	logger(kipl::logging::Logger::LogMessage,msg.str());
}

void ModuleManagerWidget2::SetApplicationObject(ApplicationBase * app )
{
	m_pApplication=app;
}

void ModuleManagerWidget2::BuildModuleManager() 
{
  m_ModuleScrolledWindow.add(m_ModuleTreeView);

  //Only show the scrollbars when they are necessary:
  m_ModuleScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_ModuleBox.pack_start(m_ModuleScrolledWindow);
  m_ModuleBox.pack_start(m_ModuleButtonBox, Gtk::PACK_SHRINK);

  m_ModuleButtonBox.pack_start(m_Button_ModuleAdd, Gtk::PACK_SHRINK);
  m_ModuleButtonBox.pack_start(m_Button_ModuleDelete, Gtk::PACK_SHRINK);
  if (m_pConfigurator!=NULL)
	  m_ModuleButtonBox.pack_start(m_Button_ConfigureModule, Gtk::PACK_SHRINK);
  
  m_ModuleButtonBox.set_border_width(5);
  m_ModuleButtonBox.set_layout(Gtk::BUTTONBOX_END);
  m_Button_ModuleAdd.signal_clicked().connect( sigc::mem_fun(*this,
              &ModuleManagerWidget2::on_Button_ModuleAdd) );
  m_Button_ModuleDelete.signal_clicked().connect( sigc::mem_fun(*this,
              &ModuleManagerWidget2::on_Button_ModuleDelete) );
  m_Button_ConfigureModule.signal_clicked().connect( sigc::mem_fun(*this,
              &ModuleManagerWidget2::on_Button_ConfigureModule) );
  //Create the Tree model:
  m_refModuleTreeModel = Gtk::ListStore::create(m_ModuleColumns);
  m_ModuleTreeView.set_model(m_refModuleTreeModel);
  m_ModuleTreeView.get_selection()->signal_changed().connect(sigc::mem_fun(*this,
              &ModuleManagerWidget2::on_Selected_Module));

  //Add the TreeView's view columns:
  //We use the *_editable convenience methods for most of these,
  //because the default functionality is enoughu
  m_ModuleTreeView.set_reorderable(true);
  m_ModuleTreeView.append_column_editable("Active", m_ModuleColumns.m_col_active);
  m_ModuleTreeView.append_column("Module name", m_ModuleColumns.m_col_name);
}

void ModuleManagerWidget2::BuildParameterManager() 
{
  m_ParameterScrolledWindow.add(m_ParameterTreeView);

  //Only show the scrollbars when they are necessary:
  m_ParameterScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_ParameterBox.pack_start(m_ParameterScrolledWindow);
  m_ParameterBox.pack_start(m_ParameterButtonBox, Gtk::PACK_SHRINK);

  m_ParameterButtonBox.pack_start(m_Button_ParameterAdd, Gtk::PACK_SHRINK);
  m_ParameterButtonBox.pack_start(m_Button_ParameterDelete, Gtk::PACK_SHRINK);
  
  m_ParameterButtonBox.set_border_width(5);
  m_ParameterButtonBox.set_layout(Gtk::BUTTONBOX_END);
  m_Button_ParameterAdd.signal_clicked().connect( sigc::mem_fun(*this,
              &ModuleManagerWidget2::on_Button_ParameterAdd) );
  m_Button_ParameterDelete.signal_clicked().connect( sigc::mem_fun(*this,
              &ModuleManagerWidget2::on_Button_ParameterDelete) );
  //Create the Tree model:
  m_refParameterTreeModel = Gtk::ListStore::create(m_ParameterColumns);
  m_ParameterTreeView.set_model(m_refParameterTreeModel);

  //Add the TreeView's view columns:
  //We use the *_editable convenience methods for most of these,
  //because the default functionality is enoughu
  m_ParameterTreeView.set_reorderable(true);
  m_ParameterTreeView.append_column_editable("Parameter", m_ParameterColumns.m_col_name);
  m_ParameterTreeView.append_column_editable("Value", m_ParameterColumns.m_col_value);
}

ModuleManagerWidget2::~ModuleManagerWidget2(void)
{
}

void ModuleManagerWidget2::on_Selected_Module()
{
	UpdateCurrentModuleParameters();

	Glib::RefPtr<Gtk::TreeSelection> selection = m_ModuleTreeView.get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	
	UpdateParameterTable(iter);

	m_CurrentRow=iter;
	Gtk::TreeModel::iterator it;
}

void ModuleManagerWidget2::UpdateParameterTable(Gtk::TreeModel::iterator iter)
{
	if(iter) 
	{
		Gtk::TreeModel::Row row = *iter;

		std::map<std::string, std::string> parameters=row[m_ModuleColumns.m_col_parameters];

		m_refParameterTreeModel->clear();
		std::map<std::string,std::string>::iterator it;

		for (it=parameters.begin(); it!=parameters.end(); it++) {
			row=*(m_refParameterTreeModel->append());
			row[m_ParameterColumns.m_col_name]  = it->first;
			row[m_ParameterColumns.m_col_value] = it->second;
		}
	}
}
void ModuleManagerWidget2::UpdateCurrentModuleParameters()
{
	if (m_CurrentRow!=NULL) {
		typedef Gtk::TreeModel::Children type_children; //minimise code length.
		type_children children = m_refParameterTreeModel->children();
		std::map<std::string, std::string> parameters;
		for(type_children::iterator iter = children.begin();
			iter != children.end(); ++iter)
		{

			Gtk::TreeModel::Row row = *iter;
			Glib::ustring name=row[m_ParameterColumns.m_col_name];
			Glib::ustring value=row[m_ParameterColumns.m_col_value];
			std::cout<<name<<" "<<value<<"\n" ;
			parameters[name]=value;
		}
		(*m_CurrentRow)[m_ModuleColumns.m_col_parameters]=parameters;
	}
}

void ModuleManagerWidget2::on_Button_ConfigureModule()
{
	std::ostringstream msg;

	if (m_pConfigurator!=NULL) {
		if (m_CurrentRow!=NULL) {
			UpdateCurrentModuleParameters();
			m_pApplication->UpdateConfig();
			Glib::ustring modulename=(*m_CurrentRow)[m_ModuleColumns.m_col_name];
			Glib::ustring soname=(*m_CurrentRow)[m_ModuleColumns.m_col_shared_object];
			size_t pos=soname.find_last_of('.');
			std::string guisoname=soname.substr(0,pos)+"GUI"+soname.substr(pos);

			msg.str(""); msg<<"Configuring "<<modulename<<" from "<<soname<<" with "<<guisoname<<std::endl;
			logger(kipl::logging::Logger::LogMessage,msg.str());

			try {
				std::map<std::string, std::string> parameters=(*m_CurrentRow)[m_ModuleColumns.m_col_parameters];

				bool res=m_pConfigurator->configure(m_sApplication,guisoname,modulename,parameters);
				if (res==true) {
					logger(kipl::logging::Logger::LogMessage,"using parameters");
					(*m_CurrentRow)[m_ModuleColumns.m_col_parameters]=parameters;
					UpdateParameterTable(m_CurrentRow);
				}
			}
			catch (ModuleException &e) {
				msg.str("");
				msg<<"Failed to open dialog: "<<e.what();
				logger(kipl::logging::Logger::LogWarning,msg.str());
			}
		}
	}
}

void ModuleManagerWidget2::on_Button_ModuleAdd()
{
	ModuleConfig module;
	AddModuleDialog add_module_dlg(m_sApplication,m_sApplicationPath);
	int res=add_module_dlg.run(m_sApplication,module);

	if (res==Gtk::RESPONSE_APPLY) { 
		Gtk::TreeModel::Row row = *(m_refModuleTreeModel->append());
		row[m_ModuleColumns.m_col_id] = 0;
		row[m_ModuleColumns.m_col_active] = true;
		row[m_ModuleColumns.m_col_name] = module.m_sModule;
		row[m_ModuleColumns.m_col_shared_object] = module.m_sSharedObject;
		row[m_ModuleColumns.m_col_parameters] = module.parameters;
	}
	on_Selected_Module();
}

void ModuleManagerWidget2::on_Button_ModuleDelete()
{
	Glib::RefPtr<Gtk::TreeSelection> selection=m_ModuleTreeView.get_selection();

	Gtk::TreeModel::iterator iter = selection->get_selected();
	if(iter) //If anything is selected
	{
		//Gtk::TreeModel::Row row = *iter;
		m_refModuleTreeModel->erase(iter);
	}

	m_refParameterTreeModel->clear();
	m_CurrentRow=m_ModuleTreeView.get_selection()->get_selected();
}

void ModuleManagerWidget2::on_Button_ParameterAdd()
{
  Gtk::TreeModel::Row row    = *(m_refParameterTreeModel->append());
  row[m_ParameterColumns.m_col_name]  = "New parameter";
  row[m_ParameterColumns.m_col_value] = "no value";
}

void ModuleManagerWidget2::on_Button_ParameterDelete()
{
	Glib::RefPtr<Gtk::TreeSelection> selection=m_ParameterTreeView.get_selection();

	Gtk::TreeModel::iterator iter = selection->get_selected();
	if(iter) //If anything is selected
	{
	//	Gtk::TreeModel::Row row = *iter;
		m_refParameterTreeModel->erase(iter);
	}
}


void ModuleManagerWidget2::SetModules(std::list<ModuleConfig> &modules)
{
	m_refModuleTreeModel->clear();
	Gtk::TreeModel::Row row;
	
	std::list<ModuleConfig>::iterator it;
	int i=0;
	for (it=modules.begin(); it!=modules.end(); it++) {
		row = *(m_refModuleTreeModel->append());
		row[m_ModuleColumns.m_col_id] = i++;
		row[m_ModuleColumns.m_col_active] = it->m_bActive;
		row[m_ModuleColumns.m_col_name] = it->m_sModule;
		row[m_ModuleColumns.m_col_shared_object] = it->m_sSharedObject;
		row[m_ModuleColumns.m_col_parameters] = it->parameters;
	}
}

std::list<ModuleConfig> ModuleManagerWidget2::GetModules()
{
	std::list<ModuleConfig> modules;

	UpdateCurrentModuleParameters();

	typedef Gtk::TreeModel::Children type_children; //minimise code length.
	type_children children = m_refModuleTreeModel->children();
	for(type_children::iterator iter = children.begin();
		iter != children.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *iter;

		ModuleConfig module;
		Glib::ustring str;
		module.m_bActive=row[m_ModuleColumns.m_col_active];
		
		str=row[m_ModuleColumns.m_col_name]; module.m_sModule=str;
		str=row[m_ModuleColumns.m_col_shared_object]; module.m_sSharedObject=str;
		module.parameters=row[m_ModuleColumns.m_col_parameters];

		modules.push_back(module);
	}

	return modules;
}

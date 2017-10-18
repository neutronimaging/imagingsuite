//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:24:22 +0200 (Mon, 21 May 2012) $
// $Rev: 1318 $
// $Id: ParameterManagerWidget.cpp 1318 2012-05-21 14:24:22Z kaestner $
//

#include "stdafx.h"
#undef max
#include "../include/ParameterManagerWidget.h"

ParameterManagerWidget::ParameterManagerWidget(void) :
Glib::ObjectBase("ParameterManagerWidget"),
m_Button_Add(Gtk::Stock::ADD),
m_Button_Delete(Gtk::Stock::DELETE)
{
  m_ScrolledWindow.add(m_TreeView);

  //Only show the scrollbars when they are necessary:
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  Gtk::Requisition req;
  req.height=256; 
  req.width=256;
  //m_ScrolledWindow.size_request(req);
  set_size_request (req.width,req.height);
  pack_start(m_ScrolledWindow,true,true);
  
  pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

  m_ButtonBox.pack_start(m_Button_Add, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_Delete, Gtk::PACK_SHRINK);
  
  m_ButtonBox.set_border_width(5);
  m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
  m_Button_Add.signal_clicked().connect( sigc::mem_fun(*this,
              &ParameterManagerWidget::on_Button_Add) );
  m_Button_Delete.signal_clicked().connect( sigc::mem_fun(*this,
              &ParameterManagerWidget::on_Button_Delete) );
  
  //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);


  //Add the TreeView's view columns:
  m_TreeView.append_column_editable("Parameter", m_Columns.m_col_name);
  m_TreeView.append_column_editable("Value", m_Columns.m_col_value);

}

ParameterManagerWidget::~ParameterManagerWidget(void)
{

}

void ParameterManagerWidget::SetParameters(std::map<std::string,std::string> &parameters)
{
	  Gtk::TreeModel::Row row; 

	  std::map<std::string,std::string>::iterator it;

	  m_refTreeModel->clear();
	  for (it=parameters.begin(); it!=parameters.end(); it++) {
		 row=*(m_refTreeModel->append());
		 row[m_Columns.m_col_name]  = it->first;
		 row[m_Columns.m_col_value] = it->second;
	  }
}

std::map<std::string,std::string> ParameterManagerWidget::GetParameters()
{
	std::map<std::string,std::string> parameters;

	typedef Gtk::TreeModel::Children type_children; //minimise code length.
	type_children children = m_refTreeModel->children();
	for(type_children::iterator iter = children.begin();
		iter != children.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *iter;
		Glib::ustring name=row[m_Columns.m_col_name];
		Glib::ustring value=row[m_Columns.m_col_value];
		parameters[name]=value;
	}

	return parameters;
}

void ParameterManagerWidget::on_Button_Add()
{
  Gtk::TreeModel::Row row    = *(m_refTreeModel->append());
  row[m_Columns.m_col_name]  = "New parameter";
  row[m_Columns.m_col_value] = "no value";
}

void ParameterManagerWidget::on_Button_Delete()
{
	Glib::RefPtr<Gtk::TreeSelection> selection=m_TreeView.get_selection();

	Gtk::TreeModel::iterator iter = selection->get_selected();
	if(iter) //If anything is selected
	{
		//Gtk::TreeModel::Row row = *iter;
		m_refTreeModel->erase(iter);
	}

}

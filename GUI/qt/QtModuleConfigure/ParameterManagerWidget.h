//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:24:22 +0200 (Mon, 21 May 2012) $
// $Rev: 1318 $
// $Id: ParameterManagerWidget.h 1318 2012-05-21 14:24:22Z kaestner $
//


#ifndef __PARAMETERMANAGERWIDGET_H
#define __PARAMETERMANAGERWIDGET_H

#include "../src/stdafx.h"
#include <gtkmm.h>
#include <string>
#include <map>


class DLL_EXPORT ParameterManagerWidget :
	public Gtk::VBox
{
public:
	ParameterManagerWidget(void);
	virtual ~ParameterManagerWidget(void);
	void SetParameters(std::map<std::string,std::string> &parameters);
	std::map<std::string,std::string> GetParameters();
protected:
	virtual void on_Button_Add();
	virtual void on_Button_Delete();
	
	//Tree model columns:
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns()
		{ add(m_col_name); add(m_col_value);  }

		Gtk::TreeModelColumn<Glib::ustring> m_col_name;
		Gtk::TreeModelColumn<Glib::ustring> m_col_value;
	};

	ModelColumns m_Columns;

	Gtk::HButtonBox m_ButtonBox;
	Gtk::Button m_Button_Add;
	Gtk::Button m_Button_Delete;

	Gtk::ScrolledWindow m_ScrolledWindow;
	Gtk::TreeView m_TreeView;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
};

#endif

//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-11-11 13:54:35 +0100 (Sun, 11 Nov 2012) $
// $Rev: 1400 $
// $Id: AddModuleDialog.h 1400 2012-11-11 12:54:35Z kaestner $
//
#ifndef __ADDMODULEDIALOG_H
#define __ADDMODULEDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QMap>
#include <QString>
#include <QButtonGroup>

#include <string>
#include <map>

#include <logging/logger.h>
#include <ModuleConfig.h>

class AddModuleDialog : public QDialog
{
    Q_OBJECT
	kipl::logging::Logger logger;
public:
    explicit AddModuleDialog(QWidget *parent = nullptr);
    virtual ~AddModuleDialog() {}

    int configure(std::string application, std::string defaultsource="", std::string application_path="");
    virtual int exec();
    ModuleConfig GetModuleConfig() {return m_ModuleConfig;}

protected slots:
    virtual void on_change_objectfile();
    virtual void accept();
    virtual void reject();

protected:
    int UpdateModuleCombobox(QString &fname);
    std::map<std::string, std::map<std::string, std::string> > GetModuleList(std::string filename);

    QVBoxLayout m_Dlg_layout;
    QHBoxLayout m_Filechooser_layout;
    QHBoxLayout m_Moduleselect_layout;

    QLabel m_Label_file;
    QLabel m_Label_module;
    QLineEdit m_Modulefile_edit;

    QPushButton m_Button_Browse;
    QComboBox m_Combobox_modules;
	
	
    QPushButton m_Button_OK;
    QPushButton m_Button_Cancel;
    QHBoxLayout m_Layout_Controls;

	std::map<std::string, std::map<std::string, std::string> > modulelist;
	std::string m_sApplication;
	std::string m_sApplicationPath;
    std::string m_sDefaultModuleSource;
    ModuleConfig m_ModuleConfig;
};
#endif

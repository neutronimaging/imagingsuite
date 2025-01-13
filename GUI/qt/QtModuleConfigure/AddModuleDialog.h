//<LICENSE>

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

    int configure(const std::string &application, 
                  const std::string &defaultsource, 
                  const std::string &application_path,
                  const std::string &sCategory);
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
    std::string m_sPreprocessorsPath;
    std::string m_sDefaultModuleSource;
    ModuleConfig m_ModuleConfig;
};
#endif

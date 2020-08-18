//<LICENSE>

#ifndef SINGLEMODULECONFIGURATORWIDGET_H
#define SINGLEMODULECONFIGURATORWIDGET_H

#include "QtModuleConfigure_global.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QFrame>
#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTreeWidget>

#include <ModuleConfig.h>
#include <logging/logger.h>

#include <map>
#include <string>

class QTMODULECONFIGURESHARED_EXPORT SingleModuleConfiguratorWidget : public QFrame
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit SingleModuleConfiguratorWidget(QWidget *parent = 0);
    void Configure(std::string sApplicationName, std::string sDefaultModuleSource="", std::string sApplicationPath="");

    void SetModule(ModuleConfig &module);
    void setDescriptionLabel(QString lbl);
    ModuleConfig GetModule();
    void SetDescription(QString name);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    
signals:
    
public slots:
    virtual void on_ButtonConfigure_Clicked();

protected:
    std::string  m_sApplication;
    std::string  m_sDefaultModuleSource;
    std::string  m_sApplicationPath;
    QFrame       m_MainFrame;
    QHBoxLayout  m_LayoutMain;
    QLabel       m_LabelDescription;
    QLabel       m_LabelModuleName;
    QPushButton  m_ButtonConfigure;

    ModuleConfig m_ModuleConfig;
};

class QTMODULECONFIGURESHARED_EXPORT SingleModuleSettingsDialog : QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit SingleModuleSettingsDialog(const std::string &sApplicationName, const std::string &sApplicationPath, const std::string & sDefaultModuleSource ,QWidget *parent);

    virtual int exec(ModuleConfig &config);
    ModuleConfig getModule();

public slots:
    virtual void on_ButtonBrowse_Clicked();
    virtual void on_ButtonBox_Clicked(QAbstractButton *button);
    virtual void on_ComboBox_Changed(QString value);

private:
    virtual int exec();
    void BuildDialog();
    int UpdateModuleCombobox(QString fname, bool bSetFirstIndex=true);
    std::map<std::string, std::map<std::string, std::string> > GetModuleList(std::string filename);
    std::map<std::string, std::string> GetParameterList();
    void UpdateCurrentModuleParameters();


    std::string m_sApplication;
    std::string m_sApplicationPath;
    std::string m_sDefaultModuleSource;
    QVBoxLayout m_LayoutMain;
    QHBoxLayout m_LayoutModule;
    QLabel      m_LabelModuleName;
    QComboBox   m_ComboModules;
    QPushButton m_ButtonBrowse;

    QTreeWidget  m_ParameterListView;
    QDialogButtonBox m_Buttons;
    QPushButton  m_ButtonOK;
    QPushButton  m_ButtonCancel;
    QPushButton  m_ButtonAdd;
    QPushButton  m_ButtonDelete;

    ModuleConfig m_ModuleConfig;
    std::map<std::string, std::map<std::string, std::string> > m_ModuleList;


};

#endif // SINGLEMODULECONFIGURATORWIDGET_H

//<LICENSE>

#ifndef SINGLEMODULESETTINGSDIALOG_H
#define SINGLEMODULESETTINGSDIALOG_H
#include "QtModuleConfigure_global.h"
#include <map>
#include <string>

#include <QDialog>
#include <QString>
#include <QAbstractButton>

#include <ModuleConfig.h>
#include <logging/logger.h>

namespace Ui {
class SingleModuleSettingsDialog;
}

class QTMODULECONFIGURESHARED_EXPORT SingleModuleSettingsDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit SingleModuleSettingsDialog(const std::string &sApplicationName,
                                        const std::string &sApplicationPath,
                                        const std::string &sCategory,
                                        const std::string & sDefaultModuleSource,
                                        QWidget *parent = nullptr);

    virtual int exec(ModuleConfig &config);
    ModuleConfig getModule();

    ~SingleModuleSettingsDialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private slots:
    void on_comboBox_Modules_currentTextChanged(const QString &module_name);

    void on_pushButton_Browse_clicked();

private:
    virtual int exec();
    int UpdateModuleCombobox(QString fname, bool bSetFirstIndex=true);
    std::map<std::string, std::map<std::string, std::string> > GetModuleList(std::string filename);
    std::map<std::string, std::string> GetParameterList();
    void UpdateCurrentModuleParameters();

    Ui::SingleModuleSettingsDialog *ui;

    std::string m_sApplication;
    std::string m_sApplicationPath;
    std::string m_sCategory;
    std::string m_sDefaultModuleSource;

    ModuleConfig m_ModuleConfig;
    std::map<std::string, std::map<std::string, std::string> > m_ModuleList;
};

#endif // SINGLEMODULESETTINGSDIALOG_H

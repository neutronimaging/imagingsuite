#include "singlemoduleconfiguratorwidget.h"
#include <QDialog>

class SingleModuleSettingsDialog : QDialog
{
    Q_OBJECT
public:
    SingleModuleSettingsDialog(QWidget *parent);

private:

};

SingleModuleConfiguratorWidget::SingleModuleConfiguratorWidget(QWidget *parent) :
    QWidget(parent),
    logger("SingleModuleConfiguratorWidget"),
    m_LabelDescription(tr("Module")),
    m_LabelModuleName(QString::fromStdString(m_ModuleConfig.m_sModule)),
    m_ButtonConfigure(tr("Settings"))
{

}

void SingleModuleConfiguratorWidget::SetModule(ModuleConfig module)
{

}

ModuleConfig SingleModuleConfiguratorWidget::GetModule()
{

    return m_ModuleConfig;
}


void SingleModuleConfiguratorWidget::SetDescription(QString name)
{
    m_LabelDescription.setText(name);
}


void SingleModuleConfiguratorWidget::on_ButtonConfigure_Clicked()
{

}

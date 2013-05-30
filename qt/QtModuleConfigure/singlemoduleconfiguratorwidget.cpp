#include "singlemoduleconfiguratorwidget.h"
#include <QDialog>
#include <iostream>

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
    m_LayoutMain(new QHBoxLayout(this)),
    m_LabelDescription(new QLabel(tr("Module"), this)),
    m_LabelModuleName(new QLabel(QString::fromStdString(m_ModuleConfig.m_sModule),this)),
    m_ButtonConfigure(new QPushButton(tr("Settings"),this))
{
    std::clog<<"a";
    this->setLayout(m_LayoutMain);
    std::clog<<"b";
    m_LayoutMain->addWidget(m_LabelDescription);
    m_LayoutMain->addWidget(m_LabelModuleName);
    m_LayoutMain->addWidget(m_ButtonConfigure);

    std::clog<<"c";
    show();
    std::clog<<"d\n";
}

QSize SingleModuleConfiguratorWidget::minimumSizeHint() const
{
    return QSize(300, 50);
}

QSize SingleModuleConfiguratorWidget::sizeHint() const
{
    return QSize(300, 50);
}
void SingleModuleConfiguratorWidget::SetModule(ModuleConfig module)
{
    m_ModuleConfig=module;
    m_LabelModuleName->setText(QString::fromStdString(m_ModuleConfig.m_sModule));
}

ModuleConfig SingleModuleConfiguratorWidget::GetModule()
{
    return m_ModuleConfig;
}


void SingleModuleConfiguratorWidget::SetDescription(QString name)
{
    m_LabelDescription->setText(name);
}


void SingleModuleConfiguratorWidget::on_ButtonConfigure_Clicked()
{

}

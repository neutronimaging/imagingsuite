#ifndef SINGLEMODULECONFIGURATORWIDGET_H
#define SINGLEMODULECONFIGURATORWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QString>

#include <ModuleConfig.h>
#include <logging/logger.h>

class SingleModuleConfiguratorWidget : public QWidget
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit SingleModuleConfiguratorWidget(QWidget *parent = 0);


    void SetModule(ModuleConfig module);
    ModuleConfig GetModule();
    void SetDescription(QString name);
    
signals:
    
public slots:
    virtual void on_ButtonConfigure_Clicked();

private:
    QHBoxLayout m_LayoutMain;
    QLabel m_LabelDescription;
    QLabel m_LabelModuleName;
    QPushButton m_ButtonConfigure;

    ModuleConfig m_ModuleConfig;
};

#endif // SINGLEMODULECONFIGURATORWIDGET_H

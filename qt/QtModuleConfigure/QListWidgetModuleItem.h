#ifndef QLISTWIDGETMODULEITEM_H
#define QLISTWIDGETMODULEITEM_H
#include <QListWidgetItem>
#include <ModuleConfig.h>
#include <QString>

class QListWidgetModuleItem : public QListWidgetItem
{
public:
    QListWidgetModuleItem(ModuleConfig & module) :
        QListWidgetItem(QString::fromStdString(module.m_sModule)),
        m_Module(module)
    {}
    ModuleConfig m_Module;
};

#endif // QLISTWIDGETMODULEITEM_H

#include "ModuleFilterProxyModel.h"
#include <QApplication>
#include <QStringList>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
// #include <iostream>
#include <memory>

ModuleFilterProxyModel::ModuleFilterProxyModel(const QString &rootPath, const QString &filter, QObject *parent) : 
    QSortFilterProxyModel(parent), 
    logger("ModuleFilterProxyModel"),
    filterString(filter) 
{
    logger.message("Root path:"+rootPath.toStdString()+", Filter:"+filter.toStdString());
    fileSystemModel = std::make_unique<QFileSystemModel>();
    fileSystemModel->setRootPath(rootPath);
    setSourceModel(fileSystemModel.get());
}

QString ModuleFilterProxyModel::rootPath() const 
{
    return fileSystemModel->rootPath();
}

bool ModuleFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QString fileName  = sourceModel()->data(index).toString();

    logger.message(fileName.toStdString());
    return fileName.contains(filterString);
}


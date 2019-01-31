#ifndef READERCONFIGLISTWIDGET_H
#define READERCONFIGLISTWIDGET_H

#include <list>

#include <QWidget>
#include <QListWidgetItem>
#include <QListWidget>

#include <logging/logger.h>
#include <datasetbase.h>

#include "readergui_global.h"

class READERGUISHARED_EXPORT ReaderConfigListWidget : public QWidget
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ReaderConfigListWidget(QWidget *parent = 0);
    ~ReaderConfigListWidget();
    void SetList(std::list<FileSet> &loaderlist);
    void AddLoader(FileSet &loader);
    std::list<FileSet> GetList();

signals:

public slots:
    virtual void on_Button_AddLoader_clicked();
    virtual void on_Button_RemoveLoader_clicked();
    virtual void on_Button_ClearLoaders_clicked();
    virtual void on_Selected_Loader_doubleclicked(QListWidgetItem* current);

protected:
    QListWidget *m_ListWidget_loaders;
};

#endif // READERCONFIGLISTWIDGET_H

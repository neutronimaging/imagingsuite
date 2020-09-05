#include <iostream>
#include <sstream>

#include <QListWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <qmessagebox.h>

#include <fileset.h>

#include "readerconfiglistwidget.h"
#include "addloaderdialog.h"

class LoaderListItem : public QListWidgetItem
{
public:
    LoaderListItem() {}
    LoaderListItem(const LoaderListItem &item) : QListWidgetItem(item) {}

    FileSet loader;
};

ReaderConfigListWidget::ReaderConfigListWidget(QWidget *parent) :
    QWidget(parent),
    logger("ReaderConfigListWidget")
{
    QVBoxLayout *layout = new QVBoxLayout;

    m_ListWidget_loaders = new QListWidget();
    m_ListWidget_loaders->setDragDropMode(QAbstractItemView::InternalMove);
    connect(m_ListWidget_loaders,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(on_Selected_Loader_doubleclicked(QListWidgetItem *)));
    layout->addWidget(m_ListWidget_loaders);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *addButton = new QPushButton("+");
    connect(addButton,SIGNAL(clicked()),this,SLOT(on_Button_AddLoader_clicked()));
    QPushButton *removeButton = new QPushButton("-");
    connect(removeButton,SIGNAL(clicked()),this,SLOT(on_Button_RemoveLoader_clicked()));
    QPushButton *clearButton = new QPushButton("Clear");
    connect(clearButton,SIGNAL(clicked()),this,SLOT(on_Button_ClearLoaders_clicked()));
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(clearButton);

    layout->addLayout(buttonLayout);

    this->setLayout(layout);
}

ReaderConfigListWidget::~ReaderConfigListWidget()
{

}

void ReaderConfigListWidget::on_Button_AddLoader_clicked()
{
    logger(logger.LogMessage,"Add loader");
    std::ostringstream msg;

    LoaderListItem *item = new LoaderListItem;

    AddLoaderDialog dlg;

    dlg.m_loader=item->loader;

    int res=0;
    try {
       res=dlg.exec();
    } catch (...) {
        QMessageBox::warning(this,"Error","Problem analyzing file names");
    }

    if (res==QDialog::Accepted) {
        item->loader=dlg.m_loader;
        msg.str("");
        msg<<"File mask: "<<(item->loader.m_sFilemask)<<std::endl
           <<"Index interval ["<<(item->loader.m_nFirst)
           <<", "<<(item->loader.m_nLast)<<"]";


        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
        m_ListWidget_loaders->addItem(item);
    }
    emit readerListModified();
}

void ReaderConfigListWidget::on_Button_RemoveLoader_clicked()
{
    logger(logger.LogMessage,"Remove loader");
    m_ListWidget_loaders->takeItem(m_ListWidget_loaders->row(m_ListWidget_loaders->currentItem()));
    emit readerListModified();
}

void ReaderConfigListWidget::on_Button_ClearLoaders_clicked()
{
    logger(logger.LogMessage,"Clear loaders");

    m_ListWidget_loaders->clear();
    emit readerListModified();
}

void ReaderConfigListWidget::on_Selected_Loader_doubleclicked(QListWidgetItem* current)
{
    logger(logger.LogMessage,"Add loader");
    std::ostringstream msg;

    LoaderListItem *item = dynamic_cast<LoaderListItem *>(current);

    AddLoaderDialog dlg;

    dlg.m_loader=item->loader;

    int res=dlg.exec();
    if (res==QDialog::Accepted) {
        item->loader=dlg.m_loader;
        msg.str("");
        msg<<"File mask: "<<(item->loader.m_sFilemask)<<std::endl
           <<"Index interval ["<<(item->loader.m_nFirst)
           <<", "<<(item->loader.m_nLast)<<"]";


        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
    }
    emit readerListModified();
}

std::list<FileSet> ReaderConfigListWidget::GetList()
{
    std::list<FileSet> loaderlist;

    for (int i=0; i<m_ListWidget_loaders->count(); i++) {
        LoaderListItem *item = dynamic_cast<LoaderListItem *>(m_ListWidget_loaders->item(i));
        loaderlist.push_back(item->loader);
    }

    return loaderlist;
}

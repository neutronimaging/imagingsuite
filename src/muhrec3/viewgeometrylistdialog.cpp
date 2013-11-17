#include "viewgeometrylistdialog.h"
#include "ui_viewgeometrylistdialog.h"

#include "stdafx.h"
#include <sstream>
#include <cstdio>

#include <ReconConfig.h>

#include <base/timage.h>

#include <QListWidgetItem>
#include <QString>
#include <QIcon>
#include <QPixmap>

class ConfigListItem : public QListWidgetItem
{
public:
    ConfigListItem() {}
    ConfigListItem(const ConfigListItem &item) : QListWidgetItem(item) {}

    kipl::base::TImage<float,2> image;
    ReconConfig config;
};



ViewGeometryListDialog::ViewGeometryListDialog(QWidget *parent) :
    QDialog(parent),
    logger("ViewGeometryListDialog"),
    ui(new Ui::ViewGeometryListDialog)
{
    ui->setupUi(this);
    ui->listWidget->setIconSize(QSize(150,150));

    SetupCallbacks();
}

ViewGeometryListDialog::~ViewGeometryListDialog()
{
    delete ui;
}

void ViewGeometryListDialog::SetupCallbacks(){
    connect(ui->buttonClearSelected,SIGNAL(clicked()),this,SLOT(ClearAllChecked()));
    connect(ui->buttonComputeTilt,SIGNAL(clicked()),this,SLOT(ComputeTilt()));
    connect(ui->buttonComputROI,SIGNAL(clicked()),this,SLOT(MaxROI()));

    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(ShowSelected(QListWidgetItem*)));
}

void ViewGeometryListDialog::setList(std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > >  &reconList)
{
    std::ostringstream msg;

    std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > >::iterator it;
    int index=0;
    for (it=reconList.begin(); it!=reconList.end(); it++) {

        msg.str("");
        msg<<"Center="<<(it->first.ProjectionInfo.fCenter)<<std::endl
          <<"Slice="<<(it->first.ProjectionInfo.roi[1])<<std::endl;
        ConfigListItem *item = new ConfigListItem;

        item->config = it->first;
        item->image  = it->second;

        QPixmap pixmap=CreateIconFromImage(it->second,
                                           it->first.MatrixInfo.fGrayInterval[0],
                                           it->first.MatrixInfo.fGrayInterval[1]);
        QIcon icon(pixmap);
        item->setIcon(pixmap);

        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
        item->setData(Qt::CheckStateRole,Qt::Unchecked);
        ui->listWidget->addItem(item);
    }
}

QPixmap ViewGeometryListDialog::CreateIconFromImage(kipl::base::TImage<float,2> &img, float lo, float hi)
{

    QPixmap pixmap(img.Size(0),img.Size(1));


    uchar * buffer=new uchar[img.Size()+1024];

    sprintf((char *)buffer,"P5\n%d\n%d\n255\n",(int)img.Size(0),(int)img.Size(1));
    int offset=strlen((char *)buffer);

    float scale = 255/(hi-lo);
    float *pImg = img.GetDataPtr();
    for (int i=0; i<img.Size(); i++) {
        if (pImg[i]<lo)
            buffer[i+offset]=0;
        else if (hi<pImg[i])
            buffer[i+offset]=255;
        else
            buffer[i+offset]=static_cast<uchar>((pImg[i]-lo)*scale);
    }
    pixmap.loadFromData(buffer,img.Size()+offset);
    delete [] buffer;
    return pixmap;
}

eChangedConfigFields ViewGeometryListDialog::changedConfigFields()
{

}

void ViewGeometryListDialog::ClearAllChecked()
{
    for(int row = 0; row < ui->listWidget->count(); row++)
    {
        QListWidgetItem *item = ui->listWidget->item(row);
        item->setCheckState(Qt::Unchecked);
    }

}

void ViewGeometryListDialog::ShowSelected(QListWidgetItem *current)
{
    logger(kipl::logging::Logger::LogMessage,"Item clicked");

    ConfigListItem *item = reinterpret_cast<ConfigListItem *>(current);

    ui->imageViewer->set_image(item->image.GetDataPtr(),
                               item->image.Dims(),
                               item->config.MatrixInfo.fGrayInterval[0],
                               item->config.MatrixInfo.fGrayInterval[1]);

}

void ViewGeometryListDialog::ComputeTilt()
{

}

void ViewGeometryListDialog::MaxROI()
{
}

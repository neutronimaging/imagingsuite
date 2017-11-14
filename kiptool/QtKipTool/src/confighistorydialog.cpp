//
// This file is part of the i KIPL image processing tool by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//

#include "confighistorydialog.h"
#include "ui_confighistorydialog.h"

#include <math/image_statistics.h>

#include <QListWidgetItem>

class ConfigListItem : public QListWidgetItem
{
public:
    ConfigListItem() {}
    ConfigListItem(const ConfigListItem &item) : QListWidgetItem(item) {}

    kipl::base::TImage<float,2> image;
    KiplProcessConfig config;
};


ConfigHistoryDialog::ConfigHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigHistoryDialog)
{
    ui->setupUi(this);
    ui->list_configurations->setIconSize(QSize(150,150));
}

ConfigHistoryDialog::~ConfigHistoryDialog()
{
    delete ui;
}

void ConfigHistoryDialog::setList(std::list<std::pair<KiplProcessConfig, kipl::base::TImage<float,2> > >  &configList)
{
    std::ostringstream msg;

    std::list<std::pair<KiplProcessConfig, kipl::base::TImage<float,2> > >::iterator it;

    for (it=configList.begin(); it!=configList.end(); it++) {

        msg.str("");
        msg<<it->first.UserInformation.sDate<<std::endl;
        ConfigListItem *item = new ConfigListItem;

        item->config = it->first;
        item->image  = it->second;

        float fMax, fMin;
        kipl::math::minmax(it->second.GetDataPtr(), it->second.Size(), &fMin, &fMax);
        QPixmap pixmap=CreateIconFromImage(it->second,
                                           fMin,
                                           fMax);
        QIcon icon(pixmap);
        item->setIcon(pixmap);

        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
        //item->setData(Qt::CheckStateRole,Qt::Unchecked);
        ui->list_configurations->addItem(item);
    }
}

QPixmap ConfigHistoryDialog::CreateIconFromImage(kipl::base::TImage<float,2> &img, float lo, float hi)
{

    QPixmap pixmap(img.Size(0),img.Size(1));


    uchar * buffer=new uchar[img.Size()+1024];

    sprintf((char *)buffer,"P5\n%d\n%d\n255\n",(int)img.Size(0),(int)img.Size(1));
    int offset=strlen((char *)buffer);

    float scale = 255/(hi-lo);
    float *pImg = img.GetDataPtr();
    for (size_t i=0; i<img.Size(); i++) {
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

void ConfigHistoryDialog::on_list_configurations_itemClicked(QListWidgetItem *item)
{
    ConfigListItem * confItem = dynamic_cast<ConfigListItem *>(item);

    ui->ImageViewer_Display->set_image(confItem->image.GetDataPtr(),confItem->image.Dims());
    m_SelectedConfig = confItem->config;

}


void ConfigHistoryDialog::on_buttonBox_accepted()
{
    accept();
}

void ConfigHistoryDialog::on_buttonBox_rejected()
{
    reject();
}

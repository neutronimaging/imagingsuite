#include "viewgeometrylistdialog.h"
#include "ui_viewgeometrylistdialog.h"

#include "stdafx.h"
#include <sstream>
#include <cstdio>
#include <map>

#include <ReconConfig.h>

#include <base/timage.h>
#include <math/mathconstants.h>

#include <QListWidgetItem>
#include <QMessageBox>
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
    ui(new Ui::ViewGeometryListDialog),
    m_eChangeConfigFields(ConfigField_None),
    m_fTilt(0.0f),
    m_fPivot(0.0f)
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
    connect(ui->buttonMaxROI,SIGNAL(clicked()),this,SLOT(MaxROI()));

    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(ShowSelected(QListWidgetItem*)));
}

void ViewGeometryListDialog::setList(std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > >  &reconList)
{
    std::ostringstream msg;

    std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > >::iterator it;

    for (it=reconList.begin(); it!=reconList.end(); it++) {

        msg.str("");
        msg<<it->first.UserInformation.sDate<<std::endl
           <<"Center="<<(it->first.ProjectionInfo.fCenter)<<std::endl
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

int ViewGeometryListDialog::changedConfigFields()
{
    return m_eChangeConfigFields;
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
    ConfigListItem *item = reinterpret_cast<ConfigListItem *>(current);

    ui->imageViewer->set_image(item->image.GetDataPtr(),
                               item->image.Dims(),
                               item->config.MatrixInfo.fGrayInterval[0],
                               item->config.MatrixInfo.fGrayInterval[1]);

}

void ViewGeometryListDialog::ComputeTilt()
{
    std::ostringstream msg;

    if (2<=ui->listWidget->count()) {
        std::map<size_t,float> centers;
        for(int row = 0; row < ui->listWidget->count(); row++)
        {
            ConfigListItem *item = dynamic_cast<ConfigListItem *>(ui->listWidget->item(row));

            if (item->checkState()==Qt::Checked) {
                centers[item->config.ProjectionInfo.roi[1]]=item->config.ProjectionInfo.fCenter;
            }
        }

        if (centers.size()==2) {
            std::map<size_t,float>::iterator a=centers.begin();
            std::map<size_t,float>::iterator b=a++;

            if (b->first==a->first) {
                QMessageBox msgdlg;

                msgdlg.setText("You have selected two items from the same slice.");
                msgdlg.exec();
                return;
            }

            if (b->first<a->first)
                swap(a,b);

            m_fPivot  = a->first;
            m_fTilt   = 180.0f / fPi *atan((b->second-a->second)/(float(b->first)-float(a->first)));
            m_fCenter = a->second;
            m_eChangeConfigFields = m_eChangeConfigFields | (int)ConfigField_Tilt;
            msg<<"Tilt "<<m_fTilt<<"deg @ "<<m_fPivot;
            ui->labelTilt->setText(QString::fromStdString(msg.str()));
        }
        else {
            QMessageBox msgdlg;

            msgdlg.setText("You have checked more than two items. Tilt can only be computed with two items.");
            msgdlg.exec();
            return;
        }

    }
    else {
         QMessageBox msgdlg;

         msgdlg.setText("You need to select two items to compute the tilt angle");
         msgdlg.exec();
         return;
    }
}

void ViewGeometryListDialog::getTilt(float &center, float &tilt, float &pivot)
{
    tilt  = m_fTilt;
    pivot = m_fPivot;
    center = m_fCenter;
}

void ViewGeometryListDialog::MaxROI()
{
    std::ostringstream msg;
    std::list<size_t *> roi;

    for(int row = 0; row < ui->listWidget->count(); row++)
    {
        ConfigListItem *item = dynamic_cast<ConfigListItem *>(ui->listWidget->item(row));

        if (item->checkState() == Qt::Checked )
        {
            if (item->config.MatrixInfo.bUseROI) {
                roi.push_back(item->config.MatrixInfo.roi);
            }
        }
    }
    msg<<"ROI count="<<roi.size();
    logger(kipl::logging::Logger::LogMessage,msg.str());
    if (!roi.empty()) {
        std::list<size_t *>::iterator it=roi.begin();

        memcpy(m_nMatrixROI,(*it),4*sizeof(size_t));

        for (it=roi.begin(); it!=roi.end(); it++) {
            m_nMatrixROI[0]=min(m_nMatrixROI[0],(*it)[0]);
            m_nMatrixROI[1]=min(m_nMatrixROI[1],(*it)[1]);
            m_nMatrixROI[2]=max(m_nMatrixROI[2],(*it)[2]);
            m_nMatrixROI[3]=max(m_nMatrixROI[3],(*it)[3]);
        }

        m_eChangeConfigFields |= ConfigField_ROI ;

        msg.str("");

        msg<<"ROI=["<<m_nMatrixROI[0]<<", "
            <<m_nMatrixROI[1]<<", "
            <<m_nMatrixROI[2]<<", "
            <<m_nMatrixROI[3]<<"]";

        ui->labelROI->setText(QString::fromStdString(msg.str()));
    }
}

void ViewGeometryListDialog::getROI(size_t *roi)
{
    memcpy(roi,m_nMatrixROI,4*sizeof(size_t));
}

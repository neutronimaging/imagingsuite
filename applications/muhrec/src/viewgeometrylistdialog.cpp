//<LICENSE>
#include "viewgeometrylistdialog.h"
#include "ui_viewgeometrylistdialog.h"

#include "stdafx.h"
#include <sstream>
#include <cstdio>
#include <map>

#include <ReconConfig.h>

#include <base/timage.h>
#include <math/mathconstants.h>
#include <math/linfit.h>
#include <base/thistogram.h>

#include <QListWidgetItem>
#include <QMessageBox>
#include <QString>
#include <QIcon>
#include <QPixmap>

class ConfigListItem : public QListWidgetItem
{
public:
    ConfigListItem() : config("") {}
    ConfigListItem(const ConfigListItem &item) : QListWidgetItem(item),config("") {}

    kipl::base::TImage<float,2> image;
    ReconConfig config;
};



ViewGeometryListDialog::ViewGeometryListDialog(QWidget *parent) :
    QDialog(parent),
    logger("ViewGeometryListDialog"),
    ui(new Ui::ViewGeometryListDialog),
    m_eChangeConfigFields(ConfigField_None),
    m_fTilt(0.0),
    m_fPivot(0.0)
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
}

void ViewGeometryListDialog::setList(std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > >  &reconList)
{
    std::ostringstream msg;

    std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > >::iterator it;

    const size_t NHist=512;
    size_t hist[NHist];
    float axis[NHist];

    for (it=reconList.begin(); it!=reconList.end(); it++) {

        msg.str("");
        if (it->first.ProjectionInfo.beamgeometry == it->first.ProjectionInfo.BeamGeometry_Parallel)
        {
            msg<<it->first.UserInformation.sDate<<std::endl
               <<"Center="<<(it->first.ProjectionInfo.fCenter)<<std::endl
              <<"Slice="<<(it->first.ProjectionInfo.roi[1])<<std::endl;
        }
        else if (it->first.ProjectionInfo.beamgeometry == it->first.ProjectionInfo.BeamGeometry_Cone)
        {
            msg<<it->first.UserInformation.sDate<<std::endl
               <<"Center="<<(it->first.ProjectionInfo.fCenter)<<std::endl
              <<"Slice="<<(it->first.ProjectionInfo.roi[3]-it->first.MatrixInfo.voi[5])<<std::endl;
        }
        ConfigListItem *item = new ConfigListItem;

        item->config = it->first;
        item->image  = it->second;

        size_t nLo=0;
        size_t nHi=0;
        memset(hist,0,sizeof(size_t)*NHist);
        memset(axis,0,sizeof(float)*NHist);
        kipl::base::Histogram(item->image.GetDataPtr(),item->image.Size(),hist,NHist,0.0f,0.0f,axis);
        kipl::base::FindLimits(hist, NHist, 99.0f, &nLo, &nHi);
        float lo=axis[nLo];
        float hi=axis[nHi];

        QPixmap pixmap=CreateIconFromImage(it->second,
                                           lo,
                                           hi);
        QIcon icon(pixmap);
        item->setIcon(pixmap);

        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
        item->setData(Qt::CheckStateRole,Qt::Unchecked);
        ui->listWidget->addItem(item);
    }
}

QPixmap ViewGeometryListDialog::CreateIconFromImage(kipl::base::TImage<float,2> &img, float lo, float hi)
{
    QPixmap pixmap(img.Size(0), img.Size(1));

    std::vector<uchar> buffer(img.Size() + 1024);

    std::string header = "P5\n" + std::to_string(img.Size(0)) + "\n" + std::to_string(img.Size(1)) + "\n255\n";
    // std::memcpy(buffer.data(), header.c_str(), header.size());
    std::copy(header.begin(), header.end(), buffer.begin());
    int offset = header.size();

    float scale = 255.0f / (hi - lo);
    auto pImg = img.GetDataPtr();
    std::transform(pImg, pImg + img.Size(), buffer.begin() + offset, [lo, hi, scale](float val) {
        return static_cast<uchar>(std::clamp((val - lo) * scale, 0.0f, 255.0f));
    });

    pixmap.loadFromData(buffer.data(), img.Size() + offset);
    return pixmap;

    // QPixmap pixmap(img.Size(0),img.Size(1));


    // uchar * buffer=new uchar[img.Size()+1024];

    // sprintf((char *)buffer,"P5\n%d\n%d\n255\n",static_cast<int>(img.Size(0)),static_cast<int>(img.Size(1)));
    // int offset=strlen((char *)buffer);

    // float scale = 255/(hi-lo);
    // float *pImg = img.GetDataPtr();
    // for (size_t i=0; i<img.Size(); i++) {
    //     if (pImg[i]<lo)
    //         buffer[i+offset]=0;
    //     else if (hi<pImg[i])
    //         buffer[i+offset]=255;
    //     else
    //         buffer[i+offset]=static_cast<uchar>((pImg[i]-lo)*scale);
    // }
    // pixmap.loadFromData(buffer,img.Size()+offset);
    // delete [] buffer;
    // return pixmap;
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

    const size_t NHist=512;
    size_t hist[NHist];
    float axis[NHist];
    memset(hist,0,sizeof(size_t)*NHist);
    memset(axis,0,sizeof(float)*NHist);

    size_t nLo=0;
    size_t nHi=0;

    kipl::base::Histogram(item->image.GetDataPtr(),item->image.Size(),hist,NHist,0.0f,0.0f,axis);
    kipl::base::FindLimits(hist, NHist, 99.0f, &nLo, &nHi);
    float lo=axis[nLo];
    float hi=axis[nHi];

    ui->imageViewer->set_image(item->image.GetDataPtr(),
                               item->image.dims(),
                               lo,
                               hi);

}

void ViewGeometryListDialog::ComputeTilt()
{
    std::ostringstream msg;

    if (2<=ui->listWidget->count()) {
        int N=1024;
        size_t *slice=new size_t[N];
        size_t minsliceidx=0;
        float *center=new float[N];
        int cnt=0;
        for(int row = 0; row < ui->listWidget->count(); row++)
        {
            ConfigListItem *item = dynamic_cast<ConfigListItem *>(ui->listWidget->item(row));

            if (item->checkState()==Qt::Checked) {
                if (item->config.ProjectionInfo.beamgeometry == item->config.ProjectionInfo.BeamGeometry_Parallel)
                {
                    slice[cnt]=item->config.ProjectionInfo.roi[1];
                }
                else if (item->config.ProjectionInfo.beamgeometry == item->config.ProjectionInfo.BeamGeometry_Cone)
                {
                    slice[cnt] = item->config.ProjectionInfo.roi[3]-item->config.MatrixInfo.voi[5];
                }
                if (slice[cnt]<slice[minsliceidx])
                    minsliceidx=cnt;
                center[cnt]=item->config.ProjectionInfo.fCenter;
                cnt++;
            }

        }
        if (1<cnt) {
            double R2;
            kipl::math::LinearLSFit(slice,center,cnt,&m_fCenter,&m_fTilt, &R2);

            msg.str("");
            msg<<"Center="<<m_fCenter<<", k="<<m_fTilt<<", R2="<<R2;
            logger(kipl::logging::Logger::LogMessage, msg.str());
//            m_fPivot  = slice[minsliceidx];

            ConfigListItem *item = dynamic_cast<ConfigListItem *>(ui->listWidget->item(0));
            if (item->config.ProjectionInfo.beamgeometry == item->config.ProjectionInfo.BeamGeometry_Parallel)
            {
                m_fPivot  = floor((item->config.ProjectionInfo.projection_roi[3]+item->config.ProjectionInfo.projection_roi[1])/2);
            }
            else if (item->config.ProjectionInfo.beamgeometry == item->config.ProjectionInfo.BeamGeometry_Cone)
            {
                m_fPivot  = floor(item->config.ProjectionInfo.fpPoint[1]);
            }

            m_fCenter+= m_fPivot*m_fTilt;
            m_fTilt   = -180.0f / fPi *atan(m_fTilt);

            m_eChangeConfigFields = m_eChangeConfigFields | static_cast<int>(ConfigField_Tilt);
            msg.str("");
            ui->labelTilt->setText(QString("%1").arg(m_fTilt,0,'g',4));
            ui->labelCenter->setText(QString("%1").arg(m_fCenter,0,'g',4));
            ui->labelPivot->setText(QString("%1").arg(m_fPivot,0,'g',4));
            ui->labelR2->setText(QString("%1").arg(R2,0,'g',4));
        }
        else {
            QMessageBox msgdlg;

            msgdlg.setText("You have only checked one item. Tilt can only be computed with at least two items.");
            msgdlg.exec();
        }
        delete [] slice;
        delete [] center;
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
    std::list<std::vector<size_t> > ROIs;

    for(int row = 0; row < ui->listWidget->count(); row++)
    {
        ConfigListItem *item = dynamic_cast<ConfigListItem *>(ui->listWidget->item(row));

        if (item->checkState() == Qt::Checked )
        {
            if (item->config.MatrixInfo.bUseROI) {
                ROIs.push_back(item->config.MatrixInfo.roi);
            }
        }
    }
    msg<<"ROI count="<<ROIs.size();
    logger(kipl::logging::Logger::LogMessage,msg.str());
    if (!ROIs.empty())
    {
        m_nMatrixROI = ROIs.front();

        for (const auto & roi : ROIs)
        {
            m_nMatrixROI[0]=min(m_nMatrixROI[0],roi[0]);
            m_nMatrixROI[1]=min(m_nMatrixROI[1],roi[1]);
            m_nMatrixROI[2]=max(m_nMatrixROI[2],roi[2]);
            m_nMatrixROI[3]=max(m_nMatrixROI[3],roi[3]);
        }

        m_eChangeConfigFields |= ConfigField_ROI ;

        msg.str("");

        msg<<"ROI=["<<m_nMatrixROI[0]<<", "
                    <<m_nMatrixROI[1]<<", "
                    <<m_nMatrixROI[2]<<", "
                    <<m_nMatrixROI[3]<<"]";
    }
}

void ViewGeometryListDialog::getROI(std::vector<size_t> &roi)
{
    roi = m_nMatrixROI;
}

void ViewGeometryListDialog::getROIi(std::vector<int> &roi)
{
    std::copy(m_nMatrixROI.begin(),m_nMatrixROI.end(),roi.begin());
}


void ViewGeometryListDialog::on_buttonSelectAll_clicked()
{
    for(int row = 0; row < ui->listWidget->count(); row++)
    {
        QListWidgetItem *item = ui->listWidget->item(row);
        item->setCheckState(Qt::Checked);
    }

}

void ViewGeometryListDialog::on_buttonClearSelected_clicked()
{
    ClearAllChecked();
}

void ViewGeometryListDialog::on_buttonComputeTilt_clicked()
{
    ComputeTilt();
}

void ViewGeometryListDialog::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ShowSelected(item);
}

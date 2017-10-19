#include <sstream>

#include <QDir>
#include <QFileDialog>
#include <QString>

#include <io/DirAnalyzer.h>
#include <strings/filenames.h>
#include <base/timage.h>
#include <io/io_tiff.h>

#include "reslicerdialog.h"
#include "ui_reslicerdialog.h"



ReslicerDialog::ReslicerDialog(QWidget *parent) :
    QDialog(parent),
    logger("ResliceDialog"),
    ui(new Ui::ReslicerDialog)
{
    ui->setupUi(this);
    UpdateDialog();
}

ReslicerDialog::~ReslicerDialog()
{
    delete ui;
}

void ReslicerDialog::on_pushButton_browsein_clicked()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select a file from data set",
                                      ui->lineEdit_infilemask->text());
    if (!projdir.isEmpty()) {
        std::string pdir=projdir.toStdString();

        #ifdef _MSC_VER
        const char slash='\\';
        #else
        const char slash='/';
        #endif
        ptrdiff_t pos=pdir.find_last_of(slash);

        QString path(QString::fromStdString(pdir.substr(0,pos+1)));
        kipl::io::DirAnalyzer da;
        kipl::io::FileItem fi=da.GetFileMask(pdir);

        ui->lineEdit_infilemask->setText(QString::fromStdString(fi.m_sMask));
    }
}

void ReslicerDialog::on_pushButton_browsout_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select destination directory",
                                      ui->lineEdit_PathOut->text());
    if (!projdir.isEmpty()) {
        ui->lineEdit_PathOut->setText(projdir);
    }
}

void ReslicerDialog::on_pushButton_startreslice_clicked()
{
    UpdateConfig();
    m_reslicer.process();
}

void ReslicerDialog::UpdateDialog()
{
    ui->lineEdit_infilemask->setText(QString::fromStdString(m_reslicer.m_sSourceMask));
    ui->lineEdit_PathOut->setText(QString::fromStdString(m_reslicer.m_sDestinationPath));
    ui->lineEdit_MaskOut->setText(QString::fromStdString(m_reslicer.m_sDestinationMask));

    ui->spinBox_firstslice->setValue(m_reslicer.m_nFirst);
    ui->spinBox_lastslice->setValue(m_reslicer.m_nLast);
    ui->spinBox_firstXZ->setValue(m_reslicer.m_nFirstXZ);
    ui->spinBox_lastXZ->setValue(m_reslicer.m_nLastXZ);
    ui->spinBox_firstYZ->setValue(m_reslicer.m_nFirstYZ);
    ui->spinBox_lastYZ->setValue(m_reslicer.m_nLastYZ);

    ui->checkBox_XZ->setChecked(m_reslicer.m_bResliceXZ);
    ui->checkBox_YZ->setChecked(m_reslicer.m_bResliceYZ);
}

void ReslicerDialog::UpdateConfig()
{
    m_reslicer.m_sSourceMask      = ui->lineEdit_infilemask->text().toStdString();
    m_reslicer.m_sDestinationPath = ui->lineEdit_PathOut->text().toStdString();
    m_reslicer.m_sDestinationMask = ui->lineEdit_MaskOut->text().toStdString();

    m_reslicer.m_nFirst           = ui->spinBox_firstslice->value();
    m_reslicer.m_nLast            = ui->spinBox_lastslice->value();
    m_reslicer.m_nFirstXZ         = ui->spinBox_firstXZ->value();
    m_reslicer.m_nLastXZ          = ui->spinBox_lastXZ->value();
    m_reslicer.m_nFirstYZ         = ui->spinBox_firstYZ->value();
    m_reslicer.m_nLastYZ          = ui->spinBox_lastYZ->value();

    m_reslicer.m_bResliceXZ       = ui->checkBox_XZ->isChecked();
    m_reslicer.m_bResliceYZ       = ui->checkBox_YZ->isChecked();
}

void ReslicerDialog::on_pushButton_preview_clicked()
{
    int idx=ui->spinBox_firstslice->value()+(ui->spinBox_lastslice->value()-ui->spinBox_firstslice->value())/2;

    std::string fmask=ui->lineEdit_infilemask->text().toStdString();
    std::string fname,ext;
    kipl::strings::filenames::MakeFileName(fmask,idx,fname,ext,'#','0');

    kipl::base::TImage<float,2> img;

    try {
        kipl::io::ReadTIFF(img,fname.c_str());
    }
    catch (kipl::base::KiplException &e) {
        std::ostringstream msg;
        msg<<"Failed to load preview image: "<<fname<<std::endl<<e.what();
        logger(logger.LogError, msg.str());
    }
    ui->viewer_slice->set_image(img.GetDataPtr(),img.Dims());
    m_currentROI=QRect(0,0,img.Size(0)-1,img.Size(1)-1);

    ui->spinBox_firstXZ->setMaximum(m_currentROI.right());
    ui->spinBox_lastXZ->setMaximum(m_currentROI.right());
    ui->spinBox_firstYZ->setMaximum(m_currentROI.bottom());
    ui->spinBox_lastYZ->setMaximum(m_currentROI.bottom());

    ui->spinBox_firstXZ->setValue(m_currentROI.left());
    ui->spinBox_lastXZ->setValue(m_currentROI.right());
    ui->spinBox_firstYZ->setValue(m_currentROI.top());
    ui->spinBox_lastYZ->setValue(m_currentROI.bottom());
}

void ReslicerDialog::on_pushButton_getROI_clicked()
{
    QRect roi=ui->viewer_slice->get_marked_roi();

    ui->spinBox_firstXZ->setValue(roi.left());
    ui->spinBox_firstYZ->setValue(roi.top());
    ui->spinBox_lastXZ->setValue(roi.right());
    ui->spinBox_lastYZ->setValue(roi.bottom());

    ui->viewer_slice->set_rectangle(roi,QColor("red"),0);
    m_currentROI=roi;
}

void ReslicerDialog::on_spinBox_firstXZ_valueChanged(int arg1)
{
    m_currentROI.setLeft(arg1);
    ui->viewer_slice->set_rectangle(m_currentROI,QColor("red"),0);
}

void ReslicerDialog::on_spinBox_lastXZ_valueChanged(int arg1)
{
    m_currentROI.setRight(arg1);
    ui->viewer_slice->set_rectangle(m_currentROI,QColor("red"),0);
}

void ReslicerDialog::on_spinBox_firstYZ_valueChanged(int arg1)
{
    m_currentROI.setTop(arg1);
    ui->viewer_slice->set_rectangle(m_currentROI,QColor("red"),0);
}

void ReslicerDialog::on_spinBox_lastYZ_valueChanged(int arg1)
{
    m_currentROI.setBottom(arg1);
    ui->viewer_slice->set_rectangle(m_currentROI,QColor("red"),0);
}

#include <iostream>
#include <sstream>

#include <QString>
#include <QFileDialog>
#include <QMessageBox>

#include <io/DirAnalyzer.h>
#include <io/io_tiff.h>
#include <strings/filenames.h>
#include <base/timage.h>

#include "mergevolumesdialog.h"
#include "ImagingToolConfig.h"
#include "ui_mergevolumesdialog.h"

MergeVolumesDialog::MergeVolumesDialog(QWidget *parent) :
    QDialog(parent),
    logger("MergeVolumesDialog"),
    ui(new Ui::MergeVolumesDialog)
{
    ui->setupUi(this);
    UpdateDialog();
}

MergeVolumesDialog::~MergeVolumesDialog()
{
    delete ui;
}

void MergeVolumesDialog::UpdateDialog()
{
    ui->spinBox_firstA->setValue(m_merger.m_nFirstA);
    ui->spinBox_lastA->setValue(m_merger.m_nLastA);

    ui->spinBox_firstB->setValue(m_merger.m_nFirstB);
    ui->spinBox_lastB->setValue(m_merger.m_nLastB);

    ui->spinBox_firstout->setValue(m_merger.m_nFirstDest);

    ui->lineEdit_pathA->setText(QString::fromStdString(m_merger.m_sPathA));
    ui->lineEdit_pathB->setText(QString::fromStdString(m_merger.m_sPathB));
    ui->lineEdit_pathout->setText(QString::fromStdString(m_merger.m_sPathOut));

    ui->spinBox_mixstart->setValue(m_merger.m_nStartOverlapA);
    ui->spinBox_mixlength->setValue(m_merger.m_nOverlapLength);

    ui->checkBox_crop->setChecked(m_merger.m_bCropSlices);
    on_checkBox_crop_toggled(m_merger.m_bCropSlices);
    ui->spinBox_offsetx->setValue(m_merger.m_nCropOffset[0]);
    ui->spinBox_offsety->setValue(m_merger.m_nCropOffset[1]);

    ui->spinBox_crop0->setValue(m_merger.m_nCrop[0]);
    ui->spinBox_crop1->setValue(m_merger.m_nCrop[1]);
    ui->spinBox_crop2->setValue(m_merger.m_nCrop[2]);
    ui->spinBox_crop3->setValue(m_merger.m_nCrop[3]);
    ui->comboBox_mixorder->setCurrentIndex(m_merger.m_nMergeOrder);
}

void MergeVolumesDialog::UpdateConfig()
{
        m_merger.m_nFirstA = ui->spinBox_firstA->value();
        m_merger.m_nLastA = ui->spinBox_lastA->value();

        m_merger.m_nFirstB = ui->spinBox_firstB->value();
        m_merger.m_nLastB = ui->spinBox_lastB->value();

        m_merger.m_nFirstDest = ui->spinBox_firstout->value();

        m_merger.m_sPathA = ui->lineEdit_pathA->text().toStdString();
        m_merger.m_sPathB = ui->lineEdit_pathB->text().toStdString();
        m_merger.m_sPathOut = ui->lineEdit_pathout->text().toStdString();

        m_merger.m_nStartOverlapA = ui->spinBox_mixstart->value();
        m_merger.m_nOverlapLength = ui->spinBox_mixlength->value();
        m_merger.m_nMergeOrder = ui->comboBox_mixorder->currentIndex();

        m_merger.m_bCropSlices = ui->checkBox_crop->isChecked();
        m_merger.m_nCropOffset[0] = ui->spinBox_offsetx->value();
        m_merger.m_nCropOffset[1] = ui->spinBox_offsety->value();

        m_merger.m_nCrop[0] = ui->spinBox_crop0->value();
        m_merger.m_nCrop[1] = ui->spinBox_crop1->value();
        m_merger.m_nCrop[2] = ui->spinBox_crop2->value();
        m_merger.m_nCrop[3] = ui->spinBox_crop3->value();
}

void MergeVolumesDialog::on_pushButton_browseA_clicked()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select location of date set A",
                                      ui->lineEdit_pathA->text());
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

        ui->lineEdit_pathA->setText(QString::fromStdString(fi.m_sMask));
    }
}

void MergeVolumesDialog::on_pushButton_browseB_clicked()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select location of date set B",
                                      ui->lineEdit_pathB->text());
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

        ui->lineEdit_pathB->setText(QString::fromStdString(fi.m_sMask));
    }
}

void MergeVolumesDialog::on_pushButton_loaddata_clicked()
{
    on_pushButton_loadA_clicked();
    on_pushButton_loadB_clicked();
}

void MergeVolumesDialog::on_pushButton_loadA_clicked()
{
    std::ostringstream msg;
    int first=ui->spinBox_firstA->value();
    int last=ui->spinBox_lastA->value();
    std::string fmask=ui->lineEdit_pathA->text().toStdString();

    try {
        m_merger.LoadVerticalSlice(fmask,first,last,&m_VerticalImgA);
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox dlg;
        dlg.setText("Failed to load vertical slice A");
        dlg.setDetailedText(QString::fromStdString(e.what()));
        msg.str(""); msg<<"Failed to load vertical slice.\ntrace:\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        dlg.exec();
        return;
    }
    ui->viewer_dataA->set_image(m_VerticalImgA.GetDataPtr(),m_VerticalImgA.Dims());
}

void MergeVolumesDialog::on_pushButton_loadB_clicked()
{
    std::ostringstream msg;
    int first=ui->spinBox_firstB->value();
    int last=ui->spinBox_lastB->value();
    std::string fmask=ui->lineEdit_pathB->text().toStdString();

    try {
        m_merger.LoadVerticalSlice(fmask,first,last,&m_VerticalImgB);
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox dlg;
        dlg.setText("Failed to load vertical slice B");
        dlg.setDetailedText(QString::fromStdString(e.what()));
        msg.str(""); msg<<"Failed to load vertical slice.\ntrace:\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());

        dlg.exec();
        return;
    }
    ui->viewer_dataB->set_image(m_VerticalImgB.GetDataPtr(),m_VerticalImgB.Dims());
}

void MergeVolumesDialog::on_comboBox_mixorder_currentIndexChanged(int index)
{

}

void MergeVolumesDialog::on_checkBox_crop_toggled(bool checked)
{
    ui->spinBox_crop0->setVisible(checked);
    ui->spinBox_crop1->setVisible(checked);
    ui->spinBox_crop2->setVisible(checked);
    ui->spinBox_crop3->setVisible(checked);
    ui->spinBox_offsetx->setVisible(checked);
    ui->spinBox_offsety->setVisible(checked);
    ui->label_cropA->setVisible(checked);
    ui->label_crop0->setVisible(checked);
    ui->label_crop1->setVisible(checked);
    ui->label_crop2->setVisible(checked);
    ui->label_crop3->setVisible(checked);
    ui->label_offsetB->setVisible(checked);
    ui->label_offsetx->setVisible(checked);
    ui->label_offsety->setVisible(checked);
}

void MergeVolumesDialog::on_pushButton_browseout_clicked()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select location of date set B",
                                      ui->lineEdit_pathout->text());
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

        ui->lineEdit_pathout->setText(QString::fromStdString(fi.m_sMask));
    }
}

void MergeVolumesDialog::on_pushButton_startmerge_clicked()
{
    UpdateConfig();
    try {
        m_merger.Process();
    }
    catch (kipl::base::KiplException & e) {
        QMessageBox dlg;

        logger(logger.LogError,e.what());
        dlg.setText("Failed to perform the mix");
        dlg.setDetailedText(QString::fromStdString(e.what()));
    }
}

void MergeVolumesDialog::on_comboBox_result_currentIndexChanged(int index)
{
    switch (index) {
    case 0 :
        logger(kipl::logging::Logger::LogMessage,"Display local vertical");
        break; // Local vertical
    case 1 :
        logger(kipl::logging::Logger::LogMessage,"Display full vertical");
        break; // Full vertical
    case 2 :
        logger(kipl::logging::Logger::LogMessage,"Display mixed");

        break; // Mixed slice
    }
}


#include <iostream>
#include <sstream>
#include <string>

#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

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
    SaveConfig();
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
        ui->viewer_result->set_image(m_VerticalImgLocalResult.GetDataPtr(),
                                     m_VerticalImgLocalResult.Dims());
        break; // Local vertical
    case 1 :
        logger(kipl::logging::Logger::LogMessage,"Display full vertical");
        ui->viewer_result->set_image(m_VerticalImgResult.GetDataPtr(),
                                     m_VerticalImgResult.Dims());
        break; // Full vertical
    case 2 :
        logger(kipl::logging::Logger::LogMessage,"Display mixed");

        logger(kipl::logging::Logger::LogMessage,"Display local vertical");
        ui->viewer_result->set_image(m_HorizontalSliceResult.GetDataPtr(),
                                     m_HorizontalSliceResult.Dims());

        break; // Mixed slice
    }
}

void MergeVolumesDialog::on_pushButton_TestMix_clicked()
{
    UpdateConfig();
    SaveConfig();
    std::ostringstream msg;

    logger(logger.LogMessage,"on_testmix");
    if ((m_merger.m_nStartOverlapA<m_merger.m_nFirstA) ||
            (m_merger.m_nLastA<(m_merger.m_nStartOverlapA+m_merger.m_nOverlapLength))) {
        msg.str("");
        msg<<"Indexing error in data A: FirstA="<<m_merger.m_nFirstA<<", StartOverlapA="<<m_merger.m_nStartOverlapA<<", OverlapLength="<<m_merger.m_nOverlapLength;
        logger(logger.LogError,msg.str());
        return;
    }

    if (((m_merger.m_nLastA-m_merger.m_nFirstA)<m_merger.m_nOverlapLength) ||
            ((m_merger.m_nLastB-m_merger.m_nFirstB)<m_merger.m_nOverlapLength))
    {
        logger(logger.LogError,"Overlap length is greater than the number of slices");
        return;
    }

    if (!m_VerticalImgA.Size() || !m_VerticalImgB.Size()) {
        logger(logger.LogError,"Data is not loaded");
        return;
    }

   if (m_VerticalImgA.Size(0)==m_VerticalImgB.Size(0)) {
        size_t dims[2]={static_cast<size_t>(m_VerticalImgA.Size(0)),
                        static_cast<size_t>(m_merger.m_nStartOverlapA+m_merger.m_nLastB+1-m_merger.m_nFirstA-m_merger.m_nFirstB)};
        m_VerticalImgResult.Resize(dims);
        memcpy(m_VerticalImgResult.GetDataPtr(),
            m_VerticalImgA.GetDataPtr(),
            (m_merger.m_nStartOverlapA-m_merger.m_nFirstA)*m_VerticalImgA.Size(0)*sizeof(float));
        memcpy(m_VerticalImgResult.GetLinePtr(m_merger.m_nOverlapLength+m_merger.m_nStartOverlapA-m_merger.m_nFirstA),
            m_VerticalImgB.GetLinePtr(m_merger.m_nOverlapLength),
            (m_merger.m_nLastB-m_merger.m_nFirstB-m_merger.m_nOverlapLength)*m_VerticalImgB.Size(0)*sizeof(float));
        size_t idxa=m_merger.m_nStartOverlapA-m_merger.m_nFirstA;
        size_t idxb=0;
        size_t i;
        float *pA, *pB, *pMix;
        float scale=1.0f/m_merger.m_nOverlapLength;
        for (i=0; i<m_merger.m_nOverlapLength; i++,idxa++,idxb++) {
            pA=m_VerticalImgA.GetLinePtr(idxa);
            pB=m_VerticalImgB.GetLinePtr(idxb);
            pMix=m_VerticalImgResult.GetLinePtr(idxa);

            for (size_t j=0; j<m_VerticalImgA.Size(0); j++) {
                pMix[j]=(1.0f-i*scale)*pA[j]+i*scale*pB[j];
            }
        }

        if (m_VerticalImgResult.Size(0)<m_VerticalImgResult.Size(1)) {
            int half=m_VerticalImgResult.Size(0)/2;
            int idx_start=idxa-half;
            idx_start=idx_start < 0 ? 0 : idx_start;

            int idx_stop=idxa+half;
            idx_stop=m_VerticalImgResult.Size(1) < idx_stop  ? m_VerticalImgResult.Size(1) : idx_stop;

            size_t dims[2]={m_VerticalImgResult.Size(0),static_cast<size_t>(idx_stop-idx_start)};
            m_VerticalImgLocalResult.Resize(dims);
            memcpy(m_VerticalImgLocalResult.GetDataPtr(),
                   m_VerticalImgResult.GetLinePtr(idx_start),
                   m_VerticalImgLocalResult.Size()*sizeof(float));
        }
        else {
            m_VerticalImgLocalResult=m_VerticalImgResult;
            m_VerticalImgLocalResult.Clone();
        }

        ui->comboBox_result->setCurrentIndex(0);
        on_comboBox_result_currentIndexChanged(0);
    }
   else
   {
       logger(logger.LogError,"Width mismatch between data A and B");
       return;
   }

}

void MergeVolumesDialog::SaveConfig()
{
    QDir dir;
    //QString qfname=
    std::string fname=dir.homePath().toStdString();

    kipl::strings::filenames::CheckPathSlashes(fname,true);
    fname+="volumemerge.xml";

    logger(logger.LogMessage,fname);

    std::ofstream conffile(fname.c_str());

    conffile<<m_merger.WriteXML(0);
}

void MergeVolumesDialog::LoadConfig()
{

}



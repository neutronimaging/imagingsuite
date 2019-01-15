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
    LoadConfig();
    UpdateDialog();
}

MergeVolumesDialog::~MergeVolumesDialog()
{
    delete ui;
}

void MergeVolumesDialog::UpdateDialog()
{
    ImageLoader loader;
    loader.m_nFirst=m_merger.m_nFirstA;
    loader.m_nLast = m_merger.m_nLastA;
    loader.m_sFilemask = m_merger.m_sPathA;

    ui->widget_readerFormA->setReaderConfig(loader);

    loader.m_nFirst=m_merger.m_nFirstB;
    loader.m_nLast = m_merger.m_nLastB;
    loader.m_sFilemask = m_merger.m_sPathB;
    ui->widget_readerFormB->setReaderConfig(loader);

    ui->spinBox_firstout->setValue(m_merger.m_nFirstDest);

    ui->lineEdit_pathout->setText(QString::fromStdString(m_merger.m_sPathOut));

    ui->spinBox_mixstart->setValue(m_merger.m_nStartOverlapA);
    ui->spinBox_mixlength->setValue(m_merger.m_nOverlapLength);

    ui->groupBox_cropImages->setChecked(m_merger.m_bCropSlices);
    ui->spinBox_offsetx->setValue(m_merger.m_nCropOffset[0]);
    ui->spinBox_offsety->setValue(m_merger.m_nCropOffset[1]);

    ui->widget_cropROI->setROI(m_merger.m_nCrop[0],m_merger.m_nCrop[1],m_merger.m_nCrop[2],m_merger.m_nCrop[3]);
    ui->comboBox_mixorder->setCurrentIndex(m_merger.m_nMergeOrder);
}

void MergeVolumesDialog::UpdateConfig()
{
    ImageLoader loader;
    loader=ui->widget_readerFormA->getReaderConfig();

    m_merger.m_nFirstA = loader.m_nFirst;
    m_merger.m_nLastA  = loader.m_nLast;
    m_merger.m_sPathA  = loader.m_sFilemask;

    loader=ui->widget_readerFormB->getReaderConfig();

    m_merger.m_nFirstB = loader.m_nFirst;
    m_merger.m_nLastB  = loader.m_nLast;
    m_merger.m_sPathB  = loader.m_sFilemask;

    m_merger.m_nFirstDest = ui->spinBox_firstout->value();

    m_merger.m_sPathOut = ui->lineEdit_pathout->text().toStdString();
    m_merger.m_sMaskOut = ui->lineEdit_OutMask->text().toStdString();

    m_merger.m_nStartOverlapA = ui->spinBox_mixstart->value();
    m_merger.m_nOverlapLength = ui->spinBox_mixlength->value();
    m_merger.m_nMergeOrder = ui->comboBox_mixorder->currentIndex();

    m_merger.m_bCropSlices = ui->groupBox_cropImages->isChecked();
    m_merger.m_nCropOffset[0] = ui->spinBox_offsetx->value();
    m_merger.m_nCropOffset[1] = ui->spinBox_offsety->value();

    ui->widget_cropROI->getROI(m_merger.m_nCrop);
}

void MergeVolumesDialog::on_pushButton_loaddata_clicked()
{
    on_pushButton_loadA_clicked();
    on_pushButton_loadB_clicked();
}

void MergeVolumesDialog::on_pushButton_loadA_clicked()
{
    std::ostringstream msg;
    ImageLoader loader=ui->widget_readerFormA->getReaderConfig();

    try {
        m_merger.LoadVerticalSlice(loader.m_sFilemask,loader.m_nFirst,loader.m_nLast,&m_VerticalImgA);
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
    on_comboBox_mixorder_currentIndexChanged(ui->comboBox_mixorder->currentIndex());
}

void MergeVolumesDialog::on_pushButton_loadB_clicked()
{
    std::ostringstream msg;

    ImageLoader loader=ui->widget_readerFormB->getReaderConfig();

    try {
        m_merger.LoadVerticalSlice(loader.m_sFilemask,loader.m_nFirst,loader.m_nLast,&m_VerticalImgB);
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
    on_comboBox_mixorder_currentIndexChanged(ui->comboBox_mixorder->currentIndex());
}

void MergeVolumesDialog::on_comboBox_mixorder_currentIndexChanged(int index)
{
    ImageLoader loader;
    switch (index) {
    case 0:
        loader=ui->widget_readerFormA->getReaderConfig();
        break;
    case 1:
        loader=ui->widget_readerFormB->getReaderConfig();
    }

    ui->spinBox_mixstart->setMinimum(loader.m_nFirst);
    ui->spinBox_mixstart->setMaximum(loader.m_nLast);
}

void MergeVolumesDialog::on_pushButton_browseout_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location of date set B",
                                      ui->lineEdit_pathout->text());
    if (!projdir.isEmpty()) {
        std::string pdir=projdir.toStdString();
        kipl::strings::filenames::CheckPathSlashes(pdir,true);

        ui->lineEdit_pathout->setText(QString::fromStdString(pdir));
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
    if (m_VerticalImgResult.Size()==0)
    {
        logger.warning("No image is available");
        return;
    }

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

        QMessageBox::critical(this,"Indexing error",QString::fromStdString(msg.str()));
        return;
    }

    if (((m_merger.m_nLastA-m_merger.m_nFirstA)<m_merger.m_nOverlapLength) ||
            ((m_merger.m_nLastB-m_merger.m_nFirstB)<m_merger.m_nOverlapLength))
    {
        logger(logger.LogError,"Overlap length is greater than the number of slices");
        QMessageBox::critical(this,"Too few slices","Overlap length is greater than the number of slices");

        return;
    }

    if (!m_VerticalImgA.Size() || !m_VerticalImgB.Size()) {
        logger(logger.LogError,"Data is not loaded");
        QMessageBox::critical(this,"No data","At least one data set is not loaded.");

        return;
    }

   if (m_VerticalImgA.Size(0)==m_VerticalImgB.Size(0)) {
        size_t dims[2]={static_cast<size_t>(m_VerticalImgA.Size(0)),
                        static_cast<size_t>(m_merger.m_nStartOverlapA+m_merger.m_nLastB+1-m_merger.m_nFirstA-m_merger.m_nFirstB)};
        m_VerticalImgResult.Resize(dims);
        m_VerticalImgResult=0.0f;
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
            size_t half=m_VerticalImgResult.Size(0)/2;
            size_t idx_start = idxa < half ? 0 : idxa-half;
            size_t idx_stop=idxa+half;

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

    QString qfname=dir.homePath()+"/"+".imagingtools";
    if (dir.exists(qfname))
    {
        dir.mkdir(qfname);
        logger.message("Created .imagingtools folder");
    }

    std::string fname = (qfname+"/").toStdString();
    kipl::strings::filenames::CheckPathSlashes(fname,true);
    fname+="volumemerge.xml";

    logger(logger.LogMessage,fname);

    std::ofstream conffile(fname.c_str());

    conffile<<m_merger.WriteXML(0);
}

void MergeVolumesDialog::LoadConfig()
{
    QDir dir;
     QString confname=dir.homePath()+"/"+".imagingtools/volumemerge.xml" ;
    if (dir.exists(confname)) {
        m_merger.ParseXML(confname.toStdString());
    }
}


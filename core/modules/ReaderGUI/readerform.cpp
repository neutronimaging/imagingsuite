#include "readerform.h"
#include "ui_readerform.h"

#include <sstream>
#include <string>
#include <vector>

#include <QString>
#include <QFileDialog>
#include <QMessageBox>

#include <strings/filenames.h>
#include <io/DirAnalyzer.h>
#include <imagereader.h>

#include <uxroiwidget.h>

ReaderForm::ReaderForm(QWidget *parent) :
    QWidget(parent),
    logger("ReaderForm"),
    ui(new Ui::ReaderForm)
{
    ui->setupUi(this);
    showMinimalForm();
    ui->widget_roi->setCheckable(true);
}

ReaderForm::~ReaderForm()
{
    delete ui;
}

void ReaderForm::on_button_browse_clicked()
{
    std::ostringstream msg;
    std::string sPath, sFname;
    std::vector<std::string> exts;
    kipl::strings::filenames::StripFileName(ui->edit_fileMask->text().toStdString(),sPath,sFname,exts);
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select a file",
                                      QString::fromStdString(sPath));
    if (!projdir.isEmpty()) {
        int count=0;
        int first=0;
        int last=0;

        ImageReader reader;

        std::string pdir=projdir.toStdString();

        auto dims = reader.GetImageSize(pdir,1.0f);
        switch (dims.size())
        {
        case 2UL:
            {
                kipl::io::DirAnalyzer da;
                kipl::io::FileItem fi=da.GetFileMask(pdir);

                ui->edit_fileMask->setText(QString::fromStdString(fi.m_sMask));
                da.AnalyzeMatchingNames(fi.m_sMask,count,first,last);
                msg<<"Found "<<count<<" files for mask "<<fi.m_sMask<<" in the interval "<<first<<" to "<<last;
            }
            break;
        case 3UL:
            {
                first = 0;
                last  = static_cast<int>(dims[2]-1);
                count = static_cast<int>(dims[2]);

                ui->edit_fileMask->setText(projdir);
                msg<<"Found "<<count<<" slices in file "<<pdir<<" in the interval "<<first<<" to "<<last;
            }
            break;
        default :
            QMessageBox::warning(this,"Image dimensions","Unsupported image dimensions");
            return;
        }


        logger(logger.LogMessage,msg.str());

        ui->spinBox_first->setValue(first);
        ui->spinBox_last->setValue(last);

        emit fileMaskChanged(getReaderConfig());
    }
}

FileSet ReaderForm::getReaderConfig()
{
    FileSet cfg;

    cfg.m_sFilemask = ui->edit_fileMask->text().toStdString();

    cfg.m_nFirst = ui->spinBox_first->value();
    cfg.m_nLast  = ui->spinBox_last->value();
    cfg.m_nStep  = ui->spinBox_step->value();
    cfg.m_nRepeat = ui->spinBox_repeat->value();
    cfg.m_nStride = ui->spinBox_stride->value();

    cfg.m_Rotate = static_cast<kipl::base::eImageRotate>(ui->comboBox_rotate->currentIndex());
    cfg.m_Flip = static_cast<kipl::base::eImageFlip>(ui->comboBox_mirror->currentIndex());

    cfg.m_bUseROI = ui->widget_roi->isChecked();
    ui->widget_roi->getROI(cfg.m_ROI);

    return cfg;
}

void ReaderForm::setReaderConfig(FileSet &cfg)
{
    ui->edit_fileMask->setText(QString::fromStdString(cfg.m_sFilemask));

    ui->spinBox_first->setValue(cfg.m_nFirst);
    ui->spinBox_last->setValue(cfg.m_nLast);
    ui->spinBox_step->setValue(cfg.m_nStep);
    ui->spinBox_repeat->setValue(cfg.m_nRepeat);
    ui->spinBox_stride->setValue(cfg.m_nStride);
    ui->comboBox_mirror->setCurrentIndex(static_cast<int>(cfg.m_Flip));
    ui->comboBox_rotate->setCurrentIndex(static_cast<int>(cfg.m_Rotate));
    ui->widget_roi->setROI(cfg.m_ROI);
    ui->widget_roi->setCheckable(true);
    ui->widget_roi->setChecked(cfg.m_bUseROI);
    emit fileMaskChanged(getReaderConfig());
}

void ReaderForm::setLabel(QString str)
{
    ui->label_fileMask->setText(str);
}

void ReaderForm::showMinimalForm()
{
    setHideRepeat(true);
    setHideStride(true);
    setHideMirrorRotate(true);
    setHideROI(true);
}

void ReaderForm::showFullForm()
{
    setHideRepeat(false);
    setHideStride(false);
    setHideMirrorRotate(false);
    setHideROI(false);
}

void ReaderForm::setHideRepeat(bool x)
{
    ui->spinBox_repeat->setHidden(x);
    ui->label_repeat->setHidden(x);
}

void ReaderForm::setHideStride(bool x)
{
    ui->spinBox_stride->setHidden(x);
    ui->label_stride->setHidden(x);
}

void ReaderForm::setHideMirrorRotate(bool x)
{
    ui->comboBox_mirror->setHidden(x);
    ui->comboBox_rotate->setHidden(x);
    ui->label_mirror->setHidden(x);
    ui->label_rotate->setHidden(x);
}

void ReaderForm::setHideROI(bool x)
{
    ui->widget_roi->setHidden(x);
}

void ReaderForm::registerViewer(QtAddons::ImageViewerWidget *viewer)
{
    ui->widget_roi->registerViewer(viewer);
}

QtAddons::uxROIWidget * ReaderForm::roiWidget()
{
    return ui->widget_roi;
}

void ReaderForm::on_spinBox_first_valueChanged(int arg1)
{
    (void) arg1;
    emit fileIntervalChanged(getReaderConfig());
}

void ReaderForm::on_spinBox_step_valueChanged(int arg1)
{
    (void) arg1;
    emit fileIntervalChanged(getReaderConfig());
}

void ReaderForm::on_spinBox_last_valueChanged(int arg1)
{
    (void) arg1;
    emit fileIntervalChanged(getReaderConfig());
}

void ReaderForm::on_spinBox_repeat_valueChanged(int arg1)
{
    (void) arg1;
    emit fileIntervalChanged(getReaderConfig());
}

void ReaderForm::on_spinBox_stride_valueChanged(int arg1)
{
    (void) arg1;
    emit fileIntervalChanged(getReaderConfig());
}

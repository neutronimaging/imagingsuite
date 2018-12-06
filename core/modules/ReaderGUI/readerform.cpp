#include "readerform.h"
#include "ui_readerform.h"

#include <sstream>
#include <string>
#include <vector>

#include <qstring.h>
#include <qfiledialog.h>

#include <strings/filenames.h>
#include <io/DirAnalyzer.h>

ReaderForm::ReaderForm(QWidget *parent) :
    QWidget(parent),
    logger("ReaderForm"),
    ui(new Ui::ReaderForm)
{
    ui->setupUi(this);
    showMinimalForm();
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
        std::string pdir=projdir.toStdString();

        kipl::io::DirAnalyzer da;
        kipl::io::FileItem fi=da.GetFileMask(pdir);

        ui->edit_fileMask->setText(QString::fromStdString(fi.m_sMask));

        int count=0;
        int first=0;
        int last=0;

        da.AnalyzeMatchingNames(fi.m_sMask,count,first,last);

        msg<<"Found "<<count<<" files for mask "<<fi.m_sMask<<" in the interval "<<first<<" to "<<last;
        logger(logger.LogMessage,msg.str());

        ui->spinBox_first->setValue(first);
        ui->spinBox_last->setValue(last);
    }
}

ImageLoader ReaderForm::getReaderConfig()
{
    ImageLoader cfg;

    cfg.m_sFilemask = ui->edit_fileMask->text().toStdString();

    cfg.m_nFirst = ui->spinBox_first->value();
    cfg.m_nLast  = ui->spinBox_last->value();
    cfg.m_nStep  = ui->spinBox_step->value();
    cfg.m_nRepeat = ui->spinBox_repeat->value();
    cfg.m_nStride = ui->spinBox_stride->value();

    cfg.m_Rotate = static_cast<kipl::base::eImageRotate>(ui->comboBox_rotate->currentIndex());
    cfg.m_Flip = static_cast<kipl::base::eImageFlip>(ui->comboBox_mirror->currentIndex());

    ui->widget_roi->getROI(cfg.m_ROI);

    return cfg;
}

void ReaderForm::setReaderConfig(ImageLoader &cfg)
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

}

void ReaderForm::setLabel(QString str)
{
    ui->label_fileMask->setText(str);
}

void ReaderForm::showMinimalForm()
{
    setHidden(true);
    setHideStride(true);
    showMirrorRotate(true);
    showROI(true);
}

void ReaderForm::showFullForm()
{
    setHideRepeat(false);
    setHideStride(false);
    showMirrorRotate(false);
    showROI(false);
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

void ReaderForm::showMirrorRotate(bool x)
{
    ui->comboBox_mirror->setHidden(x);
    ui->comboBox_rotate->setHidden(x);
    ui->label_mirror->setHidden(x);
    ui->label_rotate->setHidden(x);
}

void ReaderForm::showROI(bool x)
{
    ui->widget_roi->setHidden(x);
}

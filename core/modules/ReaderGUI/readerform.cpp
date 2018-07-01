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

    return cfg;
}

void ReaderForm::setReaderConfig(ImageLoader &cfg)
{
    ui->edit_fileMask->setText(QString::fromStdString(cfg.m_sFilemask));

    ui->spinBox_first->setValue(cfg.m_nFirst);
    ui->spinBox_last->setValue(cfg.m_nLast);
    ui->spinBox_step->setValue(cfg.m_nStep);
}

void ReaderForm::setLabel(QString str)
{
    ui->label_fileMask->setText(str);
}

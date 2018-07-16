#include "singlefileform.h"
#include "ui_singlefileform.h"

#include <QFileDialog>

#include <strings/filenames.h>

SingleFileForm::SingleFileForm(QWidget *parent, bool _load) :
    QWidget(parent),
    ui(new Ui::SingleFileForm),
    load(_load)
{
    ui->setupUi(this);
}

SingleFileForm::~SingleFileForm()
{
    delete ui;
}

void SingleFileForm::setFileName(QString fn)
{
    ui->lineEdit_fileName->setText(fn);
}

void SingleFileForm::setFileName(std::string fn)
{
    ui->lineEdit_fileName->setText(QString::fromStdString(fn));
}

std::string SingleFileForm::getFileName()
{
    return ui->lineEdit_fileName->text().toStdString();
}

void SingleFileForm::setLabel(QString str)
{
    ui->label_fileName->setText(str);
}

void SingleFileForm::setFileOperation(bool _load)
{
    load=_load;
}

void SingleFileForm::on_pushButton_browse_clicked()
{
    std::string sPath, sFname;
    std::vector<std::string> exts;
    kipl::strings::filenames::StripFileName(ui->lineEdit_fileName->text().toStdString(),sPath,sFname,exts);
    QString projdir="";

    if (load==true)
        projdir=QFileDialog::getOpenFileName(this,
                                      "Select a file",
                                      QString::fromStdString(sPath));
    else
        projdir=QFileDialog::getSaveFileName(this,
                                      "Save to ",
                                      QString::fromStdString(sPath));

   sFname=projdir.toStdString();

   kipl::strings::filenames::CheckPathSlashes(sFname,false);

   ui->lineEdit_fileName->setText(QString::fromStdString(sFname));
}

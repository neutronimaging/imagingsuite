#include "QFileDialog"

#include "dialogtoobig.h"
#include "ui_dialogtoobig.h"

DialogTooBig::DialogTooBig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTooBig)
{
    ui->setupUi(this);
}

DialogTooBig::~DialogTooBig()
{
    delete ui;
}

void DialogTooBig::on_button_browse_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location the reconstructed slices",
                                      ui->lineEdit_path->text());

    if (!projdir.isEmpty())
        ui->lineEdit_path->setText(projdir);
}

void DialogTooBig::SetFields(QString path, QString mask)
{
    ui->lineEdit_path->setText(path);
    ui->lineEdit_mask->setText(mask);
}

QString DialogTooBig::GetPath()
{
    return ui->lineEdit_path->text();
}

QString DialogTooBig::GetMask()
{
    return ui->lineEdit_mask->text();
}

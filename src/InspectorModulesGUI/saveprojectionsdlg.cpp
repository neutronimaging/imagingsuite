#include "saveprojectionsdlg.h"
#include "ui_saveprojectionsdlg.h"
#include "QFileDialog"

SaveProjectionsDlg::SaveProjectionsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveProjectionsDlg)
{
    ui->setupUi(this);
}

SaveProjectionsDlg::~SaveProjectionsDlg()
{
    delete ui;
}

void SaveProjectionsDlg::on_buttonBrowse_clicked()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select destination of the images",
                                      ui->lineEdit->text());
    if (!projdir.isEmpty()) {
        ui->lineEdit->setText(projdir);
    }
}

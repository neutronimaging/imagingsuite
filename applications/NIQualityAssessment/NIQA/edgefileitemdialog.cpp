#include "edgefileitemdialog.h"
#include "ui_edgefileitemdialog.h"

#include <qstring.h>
#include <qfiledialog.h>

EdgeFileItemDialog::EdgeFileItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EdgeFileItemDialog)
{
    ui->setupUi(this);
}

EdgeFileItemDialog::~EdgeFileItemDialog()
{
    delete ui;
}

void EdgeFileItemDialog::on_buttonBrowse_clicked()
{
    QString fname=QFileDialog::getOpenFileName(this,
                                      "Select edge image file",
                                      ui->editFilename->text());
    if (!fname.isEmpty())
            ui->editFilename->setText(fname);

}

void EdgeFileItemDialog::setInfo(float distance, QString fname)
{
    ui->editFilename->setText(fname);
    ui->spinDistance->setValue(distance);
}

void EdgeFileItemDialog::getInfo(float &distance, QString &fname)
{
    distance=ui->spinDistance->value();
    fname=ui->editFilename->text();

}

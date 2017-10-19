#include "saveasdialog.h"
#include "ui_saveasdialog.h"
#include <QSignalBlocker>
#include <QFileDialog>

#include <strings/filenames.h>

SaveAsDialog::SaveAsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveAsDialog)
{
    ui->setupUi(this);
    ui->checkBox_interval->setChecked(false);
    on_checkBox_interval_toggled(ui->checkBox_interval->isChecked());
    ui->lineEdit_mask->setText("image_#####.tif");
}

SaveAsDialog::~SaveAsDialog()
{
    delete ui;
}

void SaveAsDialog::on_checkBox_interval_toggled(bool checked)
{
    ui->spinBox_First->setVisible(checked);
    ui->spinBox_Last->setVisible(checked);
    ui->label_first->setVisible(checked);
    ui->label_last->setVisible(checked);
}

void SaveAsDialog::setLimits(int first, int last)
{
    ui->spinBox_First->setMinimum(first);
    ui->spinBox_First->setMaximum(last);
    ui->spinBox_First->setValue(first);

    ui->spinBox_Last->setMinimum(first);
    ui->spinBox_Last->setMaximum(last);
    ui->spinBox_Last->setValue(last);
}

void SaveAsDialog::setPath(std::string path, std::string mask)
{
    ui->lineEdit_path->setText(QString::fromStdString(path));
    ui->lineEdit_mask->setText(QString::fromStdString(mask));
}

void SaveAsDialog::getDialogInfo(std::string &filemask, int &first, int &last)
{
    std::string path=ui->lineEdit_path->text().toStdString();

    kipl::strings::filenames::CheckPathSlashes(path,true);

    filemask = path + ui->lineEdit_mask->text().toStdString();

    if (ui->checkBox_interval->isChecked()) {
        first = ui->spinBox_First->minimum();
        last = ui->spinBox_First->maximum();
    }
    else {
        first = ui->spinBox_First->value();
        last = ui->spinBox_Last->value();
    }

    if (last<first)
        std::swap(first,last);

}

void SaveAsDialog::on_pushButton_browse_clicked()
{
    QString destdir=QFileDialog::getExistingDirectory(this,
                                      "Select destination folder",
                                      ui->lineEdit_path->text());

    if (!destdir.isEmpty())
        ui->lineEdit_path->setText(destdir);
}

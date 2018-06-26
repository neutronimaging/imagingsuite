//<LICENSE>

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

void DialogTooBig::SetFields(QString path, QString mask, bool reconAll, int first, int last, int roiMin, int roiMax)
{
    ui->lineEdit_path->setText(path);
    ui->lineEdit_mask->setText(mask);


    ui->spin_firstSlice->setMinimum(std::min(roiMin,roiMax));
    ui->spin_firstSlice->setMaximum(std::max(roiMin,roiMax));
    ui->spin_firstSlice->setValue(first);


    ui->spin_lastSlice->setMinimum(std::min(roiMin,roiMax));
    ui->spin_lastSlice->setMaximum(std::max(roiMin,roiMax));
    ui->spin_lastSlice->setValue(last);

    ui->check_reconAll->setChecked(reconAll);

    on_check_reconAll_toggled(reconAll);
}

QString DialogTooBig::GetPath()
{
    return ui->lineEdit_path->text();
}

QString DialogTooBig::GetMask()
{
    return ui->lineEdit_mask->text();
}

int DialogTooBig::GetFirst()
{
    int val=ui->spin_firstSlice->value();
    if (ui->check_reconAll->isChecked())
        val=ui->spin_firstSlice->minimum();

    return val;
}

int DialogTooBig::GetLast()
{
    int val=ui->spin_lastSlice->value();
    if (ui->check_reconAll->isChecked())
        val=ui->spin_lastSlice->maximum();

    return val;

}

void DialogTooBig::on_check_reconAll_toggled(bool checked)
{
    ui->spin_firstSlice->setVisible(!checked);
    ui->label_first->setVisible(!checked);
    ui->spin_lastSlice->setVisible(!checked);
    ui->label_last->setVisible(!checked);
}

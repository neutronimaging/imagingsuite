#include "doublethresholddlg.h"
#include "ui_doublethresholddlg.h"

DoubleThresholdDlg::DoubleThresholdDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoubleThresholdDlg)
{
    ui->setupUi(this);
}

DoubleThresholdDlg::~DoubleThresholdDlg()
{
    delete ui;
}

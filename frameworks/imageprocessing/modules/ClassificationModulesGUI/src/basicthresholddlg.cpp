#include "basicthresholddlg.h"
#include "ui_basicthresholddlg.h"

BasicThresholdDlg::BasicThresholdDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BasicThresholdDlg)
{
    ui->setupUi(this);
}

BasicThresholdDlg::~BasicThresholdDlg()
{
    delete ui;
}

#include "tiltwarningdialog.h"
#include "ui_tiltwarningdialog.h"

TiltWarningDialog::TiltWarningDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TiltWarningDialog)
{
    ui->setupUi(this);
}

TiltWarningDialog::~TiltWarningDialog()
{
    delete ui;
}

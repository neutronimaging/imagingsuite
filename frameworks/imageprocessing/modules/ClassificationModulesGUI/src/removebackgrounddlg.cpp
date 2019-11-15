#include "removebackgrounddlg.h"
#include "ui_removebackgrounddlg.h"

RemoveBackgroundDlg::RemoveBackgroundDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveBackgroundDlg)
{
    ui->setupUi(this);
}

RemoveBackgroundDlg::~RemoveBackgroundDlg()
{
    delete ui;
}

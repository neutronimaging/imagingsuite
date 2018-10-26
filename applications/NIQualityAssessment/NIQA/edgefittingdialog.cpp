#include "edgefittingdialog.h"
#include "ui_edgefittingdialog.h"

edgefittingdialog::edgefittingdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::edgefittingdialog)
{
    ui->setupUi(this);
}

edgefittingdialog::~edgefittingdialog()
{
    delete ui;
}

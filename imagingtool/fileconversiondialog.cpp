#include "fileconversiondialog.h"
#include "ui_fileconversiondialog.h"

FileConversionDialog::FileConversionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileConversionDialog)
{
    ui->setupUi(this);
}

FileConversionDialog::~FileConversionDialog()
{
    delete ui;
}

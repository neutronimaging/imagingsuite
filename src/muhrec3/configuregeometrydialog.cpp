#include "configuregeometrydialog.h"
#include "ui_configuregeometrydialog.h"

ConfigureGeometryDialog::ConfigureGeometryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureGeometryDialog)
{
    ui->setupUi(this);
}

ConfigureGeometryDialog::~ConfigureGeometryDialog()
{
    delete ui;
}

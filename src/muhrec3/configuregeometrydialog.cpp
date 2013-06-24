#include "configuregeometrydialog.h"
#include "ui_configuregeometrydialog.h"

ConfigureGeometryDialog::ConfigureGeometryDialog(QWidget *parent) :
    QDialog(parent),
    logger("ConfigGeometryDialog"),
    ui(new Ui::ConfigureGeometryDialog)
{
    ui->setupUi(this);
}

ConfigureGeometryDialog::~ConfigureGeometryDialog()
{
    delete ui;
}

int ConfigureGeometryDialog::exec(ReconConfig &config)
{
    LoadImages();
    m_Config=config;
    UpdateDialog();

    return QDialog::exec();
}

void ConfigureGeometryDialog::GetConfig(ReconConfig & config)
{
    UpdateConfig();
    config=m_Config;
}

void ConfigureGeometryDialog::LoadImages()
{

}

void ConfigureGeometryDialog::UpdateConfig()
{

}

void ConfigureGeometryDialog::UpdateDialog()
{

}

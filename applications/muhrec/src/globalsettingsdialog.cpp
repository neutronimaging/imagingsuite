#include "globalsettingsdialog.h"
#include "ui_globalsettingsdialog.h"

GlobalSettingsDialog::GlobalSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GlobalSettingsDialog)
{
    ui->setupUi(this);
}

GlobalSettingsDialog::~GlobalSettingsDialog()
{
    delete ui;
}

void GlobalSettingsDialog::setConfig(ReconConfig &config)
{
    ui->spinBox_margin->setValue(config.ProjectionInfo.nMargin);
    ui->spinBox_memory->setValue(config.System.nMemory);
    ui->checkBox_validateData->setChecked(config.System.bValidateData);
    ui->spinBox_threads->setValue(config.System.nMaxThreads);
}

void GlobalSettingsDialog::updateConfig(ReconConfig &config)
{
    config.ProjectionInfo.nMargin = ui->spinBox_margin->value();
    config.System.nMemory         = ui->spinBox_memory->value();
    config.System.bValidateData   = ui->checkBox_validateData->isChecked();
    config.System.nMaxThreads       = ui->spinBox_threads->value();
}


#include <fstream>
#include <QDir>
#include <QFileDialog>

#include <ModuleException.h>

#include "applicationconfigdialog.h"
#include "ui_applicationconfigdialog.h"

ApplicationConfigDialog::ApplicationConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplicationConfigDialog),
    config(nullptr)
{
    ui->setupUi(this);
}

void ApplicationConfigDialog::setApplicationConfig(ApplicationConfig *cfg)
{
    if (cfg!=nullptr)
        config=cfg;
    else
        throw ModuleException("No config reference was provided.",__FILE__,__LINE__);
}

int ApplicationConfigDialog::exec()
{


    if (config==nullptr)
        throw ModuleException("No config reference was provided.",__FILE__,__LINE__);

    ui->lineEdit_AppPath->setText(QString::fromStdString(config->applicationPath));
    ui->lineEdit_DataPath->setText(QString::fromStdString(config->dataPath));
    ui->spinBox->setValue(config->memory);

    int res=QDialog::exec();

    if (res==QDialog::Accepted) {
        config->applicationPath=ui->lineEdit_AppPath->text().toStdString();
        config->dataPath= ui->lineEdit_DataPath->text().toStdString();
        config->memory = ui->spinBox->value();


        QDir dir;
        QString confname=dir.homePath()+"/.imagingtools";

        if (dir.exists(confname)==false)
            dir.mkdir(confname);

        confname+=QString("/appconfig_")+QString::fromStdString(config->getAppName())+QString(".xml");
        std::string sConfName=confname.toStdString();
        std::ofstream conffile(sConfName.c_str());

        conffile<<config->streamXML();
    }

    return res;
}

ApplicationConfigDialog::~ApplicationConfigDialog()
{
    delete ui;
}

void ApplicationConfigDialog::on_pushButton_AppPath_clicked()
{
    QString dir=QFileDialog::getExistingDirectory(this,
                                      "Select application folder",
                                      ui->lineEdit_AppPath->text());

    ui->lineEdit_AppPath->setText(dir);
}

void ApplicationConfigDialog::on_pushButton_DataPath_clicked()
{
    QString dir=QFileDialog::getExistingDirectory(this,
                                      "Select data folder",
                                      ui->lineEdit_DataPath->text());

    ui->lineEdit_DataPath->setText(dir);
}

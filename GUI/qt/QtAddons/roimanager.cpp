#include "roimanager.h"
#include "ui_roimanager.h"

ROIManager::ROIManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ROIManager)
{
    ui->setupUi(this);
}

ROIManager::~ROIManager()
{
    delete ui;
}

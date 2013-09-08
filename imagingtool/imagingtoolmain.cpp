#include "imagingtoolmain.h"
#include "ui_imagingtoolmain.h"

ImagingToolMain::ImagingToolMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ImagingToolMain)
{
    ui->setupUi(this);

}

ImagingToolMain::~ImagingToolMain()
{
    delete ui;
}

#include <string>
#include <list>
#include <sstream>

#include <QFileDialog>

#include <base/timage.h>
#include <base/trotate.h>
#include <io/io_generic.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonBrowseFile_clicked()
{
    QString fname=QFileDialog::getOpenFileName(this,
                                      "Select location of the data file",
                                      ui->editFileName->text());
    if (!fname.isEmpty()) {
        ui->editFileName->setText(fname);
    }
}

void MainWindow::on_buttonProcess_clicked()
{
    std::string fname        = ui->editFileName->text().toStdString();
    int         sizeX        = ui->spinSizeX->value();
    int         sizeY        = ui->spinSizeY->value();
    int         stride       = ui->spinStride->value();
    bool        rotate       = ui->checkRotate->isChecked();
    bool        customlevels = ui->checkCustomLevels->isChecked();
    int         type         = ui->comboDataType->currentIndex();
    int         levelLo      = ui->spinLevelLo->value();
    int         levelHi      = ui->spinLevelHi->value();

    //std::list<kipl::base::TImage<unsigned short,2> > imgs;
    std::list<kipl::base::TImage<float,2> > imgs;

    if (ui->checkStride->isChecked()) {
        stride=static_cast<int>(sizeX*(type+1)/2);
        ui->spinStride->setValue(stride);
    }

    kipl::io::ReadGeneric(  imgs,
                            fname,
                            sizeX, sizeY,
                            0,  stride,
                            1,
                            static_cast<kipl::base::eDataType>(type),
                            kipl::base::BigEndian,
                            {});

    kipl::base::TImage<float,2> img;

    kipl::base::TRotate<float> rot;
    if (rotate) {
        img=rot.Rotate90(*imgs.begin());
    }
    else {
        img=*imgs.begin();
    }

    if (customlevels)
        ui->viewer->set_image(img.GetDataPtr(),img.dims(),levelLo,levelHi);
    else
        ui->viewer->set_image(img.GetDataPtr(),img.dims());

    std::ostringstream text;

    text<<(stride*sizeY);
    ui->labelFileSize->setText(QString::fromStdString(text.str()));

}

void MainWindow::on_spinSizeX_valueChanged(int arg1)
{

}

void MainWindow::on_spinSizeY_valueChanged(int arg1)
{

}

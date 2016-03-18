#include <iostream>
#include <string>

#include <QApplication>
#include <QStringList>
#include <QFile>

#include <base/timage.h>
#include <io/analyzefileext.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>

#include "viewermainwindow.h"

#include "ui_viewermainwindow.h"

ViewerMainWindow::ViewerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewerMainWindow)
{
    ui->setupUi(this);
    QStringList args=QApplication::arguments();

    std::cout<<"N arguments="<<args.size()<<std::endl;
    if (1<args.size()) {
        std::string fname=args.last().toStdString();
        kipl::base::TImage<float,2> img;
        LoadImage(fname,img);
        ui->viewer->set_image(img.GetDataPtr(),img.Dims());
    }
}

ViewerMainWindow::~ViewerMainWindow()
{
    delete ui;
}

void ViewerMainWindow::on_actionOpen_triggered()
{

}

void ViewerMainWindow::LoadImage(std::string fname,kipl::base::TImage<float,2> &img)
{
    if (QFile::exists(QString::fromStdString(fname))) {
        switch (kipl::io::GetFileExtensionType(fname)) {
            case kipl::io::ExtensionTXT: std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionDMP: std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionDAT: std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionXML: std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionRAW: std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionFITS: kipl::io::ReadFITS(img,fname.c_str()); break;
            case kipl::io::ExtensionTIFF: kipl::io::ReadTIFF(img,fname.c_str()); break;
            case kipl::io::ExtensionPNG: std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionMAT: std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionHDF: std::cout<<"Image format not supported"<<std::endl; break;
        }

    }
    else {
        std::cout<<"File does not exist"<<endl;
    }
}

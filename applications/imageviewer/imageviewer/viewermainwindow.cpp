#include <iostream>
#include <string>
#include <vector>

#include <QApplication>
#include <QStringList>
#include <QFile>
#include <QFileDialog>
#include <QSignalBlocker>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QMimeData>
#include <QtDebug>
#include <QMessageBox>

#include <base/timage.h>
#include <io/analyzefileext.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
#include <io/io_vivaseq.h>
#include <io/io_nexus.h>
#include <math/sums.h>

#include <imagewriter.h>

#include "viewermainwindow.h"
#include "saveasdialog.h"

#include "ui_viewermainwindow.h"

ViewerMainWindow::ViewerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("ViewerMainWindow"),
    ui(new Ui::ViewerMainWindow),
    isMultiFrame(false)
{
    ui->setupUi(this);
    QStringList args=QApplication::arguments();

    std::cout<<"N arguments="<<args.size()<<std::endl;
    if (1<args.size()) {
        std::string fname=args.last().toStdString();
        kipl::base::TImage<float,2> img;
        try {
            LoadImage(fname,img);
        }  catch (kipl::base::KiplException &e) {
            QMessageBox::warning(this,"Load error",QString::fromStdString(e.what()));
            return;
        }
        catch (...) {
            QMessageBox::warning(this,"Load error","An unhandled exception was thrown");
            return;
        }

        ui->viewer->set_image(img.GetDataPtr(),img.dims());
    }

    setAcceptDrops(true);

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
    if (QFile::exists(QString::fromStdString(fname)))
    {
        isMultiFrame = false;
        m_ext = kipl::io::GetFileExtensionType(fname);
        switch (m_ext)
        {
            case kipl::io::ExtensionTXT  : std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionDMP  : std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionDAT  : std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionXML  : std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionRAW  : std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionFITS : kipl::io::ReadFITS(img,fname); break;
            case kipl::io::ExtensionTIFF : kipl::io::ReadTIFF(img,fname); break;
            case kipl::io::ExtensionPNG  : std::cout<<"Image format not supported"<<std::endl; break;
            case kipl::io::ExtensionHDF  :
            {
                kipl::io::ReadNexus(img,fname,0,{});
                break;
            }

            case kipl::io::ExtensionSEQ  :
            {
                kipl::io::ViVaSEQHeader header;
                kipl::io::GetViVaSEQHeader(fname,&header);
                isMultiFrame = true;
                if (fname!=m_fname)
                {
                    ui->horizontalSlider->setMinimum(0);
                    ui->horizontalSlider->setMaximum(header.numberOfFrames-1);
                    ui->spinBox->setMinimum(0);
                    ui->spinBox->setMaximum(header.numberOfFrames-1);
                    ui->horizontalSlider->setValue(0);

                }

                kipl::io::ReadViVaSEQ(fname,img,ui->horizontalSlider->value());
            }
            break;
        }
        m_fname = fname;
    }
    else
    {

        logger.warning("File does not exist");
    }
}

void ViewerMainWindow::on_horizontalSlider_sliderMoved(int position)
{
    QSignalBlocker blockspin(ui->spinBox);
    QSignalBlocker blockslider(ui->horizontalSlider);
    kipl::base::TImage<float,2> img;
    ui->spinBox->setValue(position);
    LoadImage(m_fname,img);
    float low,high;
    ui->viewer->get_levels(&low,&high);
    ui->viewer->set_image(img.GetDataPtr(),img.dims(),low,high);

}

void ViewerMainWindow::on_spinBox_valueChanged(int arg1)
{
    QSignalBlocker blockspin(ui->spinBox);
    QSignalBlocker blockslider(ui->horizontalSlider);
    kipl::base::TImage<float,2> img;
    ui->horizontalSlider->setValue(arg1);
    LoadImage(m_fname,img);
    float low,high;
    ui->viewer->get_levels(&low,&high);
    ui->viewer->set_image(img.GetDataPtr(),img.dims(),low,high);
}

void ViewerMainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    const QMimeData * mimeData = e->mimeData();

    if (mimeData->hasUrls()) {
        e->acceptProposedAction();
    }
}

void ViewerMainWindow::dropEvent(QDropEvent *e)
{

    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        qDebug() << "Dropped file:" << fileName;

        kipl::base::TImage<float,2> img;
        LoadImage(fileName.toStdString(),img);
        ui->viewer->set_image(img.GetDataPtr(),img.dims());
    }
}

void ViewerMainWindow::on_actionSave_as_triggered()
{
    std::ostringstream msg;
    std::string path;
    std::string fname;
    std::vector<std::string> exts;

    kipl::strings::filenames::StripFileName(m_fname,path,fname,exts);

    if (m_ext == kipl::io::ExtensionSEQ) {
        SaveAsDialog dlg;
        kipl::io::ViVaSEQHeader header;
        kipl::io::GetViVaSEQHeader(m_fname,&header);

        dlg.setLimits(0,header.numberOfFrames-1);
        dlg.setPath(path,fname+"_#####.tif");

        int res=dlg.exec();

        if (res==QDialog::Accepted) {
            std::string fmask,ext;
            int first;
            int last;

            dlg.getDialogInfo(fmask, first, last);

            msg.str(""); msg<<fmask<<", first="<<first<<", last="<<last;
            logger.message(msg.str());
            std::string destname;
            ImageWriter writer;
            kipl::base::TImage<float,2> img;
            for (int i=first; i<=last; ++i) {
                kipl::io::ReadViVaSEQ(m_fname,img,i);

                kipl::strings::filenames::MakeFileName(fmask,i,destname,ext,'#','0');
                writer.write(img,destname);
            }
        }
    }
    else {
        std::string destname=QFileDialog::getSaveFileName(this,"Save image as",QString::fromStdString(path)).toStdString();

        msg.str(""); msg<<"Saving to:"<<destname;
    }
}

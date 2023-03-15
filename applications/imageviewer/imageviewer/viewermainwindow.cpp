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
#include <analyzefileext.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
#include <io/io_vivaseq.h>
#include <io/io_nexus.h>
#include <math/sums.h>
#include <pixelsizedlg.h>

#include <imagewriter.h>

#include "viewermainwindow.h"
#include "saveasdialog.h"

#include "ui_viewermainwindow.h"

ViewerMainWindow::ViewerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("ViewerMainWindow"),
    ui(new Ui::ViewerMainWindow),
    logdlg(this),
    isMultiFrame(false)
{
    logdlg.setModal(false);
    kipl::logging::Logger::addLogTarget(&logdlg);

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
    QString fileName = QFileDialog::getOpenFileName(this, "Open File",
                                                    "./",
                                                    "Images (*.fits *.hdf *.tif)");

    updateView(fileName.toStdString());
    ui->viewer->set_levels(kipl::base::quantile99);
}

void ViewerMainWindow::LoadImage(std::string fname,kipl::base::TImage<float,2> &img)
{
    std::ostringstream msg;
    if (QFile::exists(QString::fromStdString(fname)))
    {
        isMultiFrame = false;
        m_ext = readers::GetFileExtensionType(fname);
        switch (m_ext)
        {
            case readers::ExtensionTXT  : logger.message("Image format not supported"); break;
            case readers::ExtensionDMP  : logger.message("Image format not supported"); break;
            case readers::ExtensionDAT  : logger.message("Image format not supported"); break;
            case readers::ExtensionXML  : logger.message("Image format not supported"); break;
            case readers::ExtensionRAW  : logger.message("Image format not supported"); break;
            case readers::ExtensionJPG  : logger.message("Image format not supported"); break;
            case readers::ExtensionFITS : kipl::io::ReadFITS(img,fname); break;
            case readers::ExtensionTIFF : kipl::io::ReadTIFF(img,fname); break;
            case readers::ExtensionPNG  : logger.message("Image format not supported"); break;
            case readers::ExtensionHDF  :
            case readers::ExtensionHDF4 :
            case readers::ExtensionHDF5 :
            {
                auto dims=kipl::io::GetNexusDims(fname);

                int idx = 0;
                if (dims.size()==3)
                {
                    isMultiFrame = true;
                    if (fname!=m_fname)
                    {
                        ui->horizontalSlider->setMinimum(0);
                        ui->horizontalSlider->setMaximum(dims[2]-1);
                        ui->spinBox->setMinimum(0);
                        ui->spinBox->setMaximum(dims[2]-1);
                        ui->horizontalSlider->setValue(0);
                    }
                    idx = ui->horizontalSlider->value();
                }

                kipl::io::ReadNexus(img,fname,idx,{});
                break;
            }

            case readers::ExtensionSEQ  :
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

void ViewerMainWindow::updateView(const std::string &fname)
{
    LoadImage(fname,currentImage);
    float low,high;
    auto roi = ui->viewer->get_marked_roi();
    ui->viewer->get_levels(&low,&high);
    ui->viewer->set_image(currentImage.GetDataPtr(),currentImage.dims(),low,high,true);
    QString statusText;

    statusText=QString::fromStdString(fname)+QString(": ( %1 x %2 )").arg(currentImage.Size(0)).arg(currentImage.Size(1));

    ui->statusBar->showMessage(statusText);
}

void ViewerMainWindow::on_horizontalSlider_sliderMoved(int position)
{
    QSignalBlocker blockspin(ui->spinBox);
    QSignalBlocker blockslider(ui->horizontalSlider);
    ui->spinBox->setValue(position);

    updateView(m_fname);
}

void ViewerMainWindow::on_spinBox_valueChanged(int arg1)
{
    QSignalBlocker blockspin(ui->spinBox);
    QSignalBlocker blockslider(ui->horizontalSlider);

    ui->horizontalSlider->setValue(arg1);

    updateView(m_fname);
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

        updateView(fileName.toStdString());
        ui->viewer->set_levels(kipl::base::quantile99);
    }
}

void ViewerMainWindow::on_actionSave_as_triggered()
{
    std::ostringstream msg;
    std::string path;
    std::string fname;
    std::vector<std::string> exts;

    kipl::strings::filenames::StripFileName(m_fname,path,fname,exts);

    if (m_ext == readers::ExtensionSEQ)
    {
        SaveAsDialog dlg;
        kipl::io::ViVaSEQHeader header;
        kipl::io::GetViVaSEQHeader(m_fname,&header);

        dlg.setLimits(0,header.numberOfFrames-1);
        dlg.setPath(path,fname+"_#####.tif");

        int res=dlg.exec();

        if (res==QDialog::Accepted)
        {
            std::string fmask,ext;
            int first;
            int last;

            dlg.getDialogInfo(fmask, first, last);

            msg.str(""); msg<<fmask<<", first="<<first<<", last="<<last;
            logger.message(msg.str());
            std::string destname;
            ImageWriter writer;
            kipl::base::TImage<float,2> img;
            for (int i=first; i<=last; ++i)
            {
                kipl::io::ReadViVaSEQ(m_fname,img,i);

                kipl::strings::filenames::MakeFileName(fmask,i,destname,ext,'#','0');
                writer.write(currentImage,destname);
            }
        }
    }
    else
    {
        std::string destname=QFileDialog::getSaveFileName(this,"Save image as",QString::fromStdString(path)).toStdString();

        kipl::io::WriteTIFF(currentImage,destname);
    }
}



void ViewerMainWindow::on_pushButton_showLog_clicked()
{
    if (logdlg.isHidden())
    {
        logdlg.show();
    }
    else
    {
        logdlg.hide();
    }
}


void ViewerMainWindow::on_actionMeasure_pixel_size_triggered()
{
    PixelSizeDlg psdlg(this);

    psdlg.setImage(currentImage);
    psdlg.exec();

}


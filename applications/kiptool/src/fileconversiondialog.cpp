//<LICENSE>

#include <list>
#include <map>
#include <string>
#include <iostream>

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QSignalBlocker>
#include <QtConcurrent>

#include <findskiplistdialog.h>
#include <buildfilelist.h>
#include <imagereader.h>
#include <imagewriter.h>

#include <strings/filenames.h>
#include <strings/string2array.h>

#include <averageimage.h>
#include <readerexception.h>

#include "fileconversiondialog.h"
#include "ui_fileconversiondialog.h"

FileConversionDialog::FileConversionDialog(QWidget *parent) :
    QDialog(parent),
    logger("FileConversionDialog"),
    ui(new Ui::FileConversionDialog)
{
    ui->setupUi(this);
    on_checkBox_toggled(false);
    ui->checkCollate->setChecked(false);
    on_checkCollate_toggled(false);
    ui->comboAverageMethod->setCurrentIndex(3);
    ui->progressBar->setValue(0);
    ui->comboBox_ScanOrder->setCurrentIndex(0);
    on_comboBox_ScanOrder_currentIndexChanged(0);
    ui->lineEdit_DestinationPath->setText("/data");
    ui->lineEdit_DestinationMask->setText("file####.tif");
}

FileConversionDialog::~FileConversionDialog()
{
    delete ui;
}

void FileConversionDialog::on_pushButton_GetSkipList_clicked()
{
    FindSkipListDialog dlg;

    std::list<ImageLoader> loaderlist=ui->ImageLoaderConfig->GetList();

    std::list<std::string> filelist=BuildFileList(loaderlist);

    int res=dlg.exec(filelist);

    if (res==QDialog::Accepted) {
        logger(logger.LogMessage,"Got a skiplist");
        ui->lineEdit_SkipList->setText(dlg.getSkipListString());
    }
}

void FileConversionDialog::on_pushButton_BrowseDestination_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select destination of the converted images",
                                      ui->lineEdit_DestinationPath->text());

    if (!projdir.isEmpty())
        ui->lineEdit_DestinationPath->setText(projdir);
}

void FileConversionDialog::on_pushButton_StartConversion_clicked()
{
    std::ostringstream msg;
    std::string ext1, ext2;

    if (ui->lineEdit_DestinationPath->text().isEmpty()==true) {
        logger(logger.LogWarning,"Empty destination path");
        return;
    }

    if (ui->lineEdit_DestinationMask->text().isEmpty()==true) {
        logger(logger.LogWarning,"Empty destination file mask");
        return;
    }
    filecnt=0;

    std::list<ImageLoader> ll=ui->ImageLoaderConfig->GetList();

    if (ll.empty()==true) {
        logger(logger.LogWarning,"Empty image loader.");
        return;
    }

    ext1=kipl::strings::filenames::GetFileExtension(ll.front().m_sFilemask);

    for (auto it=ll.begin(); it!=ll.end(); it++) {
        if (ext1!=kipl::strings::filenames::GetFileExtension((*it).m_sFilemask)) {
            logger(logger.LogMessage,"The data sets don't have the same file type.");
            return ;
        }
    }

    std::string skipstring=ui->lineEdit_SkipList->text().toStdString();
    std::list<int> skiplist;

    kipl::strings::String2List(skipstring,skiplist);

    std::map<float,std::string> plist=BuildProjectionFileList(ll,skiplist,ui->comboBox_ScanOrder->currentIndex(),ui->comboBox_ScanLength->currentIndex()==0 ? 180.0: 360.0);

    msg.str("");
    msg<<"Projection file list size="<<plist.size()<<" for loader list size="<<ll.size();
    logger(logger.LogDebug,msg.str());


    for (auto it=plist.begin(); it!=plist.end(); it++)
        flist.push_back(it->second);


    msg.str("");
    msg<<"File list size="<<plist.size();
    ui->progressBar->setMaximum(static_cast<int>(flist.size()));
    ui->progressBar->setValue(0);
    logger(logger.LogMessage,msg.str());

    ext2=kipl::strings::filenames::GetFileExtension(ui->lineEdit_DestinationMask->text().toStdString());
    msg.str("");
    msg<<"The input data has ext="<<ext1<<", and will be saved as ext="<<ext2;
    logger(logger.LogDebug,msg.str());

    QFuture<int> progress_thread = QtConcurrent::run(this,&FileConversionDialog::Progress);
    QFuture<int> proc_thread     = QtConcurrent::run(this,&FileConversionDialog::Process, ext1==ext2);

    progress_thread.waitForFinished();
    proc_thread.waitForFinished();

    logger(logger.LogVerbose,"Threads are joined");
}

int FileConversionDialog::Process(bool sameext)
{
    int res=0;
    if ((sameext) &&
        (ui->checkBox->checkState() == Qt::Unchecked) &&
        (ui->checkCollate->checkState() == Qt::Unchecked))
    {
        res=CopyImages();
    }
    else
    {
        res=ConvertImages();
    }

    return res;
}

int FileConversionDialog::CopyImages()
{
    logger(logger.LogMessage,"Copy thread started.");
    std::ostringstream msg;

    std::string destmask=ui->lineEdit_DestinationPath->text().toStdString();

    std::string destname,ext;
    kipl::strings::filenames::CheckPathSlashes(destmask,true);
    destmask+=ui->lineEdit_DestinationMask->text().toStdString();

    msg<<"Will copy "<<flist.size()<<" files into a common sequence at"<<destmask<<".";
    logger(logger.LogMessage,msg.str());

    int destcnt=ui->spinBox_FirstDestinationIndex->value();

    for (auto it=flist.begin(); it!=flist.end(); ++it) {
        kipl::strings::filenames::MakeFileName(destmask,destcnt,destname,ext,'#','0',false);

        if (QFile::copy(QString::fromStdString(*it),QString::fromStdString(destname))==false)
        {
            msg.str("");
            msg<<"Failed to copy "<<*it<<" -> "<<destname;
            logger(logger.LogError,msg.str());
        }
        ++destcnt;
        ++filecnt;
    }
    return 0;
}

int FileConversionDialog::ConvertImages()
{
    logger(logger.LogMessage,"Conversion thread started. The files are written in a common sequence.");

    ImageReader imgreader;
    ImageWriter imgwriter;
    std::string destmask=ui->lineEdit_DestinationPath->text().toStdString();
    std::string ext_out=kipl::strings::filenames::GetFileExtension(destmask);

    std::string destname,ext;
    kipl::strings::filenames::CheckPathSlashes(destmask,true);
    destmask+=ui->lineEdit_DestinationMask->text().toStdString();
    int destcnt=ui->spinBox_FirstDestinationIndex->value();

    kipl::base::TImage<float,2> img;
    kipl::base::TImage<float,3> img3d;
    QMessageBox dlg;
    std::ostringstream msg,errmsg;

    size_t roi[4]={static_cast<size_t>(ui->spinBox_x0->value()),
                  static_cast<size_t>(ui->spinBox_y0->value()),
                  static_cast<size_t>(ui->spinBox_x1->value()),
                  static_cast<size_t>(ui->spinBox_y1->value())};
    size_t *crop=nullptr;
    if (ui->checkBox->checkState()==Qt::Checked) {
        crop=roi;
    }

    const bool bCollate = ui->checkCollate->checkState() == Qt::Checked;
    const int nCollate   = bCollate ? ui->spinCollationSize->value() : 1;

    size_t dims[3];

    imgreader.GetImageSize(flist.front(),1.0f,dims);
    dims[2]=nCollate;

    if (bCollate==true)
        img3d.Resize(dims);

    ImagingAlgorithms::AverageImage avgimg;
    errmsg.str("");
    for (auto it=flist.begin(); it!=flist.end(); ) {

        try {
            if (bCollate==true)
            {
                for (int i=0; (i<nCollate) && (it!=flist.end()); ++i) {
                    img=imgreader.Read(*it,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1,crop);
                    std::copy(img.GetDataPtr(),img.GetDataPtr()+img.Size(),img3d.GetLinePtr(0,i));
                    ++it;
                }
                img=avgimg(img3d,static_cast<ImagingAlgorithms::AverageImage::eAverageMethod>(ui->comboAverageMethod->currentIndex()),nullptr);
            }
            else {
                img=imgreader.Read(*it,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1,crop);
                ++it;
            }
            filecnt+=nCollate;
        }
        catch (kipl::base::KiplException &e) {
            errmsg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
        }
        catch (std::exception &e) {
            errmsg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
        }
        catch (...) {
            errmsg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText("Unhandled exception");
        }
        if (errmsg.str().empty()==false) {
            dlg.exec();
            return -1;
        }

        kipl::strings::filenames::MakeFileName(destmask,destcnt,destname,ext,'#','0',false);
        msg.str("");
        msg<<"Writing "<<destname;
        logger(logger.LogMessage,msg.str());
        errmsg.str("");
        try {
            imgwriter.write(img,destname);
        }
        catch (ReaderException &e) {
            errmsg.str("");
            errmsg<<"Failed to write "<<destname;
            dlg.setText(QString::fromStdString(errmsg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
            logger(logger.LogError,errmsg.str());
        }
        catch (kipl::base::KiplException &e) {
            errmsg.str("");
            errmsg<<"Failed to write "<<destname;
            dlg.setText(QString::fromStdString(errmsg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
            logger(logger.LogError,errmsg.str());
        }
        catch (std::exception &e) {
            errmsg.str("");
            errmsg<<"Failed to write "<<destname;
            dlg.setText(QString::fromStdString(errmsg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
            logger(logger.LogError,errmsg.str());
        }
        catch (...) {
            errmsg.str("");
            errmsg<<"Failed to write "<<destname;
            dlg.setText(QString::fromStdString(errmsg.str()));
            dlg.setDetailedText("Unhandled exception");
            logger(logger.LogError,errmsg.str());
        }
        if (errmsg.str().empty()==false) {
            dlg.exec();
            return -1;
        }

        destcnt++;
    }

    return 0;
}

void FileConversionDialog::on_checkBox_toggled(bool checked)
{
    ui->label_x0->setVisible(checked);
    ui->label_x1->setVisible(checked);
    ui->label_y0->setVisible(checked);
    ui->label_y1->setVisible(checked);

    ui->spinBox_x0->setVisible(checked);
    ui->spinBox_x1->setVisible(checked);
    ui->spinBox_y0->setVisible(checked);
    ui->spinBox_y1->setVisible(checked);
}

void FileConversionDialog::on_checkCollate_toggled(bool checked)
{
    ui->spinCollationSize->setVisible(checked);
    ui->labelAverageMethod->setVisible(checked);
    ui->labelCollationStep->setVisible(checked);
    ui->comboAverageMethod->setVisible(checked);
}

int FileConversionDialog::Progress()
{
    logger(kipl::logging::Logger::LogMessage,"Progress thread is started");

  //  while (filecnt<ui->progressBar->maximum() && !proc_thread.isFinished()){
      while (filecnt<ui->progressBar->maximum()){
        ui->progressBar->setValue(filecnt);

        QThread::msleep(50);
    }

    logger(kipl::logging::Logger::LogMessage,"Progress thread end");

    return 0;

}

void FileConversionDialog::on_buttonBox_rejected()
{
        filecnt=ui->progressBar->maximum()+1;
}

void FileConversionDialog::on_spinCollationSize_valueChanged(int arg1)
{
   std::ostringstream msg;

   std::list<ImageLoader> ll=ui->ImageLoaderConfig->GetList();

   if (ll.empty()==true) {
       logger(logger.LogWarning,"Empty image loader.");
       return;
   }

   std::string skipstring=ui->lineEdit_SkipList->text().toStdString();
   std::list<int> skiplist;

   kipl::strings::String2List(skipstring,skiplist);

   std::map<float,std::string> plist=BuildProjectionFileList(ll,skiplist,ui->comboBox_ScanOrder->currentIndex(),ui->comboBox_ScanLength->currentIndex()==0 ? 180.0: 360.0);

   int N=plist.size();
   int rest = N % arg1;
   msg<<N/arg1<<" files"<<(rest==0 ? "" : " with a rest ")<<rest;
   ui->labelNumberOfFiles->setText(QString::fromStdString(msg.str()));
}

void FileConversionDialog::on_spinCollationSize_editingFinished()
{
    on_spinCollationSize_valueChanged(ui->spinCollationSize->value());
}

void FileConversionDialog::on_comboBox_ScanOrder_currentIndexChanged(int index)
{
    if (0<index) {
        ui->label_scanlength->show();
        ui->comboBox_ScanLength->show();
    }
    else {
        ui->label_scanlength->hide();
        ui->comboBox_ScanLength->hide();
    }
}

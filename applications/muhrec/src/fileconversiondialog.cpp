//<LICENSE>

#include <list>
#include <map>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QSignalBlocker>
#include <QMessageBox>

#include <findskiplistdialog.h>
#include <buildfilelist.h>
#include <roidialog.h>
#include <imagereader.h>
#include <imagewriter.h>

#include <strings/filenames.h>
#include <strings/string2array.h>

#include <averageimage.h>
#include <MorphSpotClean.h>
#include <readerexception.h>

#include "fileconversiondialog.h"
#include "ui_fileconversiondialog.h"

FileConversionDialog::FileConversionDialog(QWidget *parent) :
    QDialog(parent),
    logger("FileConversionDialog"),
    ui(new Ui::FileConversionDialog)
{
    ui->setupUi(this);

    ui->groupBox_combineImages->setChecked(false);
    ui->comboAverageMethod->setCurrentIndex(3);
    ui->progressBar->setValue(0);
    ui->comboBox_ScanOrder->setCurrentIndex(0);
    on_comboBox_ScanOrder_currentIndexChanged(0);
    QDir dir;
    ui->lineEdit_DestinationPath->setText(dir.homePath());
    ui->lineEdit_DestinationMask->setText("img_####.tif");
    ui->widgetROI->setCheckable(true);
    ui->widgetROI->setROIColor("green");

    // Hiding unstable features
    ui->widgetROI->useROIDialog(false);
//    ui->widgetROI->hide();
    ui->widgetROI->setChecked(false);
    ui->lineEdit_SkipList->hide();
    ui->pushButton_GetSkipList->hide();
    ui->label_skiplist->hide();

}

FileConversionDialog::~FileConversionDialog()
{
    delete ui;
}

void FileConversionDialog::on_pushButton_GetSkipList_clicked()
{
    QMessageBox::warning(this,"Feature not supported","Get skip list is currently not supported");
//    FindSkipListDialog dlg;

//    auto loaderlist=ui->ImageLoaderConfig->getList();

//    auto filelist=BuildFileList(loaderlist);

//    int res=dlg.exec(filelist);

//    if (res==QDialog::Accepted) {
//        logger(logger.LogMessage,"Got a skiplist");
//        ui->lineEdit_SkipList->setText(dlg.getSkipListString());
//    }
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

    QDir dir;
    if ((ui->lineEdit_DestinationPath->text().isEmpty()==true) ||
            (!dir.exists(ui->lineEdit_DestinationPath->text())))
    {
        logger(logger.LogWarning,"Empty destination path");
        QMessageBox::warning(this,"No destination path","There is no destination path.");
        return;
    }

    if (ui->lineEdit_DestinationMask->text().isEmpty()==true)
    {
        logger(logger.LogWarning,"Empty destination file mask");
        QMessageBox::warning(this,"No file mask","There is no destination file mask");
        return;
    }
    filecnt=0;
    flist.clear();

    auto ll=ui->ImageLoaderConfig->getList();

    if (ll.empty()==true)
    {
        logger(logger.LogWarning,"Empty image loader.");
        QMessageBox::warning(this,"No files","There are no files in the list");
        return;
    }

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(ll.size());
    ext1=kipl::strings::filenames::GetFileExtension(ll.front().m_sFilemask);

    for (auto it=ll.begin(); it!=ll.end(); it++)
    {
        if (ext1!=kipl::strings::filenames::GetFileExtension((*it).m_sFilemask))
        {
            logger(logger.LogMessage,"The data sets don't have the same file type.");
            QMessageBox::warning(this,"Warning","The input file sets don't have the same file type");

            return ;
        }
    }

    std::string skipstring=ui->lineEdit_SkipList->text().toStdString();
    std::vector<int> skiplist;

    kipl::strings::string2vector(skipstring,skiplist);

    std::map<float,std::string> plist=BuildProjectionFileList(ll,skiplist,
                                                              ui->comboBox_ScanOrder->currentIndex(),
                                                              ui->spinBox_goldenFirstIdx->value(),
                                                              ui->comboBox_ScanLength->currentIndex()==0 ? 180.0: 360.0);

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

    auto progress_thread = std::thread([=,this]{ Progress(); });
    auto process_thread  = std::thread([=,this]{ Process(ext1==ext2); });

    process_thread.join();
    progress_thread.join();

    ui->progressBar->setMaximum(filecnt-1);

    ui->progressBar->setValue(0);

    logger(logger.LogVerbose,"Threads are joined");
}

int FileConversionDialog::Process(bool sameext)
{
    int res=0;

    if ((sameext) &&
        (ui->widgetROI->isChecked() == false) &&
        (ui->groupBox_combineImages->isChecked() == false) &&
        (ui->groupBox_spotclean->isChecked()== false))
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

    for (auto it=flist.begin(); it!=flist.end(); ++it)
    {
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

    std::vector<size_t> roi={0,0,1,1};
    std::vector<size_t> crop;
    if (ui->widgetROI->isChecked() == true)
    {
        logger.message("using ROI");
        ui->widgetROI->getROI(roi);
        crop = roi;
    }

    const bool bCombine  = ui->groupBox_combineImages->isChecked();
    const int  nCombine  = ui->spinCombineSize->value();

    std::vector<size_t> dims(2);

    dims=imgreader.imageSize(flist.front(),1.0f);
    if (!crop.empty())
    {
        dims[0]=crop[2]-crop[0];
        dims[1]=crop[3]-crop[1];
    }

    msg.str(""); msg<<"Img3d size "<<dims[0]<<", "<<dims[1]<<", "<<dims[2]<<", "<<nCombine<< ", "<< (bCombine ?"true" : "false");
    logger.message(msg.str());
    std::vector<size_t> dims3d = {dims[0],dims[1],static_cast<size_t>(nCombine)} ;
    img3d.resize(dims3d);

    msg.str(""); msg<<"Img3d size "<<img3d.Size(0)<<", "<<img3d.Size(1)<<", "<<img3d.Size(2);
    logger.message(msg.str());

    ImagingAlgorithms::AverageImage avgimg;
    ImagingAlgorithms::MorphSpotClean *spotclean=nullptr;

    if (ui->groupBox_spotclean->isChecked())
    {
        spotclean = new ImagingAlgorithms::MorphSpotClean;
        spotclean->setCleanMethod(static_cast<ImagingAlgorithms::eMorphDetectionMethod>(ui->comboBox_spotcleanmethod->currentIndex()),
                                  ImagingAlgorithms::MorphCleanReplace);
        spotclean->setDetectionStrelSize(5);
        spotclean->setThresholdByFraction(true);
        spotclean->useThreading(true);

    }

    errmsg.str("");
    for (auto it=flist.begin(); it!=flist.end(); )
    {

        try
        {
            if (bCombine==true)
            {
                for (int i=0; (i<nCombine) && (it!=flist.end()); ++i)
                {
                    img=imgreader.Read(*it,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1,crop);
//                    img=imgreader.Read(*it,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f);
                    if (spotclean!=nullptr)
                    {
                        spotclean->process(img,ui->doubleSpinBox_spotclean_threshold->value(),0.01);
                    }
                   std::copy_n(img.GetDataPtr(),img.Size(),img3d.GetLinePtr(0,i));
                   ++it;
                }

                img=avgimg(img3d,static_cast<ImagingAlgorithms::AverageImage::eAverageMethod>(ui->comboAverageMethod->currentIndex()));
            }
            else
            {
                img=imgreader.Read(*it,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1,crop);
//                img=imgreader.Read(*it,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1);
                if (spotclean!=nullptr)
                {
                    spotclean->process(img,ui->doubleSpinBox_spotclean_threshold->value(),0.01);
                }
                ++it;
            }
            filecnt+=nCombine;
        }
        catch (ReaderException &e)
        {
            errmsg.str("");
            errmsg<<"Failed to read "<<destname;
            dlg.setText(QString::fromStdString(errmsg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
            logger(logger.LogError,errmsg.str());
        }
        catch (kipl::base::KiplException &e)
        {
            errmsg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
        }
        catch (std::exception &e)
        {
            errmsg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
        }
        catch (...)
        {
            errmsg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText("Unhandled exception");
        }
        if (errmsg.str().empty()==false)
        {
            dlg.exec();
            return -1;
        }

        kipl::strings::filenames::MakeFileName(destmask,destcnt,destname,ext,'#','0',false);
        msg.str("");
        msg<<"Writing "<<destname;
        logger(logger.LogMessage,msg.str());
        errmsg.str("");
        try
        {
            imgwriter.write(img,destname);
        }
        catch (ReaderException &e)
        {
            errmsg.str("");
            errmsg<<"Failed to write "<<destname;
            dlg.setText(QString::fromStdString(errmsg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
            logger(logger.LogError,errmsg.str());
        }
        catch (kipl::base::KiplException &e)
        {
            errmsg.str("");
            errmsg<<"Failed to write "<<destname;
            dlg.setText(QString::fromStdString(errmsg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
            logger(logger.LogError,errmsg.str());
        }
        catch (std::exception &e)
        {
            errmsg.str("");
            errmsg<<"Failed to write "<<destname;
            dlg.setText(QString::fromStdString(errmsg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
            logger(logger.LogError,errmsg.str());
        }
        catch (...)
        {
            errmsg.str("");
            errmsg<<"Failed to write "<<destname;
            dlg.setText(QString::fromStdString(errmsg.str()));
            dlg.setDetailedText("Unhandled exception");
            logger(logger.LogError,errmsg.str());
        }

        if (errmsg.str().empty()==false)
        {
            dlg.exec();
            return -1;
        }

        destcnt++;
    }

    return 0;
}

int FileConversionDialog::Progress()
{
    logger.message("Progress thread is started");

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    while (filecnt<ui->progressBar->maximum())
    {
        ui->progressBar->setValue(filecnt);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    logger.message("Progress thread end");

    return 0;

}

void FileConversionDialog::on_buttonBox_rejected()
{
        filecnt=ui->progressBar->maximum()+1;
}

void FileConversionDialog::on_spinCombineSize_valueChanged(int arg1)
{
   std::ostringstream msg;

   auto ll=ui->ImageLoaderConfig->getList();

   if (ll.empty()==true)
   {
       logger(logger.LogWarning,"Empty image loader.");
       return;
   }

   std::string skipstring=ui->lineEdit_SkipList->text().toStdString();
   std::vector<int> skiplist;

   kipl::strings::string2vector(skipstring,skiplist);

   std::map<float,std::string> plist=BuildProjectionFileList(ll,
                                                             skiplist,
                                                             ui->comboBox_ScanOrder->currentIndex(),
                                                             ui->spinBox_goldenFirstIdx->value(),
                                                             ui->comboBox_ScanLength->currentIndex()==0 ? 180.0: 360.0);

   int N    = static_cast<int>(plist.size());
   int rest = N % arg1;
   msg<<N/arg1<<" files with a rest of "<<rest<<" files";
   ui->labelNumberOfFiles->setText(QString::fromStdString(msg.str()));
}

void FileConversionDialog::on_spinCombineSize_editingFinished()
{
    on_spinCombineSize_valueChanged(ui->spinCombineSize->value());
}

void FileConversionDialog::on_comboBox_ScanOrder_currentIndexChanged(int index)
{
    if (0<index)
    {
        ui->label_scanlength->show();
        ui->comboBox_ScanLength->show();
    }
    else
    {
        ui->label_scanlength->hide();
        ui->comboBox_ScanLength->hide();
    }
}

void FileConversionDialog::on_ImageLoaderConfig_readerListModified()
{
    std::ostringstream msg;

    auto ll=ui->ImageLoaderConfig->getList();
    kipl::base::TImage<float,2> img;
    if (ll.empty()==true)
    {
        logger(logger.LogWarning,"Empty image loader.");

        ui->widgetROI->setSelectionImage(img);
        return;
    }

    FileSet fs=ll.front();

    std::string fname=fs.makeFileName(fs.m_nFirst);

    ImageReader reader;

    img=reader.Read(fname);

    ui->widgetROI->setSelectionImage(img);
}

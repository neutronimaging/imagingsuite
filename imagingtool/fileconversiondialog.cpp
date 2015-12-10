#include <list>
#include <string>

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>

#include <findskiplistdialog.h>
#include <buildfilelist.h>
#include <imagereader.h>
#include <imagewriter.h>

#include <strings/filenames.h>
#include <strings/string2array.h>

#include "fileconversiondialog.h"
#include "ui_fileconversiondialog.h"

FileConversionDialog::FileConversionDialog(QWidget *parent) :
    QDialog(parent),
    logger("FileConversionDialog"),
    ui(new Ui::FileConversionDialog)
{
    ui->setupUi(this);
    on_checkBox_toggled(false);
}

FileConversionDialog::~FileConversionDialog()
{
    delete ui;
}


//void ImagingToolMain::f2t_Preview()
//{
//    UpdateConfig();
//    std::ostringstream msg;

//    std::string fname,ext;

//    std::map<std::string,int> extensions;
//    extensions[".fts"]  = 0;
//    extensions[".fits"] = 0;
//    extensions[".fit"]  = 0;
//    extensions[".fts"]  = 0;
//    extensions[".dat"]  = 1;
//    extensions[".dmp"]  = 1;
//    extensions[".DMP"]  = 1;
//    extensions[".raw"]  = 1;
//    extensions[".bin"]  = 1;

//    try {
//        kipl::strings::filenames::MakeFileName(m_config.fileconv.sSourceMask,
//                                               m_config.fileconv.nFirstSrc,
//                                               fname,
//                                               ext,
//                                               '#',
//                                               '0',m_config.fileconv.bReverseIdx);

//        fname = m_config.fileconv.sSourcePath+fname;
//        Fits2Tif f2t;

//        if (QFile::exists(QString::fromStdString(fname))) {
//            kipl::base::TImage<float> img;
//            std::list<kipl::base::TImage<float,2> > imglist;
//            f2t.GetImage(imglist,fname,m_config.fileconv);

//            msg<<imglist.front()<<std::endl;
//            logger(kipl::logging::Logger::LogMessage,msg.str());
//            img=imglist.front();
//            ui->f2t_imageviewer->set_image(img.GetDataPtr(),img.Dims());
//        }
//        else {
//            msg.str("");
//            msg<<"Could not open file "<<fname;
//            logger(kipl::logging::Logger::LogWarning,msg.str());
//        }
//    }
//    catch (kipl::base::KiplException &e) {
//        std::ostringstream msg;

//        logger(kipl::logging::Logger::LogError, e.what());
//    }
//    catch (...) {
//        logger(kipl::logging::Logger::LogError, "An error occured during reading showing preview");

//    }


//}


//void ImagingToolMain::f2t_GetROI()
//{
//    QRect ROI = ui->f2t_imageviewer->get_marked_roi();

//    ui->f2t_spin_x0->setValue(ROI.x());
//    ui->f2t_spin_y0->setValue(ROI.y());
//    ui->f2t_spin_x1->setValue(ROI.x()+ROI.width());
//    ui->f2t_spin_y1->setValue(ROI.y()+ROI.height());

//    ui->f2t_imageviewer->set_rectangle(ROI,QColor(Qt::green),0);
//}


//void ImagingToolMain::f2t_Convert()
//{

//    std::ostringstream msg;

//    UpdateConfig();
//    SaveConfig();


//    Fits2Tif f2t;

//    try {
//        f2t.process(m_config.fileconv);
//    }
//    catch (kipl::base::KiplException &e) {
//        QMessageBox dlg;

//        dlg.setText("Fits to tif conversion failed");
//        dlg.setDetailedText(QString::fromStdString(e.what()));

//        dlg.exec();
//    }
//}

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

    std::list<ImageLoader> ll=ui->ImageLoaderConfig->GetList();
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

    std::list<std::string> flist=BuildFileList(ll,skiplist);

    ext2=kipl::strings::filenames::GetFileExtension(ui->lineEdit_DestinationMask->text().toStdString());
    msg.str("");
    msg<<"The input data has ext="<<ext1<<", and will be saved as ext="<<ext2;
    logger(logger.LogMessage,msg.str());
    if ((ext2==ext1) && (ui->checkBox->checkState() == Qt::Unchecked))
    {
        CopyImages(flist);
    }
    else {
        ConvertImages(flist);
    }

}

void FileConversionDialog::CopyImages(std::list<std::string> &flist)
{
    logger(logger.LogMessage,"Will copy the files in a common sequence.");

    std::string destmask=ui->lineEdit_DestinationPath->text().toStdString();
    std::string destname,ext;
    kipl::strings::filenames::CheckPathSlashes(destmask,true);
    destmask+=ui->lineEdit_DestinationMask->text().toStdString();
    int destcnt=ui->spinBox_FirstDestinationIndex->value();

    for (auto it=flist.begin(); it!=flist.end(); it++) {
        kipl::strings::filenames::MakeFileName(destmask,destcnt,destname,ext,'#','0',false);

        QFile::copy(QString::fromStdString(*it),QString::fromStdString(destname));
        destcnt++;
    }
}

void FileConversionDialog::ConvertImages(std::list<std::string> &flist)
{
    logger(logger.LogMessage,"Will convert the files in a common sequence.");

    ImageReader imgreader;
    ImageWriter imgwriter;
    std::string destmask=ui->lineEdit_DestinationPath->text().toStdString();
    std::string ext_out=kipl::strings::filenames::GetFileExtension(destmask);

    std::string destname,ext;
    kipl::strings::filenames::CheckPathSlashes(destmask,true);
    destmask+=ui->lineEdit_DestinationMask->text().toStdString();
    int destcnt=ui->spinBox_FirstDestinationIndex->value();

    kipl::base::TImage<float,2> img;
    QMessageBox dlg;
    std::ostringstream msg;

    size_t roi[4]={static_cast<size_t>(ui->spinBox_x0->value()),
                  static_cast<size_t>(ui->spinBox_y0->value()),
                  static_cast<size_t>(ui->spinBox_x1->value()),
                  static_cast<size_t>(ui->spinBox_y1->value())};
    size_t *crop=nullptr;
    if (ui->checkBox->checkState()==Qt::Checked) {
        crop=roi;
    }


    for (auto it=flist.begin(); it!=flist.end(); it++) {
        try {
            img=imgreader.Read(*it,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1,crop);
        }
        catch (kipl::base::KiplException &e) {
            msg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
        }
        catch (std::exception &e) {
            msg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
        }
        catch (...) {
            msg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText("Unhandled exception");
        }

        kipl::strings::filenames::MakeFileName(destmask,destcnt,destname,ext,'#','0',false);

        try {
            imgwriter.write(img,destname);
        }
        catch (kipl::base::KiplException &e) {
            msg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
        }
        catch (std::exception &e) {
            msg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText(QString::fromStdString(e.what()));
        }
        catch (...) {
            msg.str("");
            msg<<"Failed to read "<<*it;
            dlg.setText(QString::fromStdString(msg.str()));
            dlg.setDetailedText("Unhandled exception");
        }

        destcnt++;
    }
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

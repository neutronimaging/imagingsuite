#include <list>
#include <string>

#include <findskiplistdialog.h>
#include <buildfilelist.h>


#include "fileconversiondialog.h"
#include "ui_fileconversiondialog.h"


FileConversionDialog::FileConversionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileConversionDialog)
{
    ui->setupUi(this);
}

FileConversionDialog::~FileConversionDialog()
{
    delete ui;
}


//void ImagingToolMain::f2t_BrowseSrcPath()
//{
//    QString projdir=QFileDialog::getOpenFileName(this,
//                                      "Select location of the images",
//                                      ui->f2t_edit_srcfilemask->text());
//    if (!projdir.isEmpty()) {
//        std::string pdir=projdir.toStdString();

//        #ifdef _MSC_VER
//        const char slash='\\';
//        #else
//        const char slash='/';
//        #endif
//        ptrdiff_t pos=pdir.find_last_of(slash);

//        QString path(QString::fromStdString(pdir.substr(0,pos+1)));
//        std::string fname=pdir.substr(pos+1);
//        kipl::io::DirAnalyzer da;
//        kipl::io::FileItem fi=da.GetFileMask(pdir);

//    }
//}

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

//void ImagingToolMain::f2t_FindProjections()
//{
//    FindSkipListDialog dlg;
//    UpdateConfig();

//    std::string fname,ext;

//    kipl::strings::filenames::MakeFileName(m_config.fileconv.sSourceMask,
//                                           m_config.fileconv.nFirstSrc,
//                                           fname,
//                                           ext,
//                                           '#',
//                                           '0');

//    fname = m_config.fileconv.sSourcePath+fname;

//    if (QFile::exists(QString::fromStdString(fname))) {


//        int res = dlg.exec(m_config.fileconv.sSourcePath,m_config.fileconv.sSourceMask, m_config.fileconv.nFirstSrc, m_config.fileconv.nLastSrc);

//        if ( res == QDialog::Accepted )
//        {
//            logger(kipl::logging::Logger::LogMessage,"Accepted skiplist");
//            ui->f2t_edit_skiplist->setText(dlg.getSkipList());
//        }
//    }
//    else {
//        QMessageBox dlg;

//        dlg.setText("Please enter a valid path and file mask");
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

//void ImagingToolMain::f2t_TakePath()
//{
////    ui->f2t_edit_srcfilemask->setText(ui->f2t_edit_srcpath->text());
//}

//void ImagingToolMain::f2t_BrowseDestPath()
//{
//    QString projdir=QFileDialog::getExistingDirectory(this,
//                                      "Select destination of converted the projections",
//                                      ui->f2t_edit_destpath->text());

//    if (!projdir.isEmpty())
//        ui->f2t_edit_destpath->setText(projdir);
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

    }
}

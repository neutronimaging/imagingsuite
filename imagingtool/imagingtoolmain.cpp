#include <QFileDialog>
#include <QMessageBox>
#include "imagingtoolmain.h"
#include "ui_imagingtoolmain.h"
#include "findskiplistdialog.h"
#include "Fits2Tif.h"
#include "Reslicer.h"
#include "mergevolumesdialog.h"
#include <base/kiplenums.h>
#include <strings/string2array.h>
#include <strings/filenames.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
#include <io/io_generic.h>
#include <io/DirAnalyzer.h>
#include <base/timage.h>


#include <sstream>
#include <fstream>

ImagingToolMain::ImagingToolMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ImagingToolMain),
    logger("ImagingToolMain")
{
    ui->setupUi(this);
    logger.AddLogTarget(*ui->logviewer);
    LoadConfig();
    UpdateDialog();
    connect(ui->f2t_button_browsesrcpath,   SIGNAL(clicked()),this,SLOT(f2t_BrowseSrcPath()));
    connect(ui->f2t_button_preview,         SIGNAL(clicked()),this,SLOT(f2t_Preview()));
    connect(ui->f2t_button_findprojections, SIGNAL(clicked()),this,SLOT(f2t_FindProjections()));
    connect(ui->f2t_button_getroi,          SIGNAL(clicked()),this,SLOT(f2t_GetROI()));
    connect(ui->f2t_button_takepath,        SIGNAL(clicked()),this,SLOT(f2t_TakePath()));
    connect(ui->f2t_button_browsedestpath,  SIGNAL(clicked()),this,SLOT(f2t_BrowseDestPath()));
    connect(ui->f2t_button_convert,         SIGNAL(clicked()),this,SLOT(f2t_Convert()));

}

ImagingToolMain::~ImagingToolMain()
{
    delete ui;
}



void ImagingToolMain::f2t_BrowseSrcPath()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select location of the images",
                                      ui->f2t_edit_srcfilemask->text());
    if (!projdir.isEmpty()) {
        std::string pdir=projdir.toStdString();

        #ifdef _MSC_VER
        const char slash='\\';
        #else
        const char slash='/';
        #endif
        ptrdiff_t pos=pdir.find_last_of(slash);

        QString path(QString::fromStdString(pdir.substr(0,pos+1)));
        std::string fname=pdir.substr(pos+1);
        kipl::io::DirAnalyzer da;
        kipl::io::FileItem fi=da.GetFileMask(pdir);

        ui->f2t_edit_srcfilemask->setText(QString::fromStdString(fi.m_sMask));
    }
}

void ImagingToolMain::f2t_Preview()
{
    UpdateConfig();
    std::ostringstream msg;

    std::string fname,ext;

    std::map<std::string,int> extensions;
    extensions[".fts"]  = 0;
    extensions[".fits"] = 0;
    extensions[".fit"]  = 0;
    extensions[".fts"]  = 0;
    extensions[".dat"]  = 1;
    extensions[".dmp"]  = 1;
    extensions[".DMP"]  = 1;
    extensions[".raw"]  = 1;
    extensions[".bin"]  = 1;

    try {
        kipl::strings::filenames::MakeFileName(m_config.fileconv.sSourceMask,
                                               m_config.fileconv.nFirstSrc,
                                               fname,
                                               ext,
                                               '#',
                                               '0',m_config.fileconv.bReverseIdx);

        fname = m_config.fileconv.sSourcePath+fname;
        Fits2Tif f2t;

        if (QFile::exists(QString::fromStdString(fname))) {
            kipl::base::TImage<float> img;
            std::list<kipl::base::TImage<float,2> > imglist;
            f2t.GetImage(imglist,fname,m_config.fileconv);

            msg<<imglist.front()<<std::endl;
            logger(kipl::logging::Logger::LogMessage,msg.str());
            img=imglist.front();
            ui->f2t_imageviewer->set_image(img.GetDataPtr(),img.Dims());
        }
        else {
            msg.str("");
            msg<<"Could not open file "<<fname;
            logger(kipl::logging::Logger::LogWarning,msg.str());
        }
    }
    catch (kipl::base::KiplException &e) {
        std::ostringstream msg;

        logger(kipl::logging::Logger::LogError, e.what());
    }
    catch (...) {
        logger(kipl::logging::Logger::LogError, "An error occured during reading showing preview");

    }


}

void ImagingToolMain::f2t_FindProjections()
{
    FindSkipListDialog dlg;
    UpdateConfig();

    std::string fname,ext;

    kipl::strings::filenames::MakeFileName(m_config.fileconv.sSourceMask,
                                           m_config.fileconv.nFirstSrc,
                                           fname,
                                           ext,
                                           '#',
                                           '0');

    fname = m_config.fileconv.sSourcePath+fname;

    if (QFile::exists(QString::fromStdString(fname))) {


        int res = dlg.exec(m_config.fileconv.sSourcePath,m_config.fileconv.sSourceMask, m_config.fileconv.nFirstSrc, m_config.fileconv.nLastSrc);

        if ( res == QDialog::Accepted )
        {
            logger(kipl::logging::Logger::LogMessage,"Accepted skiplist");
            ui->f2t_edit_skiplist->setText(dlg.getSkipList());
        }
    }
    else {
        QMessageBox dlg;

        dlg.setText("Please enter a valid path and file mask");
    }
}

void ImagingToolMain::f2t_GetROI()
{
    QRect ROI = ui->f2t_imageviewer->get_marked_roi();

    ui->f2t_spin_x0->setValue(ROI.x());
    ui->f2t_spin_y0->setValue(ROI.y());
    ui->f2t_spin_x1->setValue(ROI.x()+ROI.width());
    ui->f2t_spin_y1->setValue(ROI.y()+ROI.height());

    ui->f2t_imageviewer->set_rectangle(ROI,QColor(Qt::green),0);
}

void ImagingToolMain::f2t_TakePath()
{
//    ui->f2t_edit_srcfilemask->setText(ui->f2t_edit_srcpath->text());
}

void ImagingToolMain::f2t_BrowseDestPath()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select destination of converted the projections",
                                      ui->f2t_edit_destpath->text());

    if (!projdir.isEmpty())
        ui->f2t_edit_destpath->setText(projdir);
}

void ImagingToolMain::f2t_Convert()
{

    std::ostringstream msg;

    UpdateConfig();
    SaveConfig();


    Fits2Tif f2t;

    try {
        f2t.process(m_config.fileconv);
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox dlg;

        dlg.setText("Fits to tif conversion failed");
        dlg.setDetailedText(QString::fromStdString(e.what()));

        dlg.exec();
    }
}

void ImagingToolMain::UpdateDialog()
{
    std::ostringstream str;

    // Reslicer
    ui->reslice_check_XZ->setChecked(m_config.reslice.bResliceXZ);
    ui->reslice_check_YZ->setChecked(m_config.reslice.bResliceYZ);
    ui->reslice_edit_FileMask->setText(QString::fromStdString(m_config.reslice.sSourceMask));
    ui->reslice_edit_OutPath->setText(QString::fromStdString(m_config.reslice.sDestinationPath));
    ui->reslice_spin_FirstImage->setValue(m_config.reslice.nFirst);
    ui->reslice_spin_LastImage->setValue(m_config.reslice.nLast);
    ui->reslice_spin_firstXZ->setValue(m_config.reslice.nFirstXZ);
    ui->reslice_spin_lastXZ->setValue(m_config.reslice.nLastXZ);
    ui->reslice_spin_firstYZ->setValue(m_config.reslice.nFirstYZ);
    ui->reslice_spin_lastYZ->setValue(m_config.reslice.nLastYZ);

    // Fits 2 TIFF
    ui->f2t_edit_srcfilemask->setText(QString::fromStdString(m_config.fileconv.sSourceMask));

    ui->f2t_edit_destpath->setText(QString::fromStdString(m_config.fileconv.sDestPath));
    ui->f2t_edit_destfilemask->setText(QString::fromStdString(m_config.fileconv.sDestMask));

    ui->f2t_spin_firstprojection->setValue(m_config.fileconv.nFirstSrc);
    ui->f2t_spin_lastprojection->setValue(m_config.fileconv.nLastSrc);
    ui->f2t_spin_destfirstindex->setValue(m_config.fileconv.nFirstDest);
    ui->f2t_check_reverseidx->setChecked(m_config.fileconv.bReverseIdx);

    ui->f2t_combo_mirror->setCurrentIndex(m_config.fileconv.flip);
    ui->f2t_combo_rotate->setCurrentIndex(m_config.fileconv.rotate);
    ui->f2t_check_fixzeros->setChecked(m_config.fileconv.bReplaceZeros);

    ui->f2t_check_useroi->setChecked(m_config.fileconv.bCrop);
    ui->f2t_spin_x0->setValue(m_config.fileconv.nCrop[0]);
    ui->f2t_spin_y0->setValue(m_config.fileconv.nCrop[1]);
    ui->f2t_spin_x1->setValue(m_config.fileconv.nCrop[2]);
    ui->f2t_spin_y1->setValue(m_config.fileconv.nCrop[3]);
    ui->f2t_spin_spotthreshold->setValue((m_config.fileconv.fSpotThreshold));
    ui->f2t_check_cleanspots->setChecked(m_config.fileconv.bUseSpotClean);
    ui->f2t_check_sortgolden->setChecked(m_config.fileconv.bSortGoldenScan);
    ui->f2t_combo_goldenscan->setCurrentIndex(m_config.fileconv.nGoldenScanArc);

    ui->f2t_spin_stride->setValue(m_config.fileconv.nStride);
    ui->f2t_spin_raw_offset->setValue(m_config.fileconv.nReadOffset);
    ui->f2t_spin_imgperfile->setValue(m_config.fileconv.nImagesPerFile);
    ui->f2t_spin_raw_sizex->setValue(m_config.fileconv.nImgSizeX);
    ui->f2t_spin_raw_sizey->setValue(m_config.fileconv.nImgSizeY);
    ui->f2t_spin_imgidx->setValue(m_config.fileconv.nImageIndex);
    ui->f2t_check_reverseidx->setChecked(m_config.fileconv.bReverseIdx);

    str.str("");
    std::set<size_t>::iterator it;
    for (it=m_config.fileconv.skip_list.begin(); it!=m_config.fileconv.skip_list.end(); it++)
        str<<*it<<" ";
    ui->f2t_edit_skiplist->setText(QString::fromStdString(str.str()));
}


void ImagingToolMain::UpdateConfig()
{
    std::ostringstream str;
    // Reslicer
    m_config.reslice.bResliceXZ  = ui->reslice_check_XZ->checkState();
    m_config.reslice.bResliceYZ  = ui->reslice_check_YZ->checkState();
    m_config.reslice.sSourcePath="";
    m_config.reslice.sSourceMask = ui->reslice_edit_FileMask->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_config.reslice.sSourceMask,false);
    m_config.reslice.sDestinationPath = ui->reslice_edit_OutPath->text().toStdString();
    m_config.reslice.nFirst = ui->reslice_spin_FirstImage->value();
    m_config.reslice.nLast = ui->reslice_spin_LastImage->value();
    m_config.reslice.nFirstXZ = ui->reslice_spin_firstXZ->value();
    m_config.reslice.nLastXZ = ui->reslice_spin_lastXZ->value();
    m_config.reslice.nFirstYZ = ui->reslice_spin_firstYZ->value();
    m_config.reslice.nLastYZ = ui->reslice_spin_lastYZ->value();

    // Fits 2 TIFF
    m_config.fileconv.sSourcePath = "";

    m_config.fileconv.sSourceMask = ui->f2t_edit_srcfilemask->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_config.fileconv.sSourceMask,false);

    m_config.fileconv.sDestPath   = ui->f2t_edit_destpath->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_config.fileconv.sDestPath,true);
    m_config.fileconv.sDestMask   = ui->f2t_edit_destfilemask->text().toStdString();

    m_config.fileconv.nFirstSrc   = ui->f2t_spin_firstprojection->value();
    m_config.fileconv.nLastSrc    = ui->f2t_spin_lastprojection->value();
    m_config.fileconv.nFirstDest  = ui->f2t_spin_destfirstindex->value();

    m_config.fileconv.flip        = static_cast<kipl::base::eImageFlip>(ui->f2t_combo_mirror->currentIndex());
    m_config.fileconv.rotate      = static_cast<kipl::base::eImageRotate>(ui->f2t_combo_rotate->currentIndex());
    m_config.fileconv.bReplaceZeros = ui->f2t_check_fixzeros->checkState();

    m_config.fileconv.bCrop       = ui->f2t_check_useroi->checkState();
    m_config.fileconv.nCrop[0]    = ui->f2t_spin_x0->value();
    m_config.fileconv.nCrop[1]    = ui->f2t_spin_y0->value();
    m_config.fileconv.nCrop[2]    = ui->f2t_spin_x1->value();
    m_config.fileconv.nCrop[3]    = ui->f2t_spin_y1->value();

    m_config.fileconv.fSpotThreshold = ui->f2t_spin_spotthreshold->value();
    m_config.fileconv.bUseSpotClean  = ui->f2t_check_cleanspots->checkState();

    m_config.fileconv.bSortGoldenScan = ui->f2t_check_sortgolden->checkState();
    m_config.fileconv.nGoldenScanArc = ui->f2t_combo_goldenscan->currentIndex();

    m_config.fileconv.nStride = ui->f2t_spin_stride->value();
    m_config.fileconv.nReadOffset = ui->f2t_spin_raw_offset->value();
    m_config.fileconv.nImagesPerFile = ui->f2t_spin_imgperfile->value();
    m_config.fileconv.nImgSizeX = ui->f2t_spin_raw_sizex->value();
    m_config.fileconv.nImgSizeY = ui->f2t_spin_raw_sizey->value();
    m_config.fileconv.nImageIndex = ui->f2t_spin_imgidx->value();
    m_config.fileconv.bReverseIdx = ui->f2t_check_reverseidx->checkState();

    kipl::strings::String2Set(ui->f2t_edit_skiplist->text().toStdString(), m_config.fileconv.skip_list);

}

void ImagingToolMain::SaveConfig()
{
   QString fname;
   QDir dir;

   fname=dir.homePath()+"/.imagingtools";
   if (!dir.exists(fname)) {
     dir.mkdir(fname);
   }

   fname=fname + "/imagingtool_config.xml";
   std::ofstream ofile(fname.toStdString().c_str());

   ofile<<m_config.WriteXML();

}

void ImagingToolMain::LoadConfig()
{
    QString fname=QDir::homePath()+"/.imagingtools/imagingtool_config.xml";
    if (QFile::exists(fname)) {
        m_config.LoadConfigFile(fname.toStdString());
    }
}


void ImagingToolMain::on_reslice_button_BrowseInPath_clicked()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select location of the slices",
                                      ui->reslice_edit_FileMask->text());
    if (!projdir.isEmpty()) {
        std::string pdir=projdir.toStdString();

        #ifdef _MSC_VER
        const char slash='\\';
        #else
        const char slash='/';
        #endif
        ptrdiff_t pos=pdir.find_last_of(slash);

        QString path(QString::fromStdString(pdir.substr(0,pos+1)));
        std::string fname=pdir.substr(pos+1);
        kipl::io::DirAnalyzer da;
        kipl::io::FileItem fi=da.GetFileMask(pdir);

        ui->reslice_edit_FileMask->setText(QString::fromStdString(fi.m_sMask));
    }
}

void ImagingToolMain::on_reslice_button_BrowseOutPath_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location of the projections",
                                      ui->reslice_edit_OutPath->text());

    if (!projdir.isEmpty())
        ui->reslice_edit_OutPath->setText(projdir);
}

void ImagingToolMain::on_reslice_button_process_clicked()
{

    UpdateConfig();
    SaveConfig();

    TIFFReslicer reslicer;
    std::string sSrcMask=m_config.reslice.sSourcePath+m_config.reslice.sSourceMask;
    std::ostringstream msg;

    std::string sDstMask=m_config.reslice.sDestinationPath+m_config.reslice.sSourceMask;
    std::string a,b;
    size_t pos=sDstMask.find('#',0);
    if (pos==string::npos) {
        QMessageBox dlg;

        dlg.setText("Failed to create destination filename.\n There are no # in the file mask.");
        dlg.exec();
        return;
    }
    a=sDstMask.substr(0,pos);
    b=sDstMask.substr(pos);

    if (m_config.reslice.bResliceXZ) {
        sDstMask=a+"XZ"+b;
        logger(kipl::logging::Logger::LogMessage,sDstMask);
        try {
        reslicer.process(sSrcMask,
            m_config.reslice.nFirst,
            m_config.reslice.nLast,
            sDstMask, kipl::base::ImagePlaneXZ);
        }
        catch (kipl::base::KiplException &E) {
            msg.str("");
            msg<<"XZ-Reslicing failed with an exception\n"<<E.what();
            logger(kipl::logging::Logger::LogError,msg.str());
            QMessageBox dlg;
            dlg.setText("XZ-Reslicing failed");
            dlg.exec();
            return;
        }
        logger(kipl::logging::Logger::LogMessage,"Reslice XZ done.");
    }

    if (m_config.reslice.bResliceYZ) {
        sDstMask=a+"YZ"+b;
        logger(kipl::logging::Logger::LogMessage,sDstMask);
        try {
        reslicer.process(sSrcMask,
            m_config.reslice.nFirst,
            m_config.reslice.nLast,
            sDstMask, kipl::base::ImagePlaneYZ);
        }
        catch (kipl::base::KiplException &E) {
            msg.str("");
            msg<<"YZ-Reslicing failed with an exception\n"<<E.what();
            logger(kipl::logging::Logger::LogError,msg.str());
            QMessageBox dlg;
            dlg.setText("YZ-Reslicing failed");
            dlg.exec();
            return;
        }
        logger(kipl::logging::Logger::LogMessage,"Reslice YZ done.");

    }

}

void ImagingToolMain::on_actionMerge_volume_triggered()
{
    MergeVolumesDialog dlg;

    dlg.exec();
    std::cout<<"Hepp"<<std::endl;
}

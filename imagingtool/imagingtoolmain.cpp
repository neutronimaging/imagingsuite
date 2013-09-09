#include <QFileDialog>
#include <QMessageBox>
#include "imagingtoolmain.h"
#include "ui_imagingtoolmain.h"
#include "findskiplistdialog.h"
#include "Fits2Tif.h"
#include <base/kiplenums.h>
#include <strings/string2array.h>
#include <strings/filenames.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
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
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location of the projections",
                                      ui->f2t_edit_srcpath->text());

    if (!projdir.isEmpty())
        ui->f2t_edit_srcpath->setText(projdir);
}

void ImagingToolMain::f2t_Preview()
{
    UpdateConfig();

    std::string fname,ext;

    kipl::strings::filenames::MakeFileName(m_config.fits2tif.sSourceMask,
                                           m_config.fits2tif.nFirstSrc,
                                           fname,
                                           ext,
                                           '#',
                                           '0');

    fname = m_config.fits2tif.sSourcePath+fname;

    if (QFile::exists(QString::fromStdString(fname))) {
        kipl::base::TImage<float,2> img;
        kipl::io::ReadFITS(img,fname.c_str(),NULL);
        ui->f2t_imageviewer->set_image(img.GetDataPtr(),img.Dims());
    }
    else {
        logger(kipl::logging::Logger::LogWarning,"File does not exist");
    }


}

void ImagingToolMain::f2t_FindProjections()
{
    FindSkipListDialog dlg;
    UpdateConfig();

    std::string fname,ext;

    kipl::strings::filenames::MakeFileName(m_config.fits2tif.sSourceMask,
                                           m_config.fits2tif.nFirstSrc,
                                           fname,
                                           ext,
                                           '#',
                                           '0');

    fname = m_config.fits2tif.sSourcePath+fname;

    if (QFile::exists(QString::fromStdString(fname))) {


        int res = dlg.exec(m_config.fits2tif.sSourcePath,m_config.fits2tif.sSourceMask, m_config.fits2tif.nFirstSrc, m_config.fits2tif.nLastSrc);

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
    ui->f2t_edit_destpath->setText(ui->f2t_edit_srcpath->text());
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
        f2t.process(m_config.fits2tif);
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

    // Fits 2 TIFF
    ui->f2t_edit_srcpath->setText(QString::fromStdString(m_config.fits2tif.sSourcePath));
    ui->f2t_edit_srcfilemask->setText(QString::fromStdString(m_config.fits2tif.sSourceMask));

    ui->f2t_edit_destpath->setText(QString::fromStdString(m_config.fits2tif.sDestPath));
    ui->f2t_edit_destfilemask->setText(QString::fromStdString(m_config.fits2tif.sDestMask));

    ui->f2t_spin_firstprojection->setValue(m_config.fits2tif.nFirstSrc);
    ui->f2t_spin_lastprojection->setValue(m_config.fits2tif.nLastSrc);
    ui->f2t_spin_destfirstindex->setValue(m_config.fits2tif.nFirstDest);

    ui->f2t_combo_mirror->setCurrentIndex(m_config.fits2tif.flip);
    ui->f2t_combo_rotate->setCurrentIndex(m_config.fits2tif.rotate);
    ui->f2t_check_fixzeros->setChecked(m_config.fits2tif.bReplaceZeros);

    ui->f2t_check_useroi->setChecked(m_config.fits2tif.bCrop);
    ui->f2t_spin_x0->setValue(m_config.fits2tif.nCrop[0]);
    ui->f2t_spin_y0->setValue(m_config.fits2tif.nCrop[1]);
    ui->f2t_spin_x1->setValue(m_config.fits2tif.nCrop[2]);
    ui->f2t_spin_y1->setValue(m_config.fits2tif.nCrop[3]);

    str.str("");
    std::set<size_t>::iterator it;
    for (it=m_config.fits2tif.skip_list.begin(); it!=m_config.fits2tif.skip_list.end(); it++)
        str<<*it<<" ";
    ui->f2t_edit_skiplist->setText(QString::fromStdString(str.str()));
}


void ImagingToolMain::UpdateConfig()
{
    std::ostringstream str;

    // Fits 2 TIFF
    m_config.fits2tif.sSourcePath = ui->f2t_edit_srcpath->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_config.fits2tif.sSourcePath,true);
    m_config.fits2tif.sSourceMask = ui->f2t_edit_srcfilemask->text().toStdString();

    m_config.fits2tif.sDestPath   = ui->f2t_edit_destpath->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_config.fits2tif.sDestPath,true);
    m_config.fits2tif.sDestMask   = ui->f2t_edit_destfilemask->text().toStdString();

    m_config.fits2tif.nFirstSrc   = ui->f2t_spin_firstprojection->value();
    m_config.fits2tif.nLastSrc    = ui->f2t_spin_lastprojection->value();
    m_config.fits2tif.nFirstDest  = ui->f2t_spin_destfirstindex->value();

    m_config.fits2tif.flip        = static_cast<kipl::base::eImageFlip>(ui->f2t_combo_mirror->currentIndex());
    m_config.fits2tif.rotate      = static_cast<kipl::base::eImageRotate>(ui->f2t_combo_rotate->currentIndex());
    m_config.fits2tif.bReplaceZeros = ui->f2t_check_fixzeros->checkState();

    m_config.fits2tif.bCrop       = ui->f2t_check_useroi->checkState();
    m_config.fits2tif.nCrop[0]    = ui->f2t_spin_x0->value();
    m_config.fits2tif.nCrop[1]    = ui->f2t_spin_y0->value();
    m_config.fits2tif.nCrop[2]    = ui->f2t_spin_x1->value();
    m_config.fits2tif.nCrop[3]    = ui->f2t_spin_y1->value();

    kipl::strings::String2Set(ui->f2t_edit_skiplist->text().toStdString(), m_config.fits2tif.skip_list);

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
   std::ofstream ofile(fname.toAscii());

   ofile<<m_config.WriteXML();

}

void ImagingToolMain::LoadConfig()
{
    QString fname=QDir::homePath()+"/.imagingtools/imagingtool_config.xml";
    if (QFile::exists(fname)) {
        m_config.LoadConfigFile(fname.toStdString());
    }
}

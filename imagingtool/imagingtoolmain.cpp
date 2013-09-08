#include <QFileDialog>
#include "imagingtoolmain.h"
#include "ui_imagingtoolmain.h"
#include "findskiplistdialog.h"
#include <sstream>

ImagingToolMain::ImagingToolMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ImagingToolMain),
    logger("ImagingToolMain")
{
    ui->setupUi(this);
    logger.AddLogTarget(*ui->logviewer);
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

}

void ImagingToolMain::f2t_FindProjections()
{
    FindSkipListDialog dlg;

    int res = dlg.exec();

    if ( res == QDialog::Accepted )
    {
        logger(kipl::logging::Logger::LogMessage,"Accepted skiplist");
    }

}

void ImagingToolMain::f2t_GetROI()
{

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

    str.str("");
    std::set<size_t>::iterator it;
    for (it=m_config.fits2tif.skip_list.begin(); it!=m_config.fits2tif.skip_list.end(); it++)
        str<<*it<<" ";
    ui->f2t_edit_skiplist->setText(QString::fromStdString(str.str()));


}


void ImagingToolMain::UpdateConfig()
{

}

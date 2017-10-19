
#include <sstream>
#include <fstream>

#include <QFileDialog>
#include <QMessageBox>

#include <base/kiplenums.h>
#include <strings/string2array.h>
#include <strings/filenames.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
#include <io/io_generic.h>
#include <io/DirAnalyzer.h>
#include <base/timage.h>

#include "imagingtoolmain.h"
#include "ui_imagingtoolmain.h"
#include "findskiplistdialog.h"
#include "Fits2Tif.h"
#include "reslicerdialog.h"
#include "mergevolumesdialog.h"
#include "fileconversiondialog.h"
#include "genericconversion.h"

ImagingToolMain::ImagingToolMain(QWidget *parent) :
    QMainWindow(parent),
    logger("ImagingToolMain"),
    ui(new Ui::ImagingToolMain)
{
    ui->setupUi(this);
    logger.AddLogTarget(*ui->logviewer);
    LoadConfig();
    UpdateDialog();
}

ImagingToolMain::~ImagingToolMain()
{
    delete ui;
}




void ImagingToolMain::UpdateDialog()
{
    std::ostringstream str;

}


void ImagingToolMain::UpdateConfig()
{
    std::ostringstream str;
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

void ImagingToolMain::on_actionMerge_volume_triggered()
{
    MergeVolumesDialog dlg;

    logger(logger.LogMessage,"Opening merge volume dialog");
    dlg.exec();
}

void ImagingToolMain::on_actionReslice_triggered()
{
    ReslicerDialog dlg;

    logger(logger.LogMessage,"Opening reslice dialog");
    dlg.exec();
}

void ImagingToolMain::on_actionConvertFiles_triggered()
{
    FileConversionDialog dlg;

    logger(logger.LogMessage,"Opening file conversion dialog");
    dlg.exec();

}

void ImagingToolMain::on_actionGeneric_to_TIFF_triggered()
{
    GenericConversion dlg;
    logger(logger.LogMessage,"Opening generic file conversion dialog");

    dlg.exec();
}

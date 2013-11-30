#include <QDir>
#include <QFileDialog>
#include <QMessageBox>


#include <ModuleException.h>
#include <utilities/TimeDate.h>

#include "kiptoolmainwindow.h"
#include "ui_kiptoolmainwindow.h"
#include "confighistorydialog.h"

#include "ImageIO.h"

KipToolMainWindow::KipToolMainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("KipToolMainWindow"),
    ui(new Ui::KipToolMainWindow),
    m_sFileName("noname.xml")
{
    ui->setupUi(this);
    logger.AddLogTarget(*(ui->widget_logviewer));

    ui->widget_moduleconfigurator->configure("kiptool",QDir::currentPath().toStdString());
    LoadDefaults();
    UpdateDialog();
    SetupCallbacks();
}

KipToolMainWindow::~KipToolMainWindow()
{
    delete ui;
}

void KipToolMainWindow::LoadDefaults()
{
    std::string defaultsname;
    QDir dir;
    QString currentname=QDir::homePath()+"/.imagingtools/CurrentKIPToolConfig.xml";

    bool bUseDefaults=true;
    if (dir.exists(currentname)) {
        defaultsname=currentname.toStdString();
        bUseDefaults=false;
    }
//    else {
//      //  m_QtApp->
//    #ifdef Q_OS_WIN32
//         defaultsname="resources/defaults_windows.xml";
//    #elseif Q_OS_LINUX
//        defaultsname=m_sApplicationPath+"resources/defaults_linux.xml";
//    #elseif Q_OS_DARWIN
//        defaultsname=m_sApplicationPath+"../Resources/defaults_mac.xml";
//    #endif
//        bUseDefaults=true;
//    }
    std::ostringstream msg;

    kipl::strings::filenames::CheckPathSlashes(defaultsname,false);
    try {
        m_config.LoadConfigFile(defaultsname.c_str(),"kiplprocessing");

    }
    catch (ModuleException &e) {
        msg<<"Loading defaults failed :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Loading defaults failed :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
    }

    if (bUseDefaults) {
        m_config.mImageInformation.sSourcePath         = QDir::homePath().toStdString();
        m_config.mOutImageInformation.sDestinationPath = QDir::homePath().toStdString();
    }

}


void KipToolMainWindow::UpdateDialog()
{
    ui->edit_datapath->setText(QString::fromStdString(m_config.mImageInformation.sSourcePath));
    ui->edit_datafilemask->setText(QString::fromStdString(m_config.mImageInformation.sSourceFileMask));

    ui->edit_destinationpath->setText(QString::fromStdString(m_config.mOutImageInformation.sDestinationPath));
    ui->edit_destinationmask->setText(QString::fromStdString(m_config.mOutImageInformation.sDestinationFileMask));

    ui->spin_idxfirst->setValue(m_config.mImageInformation.nFirstFileIndex);
    ui->spin_idxlast->setValue(m_config.mImageInformation.nLastFileIndex);
    ui->spin_idxstep->setValue(m_config.mImageInformation.nStepFileIndex);

    ui->check_crop->setChecked(m_config.mImageInformation.bUseROI);
    ui->spin_crop0->setValue(m_config.mImageInformation.nROI[0]);
    ui->spin_crop1->setValue(m_config.mImageInformation.nROI[1]);
    ui->spin_crop2->setValue(m_config.mImageInformation.nROI[2]);
    ui->spin_crop3->setValue(m_config.mImageInformation.nROI[3]);

    ui->widget_moduleconfigurator->SetModules(m_config.modules);
}

void KipToolMainWindow::UpdateConfig()
{
    m_config.mImageInformation.sSourcePath     = ui->edit_datapath->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_config.mImageInformation.sSourcePath,true);
    m_config.mImageInformation.sSourceFileMask = ui->edit_datafilemask->text().toStdString();

    m_config.mOutImageInformation.sDestinationPath     = ui->edit_destinationpath->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_config.mOutImageInformation.sDestinationPath,true);
    m_config.mOutImageInformation.sDestinationFileMask = ui->edit_destinationmask->text().toStdString();

    m_config.mImageInformation.nFirstFileIndex = ui->spin_idxfirst->value();
    m_config.mImageInformation.nLastFileIndex  = ui->spin_idxlast->value();
    m_config.mImageInformation.nStepFileIndex  = ui->spin_idxstep->value();

    m_config.mImageInformation.bUseROI = ui->check_crop->checkState();
    m_config.mImageInformation.nROI[0] = ui->spin_crop0->value();
    m_config.mImageInformation.nROI[1] = ui->spin_crop1->value();
    m_config.mImageInformation.nROI[2] = ui->spin_crop2->value();
    m_config.mImageInformation.nROI[3] = ui->spin_crop3->value();

    m_config.modules = ui->widget_moduleconfigurator->GetModules();
}

void KipToolMainWindow::SetupCallbacks()
{
}

void KipToolMainWindow::on_button_browsedatapath_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location of the image data",
                                      ui->edit_datapath->text());

    if (!projdir.isEmpty())
        ui->edit_datapath->setText(projdir);
}

void KipToolMainWindow::on_button_getROI_clicked()
{
    QRect rect=ui->imageviewer_original->get_marked_roi();

    if (rect.width()*rect.height()!=0)
    {
        ui->spin_crop0->blockSignals(true);
        ui->spin_crop1->blockSignals(true);
        ui->spin_crop2->blockSignals(true);
        ui->spin_crop3->blockSignals(true);
        ui->spin_crop0->setValue(rect.x());
        ui->spin_crop1->setValue(rect.y());
        ui->spin_crop2->setValue(rect.x()+rect.width());
        ui->spin_crop3->setValue(rect.y()+rect.height());
        ui->spin_crop0->blockSignals(false);
        ui->spin_crop1->blockSignals(false);
        ui->spin_crop2->blockSignals(false);
        ui->spin_crop3->blockSignals(false);

        UpdateMatrixROI();
    }
}

void KipToolMainWindow::UpdateMatrixROI()
{
    logger(kipl::logging::Logger::LogMessage,"Update MatrixROI");
    QRect rect;

    rect.setCoords(ui->spin_crop0->value(),
                   ui->spin_crop1->value(),
                   ui->spin_crop2->value(),
                   ui->spin_crop3->value());

    ui->imageviewer_original->set_rectangle(rect,QColor("green"),0);
}

void KipToolMainWindow::on_button_loaddata_clicked()
{
    logger(kipl::logging::Logger::LogMessage,"Load image data");
    UpdateConfig();
    try {
        m_OriginalImage = LoadVolumeImage(m_config);
        m_ProcessedImage = m_OriginalImage;
        m_ProcessedImage.Clone();
        ui->slider_images->setMinimum(0);
        ui->slider_images->setMaximum(m_OriginalImage.Size(2)-1);
        ui->slider_images->setValue(0);
        on_slider_images_sliderMoved(0);
        // Todo: Show histogram...
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox dlg;

        dlg.setText("Failed to load image");
        dlg.setDetailedText(QString::fromStdString(e.what()));
        dlg.exec();
    }
}

void KipToolMainWindow::on_button_browsedestination_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location to save the processed image data",
                                      ui->edit_destinationpath->text());

    if (!projdir.isEmpty())
        ui->edit_destinationpath->setText(projdir);
}

void KipToolMainWindow::on_check_crop_stateChanged(int arg1)
{
    logger(kipl::logging::Logger::LogMessage,"crop state changed");

}

void KipToolMainWindow::on_button_savedata_clicked()
{
    logger(kipl::logging::Logger::LogMessage,"Save processed data");

    // Todo: Implement the processed data
}

void KipToolMainWindow::on_check_showorighist_stateChanged(int arg1)
{
logger(kipl::logging::Logger::LogMessage,"Show orig hist");
}

void KipToolMainWindow::on_combo_plotselector_currentIndexChanged(int index)
{
logger(kipl::logging::Logger::LogMessage,"plot selector index changed");
}

void KipToolMainWindow::on_slider_images_sliderMoved(int position)
{
    if (1<m_OriginalImage.Size()) {
        ui->imageviewer_original->set_image(m_OriginalImage.GetLinePtr(0,position),m_OriginalImage.Dims());
    }
    if (1<m_ProcessedImage.Size()) {
        ui->imageviewer_processed->set_image(m_ProcessedImage.GetLinePtr(0,position),m_ProcessedImage.Dims());
        if ((m_OriginalImage.Size(0)==m_ProcessedImage.Size(0)) && (m_OriginalImage.Size(0)==m_ProcessedImage.Size(0))) {
            kipl::base::TImage<float,2> diff(m_OriginalImage.Dims());

            for (int i=0; i<diff.Size(); i++) {
                diff[i]=m_ProcessedImage.GetLinePtr(0,position)[i]-m_OriginalImage.GetLinePtr(0,position)[i];

            }
            ui->imageviewer_difference->set_image(diff.GetDataPtr(),diff.Dims());
        }
    }
}

void KipToolMainWindow::on_actionNew_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"New config");
    KiplProcessConfig cfg;
    m_config = cfg;
    ui->imageviewer_original->clear_viewer();
    ui->imageviewer_processed->clear_viewer();
    ui->plotter_histogram->clearAllCurves();
    ui->plotter_plots->clearAllCurves();
    ui->imageviewer_difference->clear_viewer();
}

void KipToolMainWindow::on_actionOpen_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"open config");
}


void KipToolMainWindow::on_actionSave_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"save config");

    if (m_sFileName == "noname.xml") {
        on_actionSave_as_triggered();
    }
    else {
        SaveConfiguration(m_sFileName);
    }
}

void KipToolMainWindow::on_actionSave_as_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"save config as");

    QString qfname=QDir::homePath()+"/"+m_sFileName;

    qfname=QFileDialog::getSaveFileName(this,"Select location to save the configuration",qfname,"*.xml");

    if (!qfname.isEmpty()) {
        m_sFileName = qfname;
        SaveConfiguration(qfname);

    }

}

void KipToolMainWindow::SaveConfiguration(QString qfname)
{
    std::string fname = qfname.toStdString();
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    std::ofstream cfgfile(fname.c_str());

    cfgfile<<m_config.WriteXML();
}

void KipToolMainWindow::on_actionQuit_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"Quit");
}

void KipToolMainWindow::on_actionStart_processing_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"Start processing");

    UpdateConfig();
    QString qfname=QDir::homePath()+"/.imagingtools/CurrentKIPToolConfig.xml";
    SaveConfiguration(qfname);
//  todo: Start the processing

    kipl::base::TImage<float,2> img(m_ProcessedImage.Dims());
    m_config.UserInformation.sDate = kipl::utilities::TimeStamp();
    memcpy(img.GetDataPtr(),m_ProcessedImage.GetLinePtr(0,m_ProcessedImage.Size(2)/2),img.Size()*sizeof(float));
    m_configHistory.push_back(make_pair(m_config,img));

}

void KipToolMainWindow::on_actionProcessing_history_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"Processing history");

    if (!m_configHistory.empty()) {
        ConfigHistoryDialog dlg;

        dlg.setList(m_configHistory);

        int res = dlg.exec();

        if (res == QDialog::Accepted) {
            logger(kipl::logging::Logger::LogMessage,"Taking an old config");

            m_config = dlg.getSelectedConfig();
            UpdateDialog();
        }
    }
    else
        logger(kipl::logging::Logger::LogMessage,"The history list is empty.");
}

void KipToolMainWindow::on_actionClear_History_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"Clear history");
    m_configHistory.clear();
}


void KipToolMainWindow::on_actionAbout_triggered()
{
logger(kipl::logging::Logger::LogMessage,"About");
}

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include <KiplFrameworkException.h>
#include <ModuleException.h>
#include <utilities/TimeDate.h>
#include <math/image_statistics.h>
#include <base/thistogram.h>

#include "kiptoolmainwindow.h"
#include "ui_kiptoolmainwindow.h"
#include "confighistorydialog.h"

#include "ImageIO.h"

KipToolMainWindow::KipToolMainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("KipToolMainWindow"),
    ui(new Ui::KipToolMainWindow),
    m_Engine(NULL),
    m_OriginalHistogram(1024),
    m_sFileName("noname.xml"),
    m_bRescaleViewers(false),
    m_bJustLoaded(false)
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
    switch (ui->combo_FileType->currentIndex()) {
        case 0: m_config.mOutImageInformation.eResultImageType = kipl::io::TIFF16bits; break;
        case 1: m_config.mOutImageInformation.eResultImageType = kipl::io::TIFF8bits; break;
        case 2: m_config.mOutImageInformation.eResultImageType = kipl::io::TIFF16bits; break;
        case 3: m_config.mOutImageInformation.eResultImageType = kipl::io::TIFFfloat; break;
    }
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

void KipToolMainWindow::UpdateHistogramView()
{
    std::map<std::string, kipl::containers::PlotData<float,size_t> >::iterator it;
    int idx=1;

    if (!m_HistogramList.empty()) {

        for (it=m_HistogramList.begin(); it!=m_HistogramList.end(); it++, idx++) {
            ui->plotter_histogram->setCurveData(idx,it->second.GetX(),it->second.GetY(),it->second.Size());
        }

        ui->plotter_histogram->setCurveData(0,m_OriginalHistogram.GetX(), m_OriginalHistogram.GetY(),m_OriginalHistogram.Size());
    }
    else {
        ui->plotter_histogram->clearAllCurves();
    }
    //plotviewer_histogram.show_all();
}

void KipToolMainWindow::UpdatePlotView()
{
    std::map<std::string, kipl::containers::PlotData<float,float> >::iterator plot_it;
    std::map<std::string, std::map<std::string, kipl::containers::PlotData<float,float> > >::iterator module_it;
    int idx=0;

    if (!m_PlotList.empty()) {
        for (module_it=m_PlotList.begin(); module_it!=m_PlotList.end(); module_it++) {
            ui->plotter_plots->clearAllCurves();
            idx=0;
            for (plot_it=module_it->second.begin(); plot_it!=module_it->second.end(); plot_it++,idx++) {
                ui->plotter_plots->setCurveData(idx,plot_it->second.GetX(),plot_it->second.GetY(),plot_it->second.Size());
            }
        }
    }

        //plotviewer_plots.show_all();
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
        if (m_Engine!=NULL) {
            delete m_Engine;
            m_Engine = NULL;
        }

        ui->slider_images->setMinimum(0);
        ui->slider_images->setMaximum(m_OriginalImage.Size(2)-1);
        ui->slider_images->setValue(0);
        m_bJustLoaded = true;
        on_slider_images_sliderMoved(0);
        // Todo: Show histogram...
        float *axis = new float[m_OriginalHistogram.Size()];
        size_t *bins = new size_t[m_OriginalHistogram.Size()];
        kipl::base::Histogram(m_OriginalImage.GetDataPtr(),m_OriginalImage.Size(),bins,m_OriginalHistogram.Size(),0,0,axis);

        m_OriginalHistogram.SetData(axis,bins,m_OriginalHistogram.Size());
        delete [] axis;
        delete [] bins;
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
    if (m_Engine!=NULL) {
        UpdateConfig();
        m_Engine->SaveImage(&m_config.mOutImageInformation);
    }
    else {
        QMessageBox dlg;
        dlg.setText("There is not processed data to save");
        dlg.exec();
    }
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
    if ((m_OriginalImage.Size()!=0) && (position<m_OriginalImage.Size(2)) && (0<=position)) {
        float lo = 0.0f;
        float hi = 0.0f;
        if (m_bJustLoaded) {
            m_bJustLoaded = false;
            kipl::math::minmax(m_OriginalImage.GetDataPtr(),m_OriginalImage.Size(),&lo,&hi);
        }
        else
            ui->imageviewer_original->get_levels(&lo,&hi);

        ui->imageviewer_original->set_image(m_OriginalImage.GetLinePtr(0,position),m_OriginalImage.Dims(),lo,hi);
        if (m_Engine!=NULL) {
            kipl::base::TImage<float,3> &result=m_Engine->GetResultImage();

            if ((result.Size(0)==m_OriginalImage.Size(0)) &&
                (result.Size(1)==m_OriginalImage.Size(1)) &&
                (result.Size(2)==m_OriginalImage.Size(2)))
            {
                if (m_bRescaleViewers) {
                    ui->imageviewer_processed->set_image(result.GetLinePtr(0,position),result.Dims());
                    m_bRescaleViewers=false;
                }
                else {
                    ui->imageviewer_processed->get_levels(&lo,&hi);
                    ui->imageviewer_processed->set_image(result.GetLinePtr(0,position),result.Dims(),lo,hi);
                }

                kipl::base::TImage<float,2> diff(result.Dims());
                float *pDiff=diff.GetDataPtr();
                float *pRes=result.GetLinePtr(0,position);
                float *pImg=m_OriginalImage.GetLinePtr(0,position);

                for (int i=0; i<diff.Size(); i++) {
                        pDiff[i]=pRes[i]-pImg[i];
                }
                ui->imageviewer_difference->set_image(pDiff,diff.Dims());
            }
        }
        else {
            ui->imageviewer_processed->clear_viewer();
            ui->imageviewer_difference->clear_viewer();
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
//  Start the processing
    std::ostringstream msg;


    if (m_OriginalImage.Size()==0) {
        QMessageBox dlg;
        dlg.setText("Please load an image before you start processing!");
        dlg.exec();
        return;
    }

    if (m_config.modules.empty()) {
        QMessageBox dlg;
        dlg.setText("There are no modules in the process chain.");
        dlg.exec();
        return;
    }

    if (m_Engine) {
        delete m_Engine;
        m_Engine=NULL;
    }

    bool bBuildFailed=false;

    try {
        m_Engine=m_Factory.BuildEngine(m_config);
    }
    catch (ModuleException &e) {
        bBuildFailed=true;
        msg.str("");
        msg<<"ModuleException: "<<e.what();
    }
    catch (KiplFrameworkException &e) {
        bBuildFailed=true;
        msg.str("");
        msg<<"KiplFrameworkException: "<<e.what();
    }
    catch (kipl::base::KiplException &e) {
        bBuildFailed=true;
        msg.str("");
        msg<<"KiplException: "<<e.what();
    }
    catch (std::exception &e) {
        bBuildFailed=true;
        msg.str("");
        msg<<"STL Exception: "<<e.what();
    }
    catch (...) {
        bBuildFailed=true;
        msg.str("");
        msg<<"Unhandled exception";
    }

    if (bBuildFailed) {
        logger(kipl::logging::Logger::LogError,msg.str());
        QMessageBox dlg;

        dlg.setText("Failed to build the process chain.");
        dlg.exec();
        return;
    }

    logger(kipl::logging::Logger::LogMessage,"The process chain was built successfully.");

    bool bExecutionFailed=false;
    m_PlotList.clear();
    m_HistogramList.clear();

    msg.str("");
    try {
        m_Engine->Run(&m_OriginalImage);

        m_PlotList=m_Engine->GetPlots();
        m_HistogramList=m_Engine->GetHistograms();
    }
    catch (ModuleException &e) {
        bBuildFailed=true;
        msg.str("");
        msg<<"ModuleException: "<<e.what();
    }
    catch (KiplFrameworkException &e) {
        bExecutionFailed=true;
        msg<<"KiplFrameworkException: "<<e.what();
    }
    catch (kipl::base::KiplException &e) {
        bExecutionFailed=true;
        msg<<"KiplException: "<<e.what();
    }
    catch (std::exception &e) {
        bExecutionFailed=true;
        msg<<"STL Exception: "<<e.what();
    }
    catch (...) {
        bExecutionFailed=true;
        msg<<"Unhandled exception";
    }

    if (bExecutionFailed) {
        logger(kipl::logging::Logger::LogError,msg.str());
        QMessageBox dlg;
        dlg.setText("The process chain exectution failed");
        dlg.exec();
        return;
    }

    logger(kipl::logging::Logger::LogMessage,"The process chain ended successfully");

    m_bRescaleViewers=true;
    on_slider_images_sliderMoved(ui->slider_images->value());
    UpdatePlotView();
    UpdateHistogramView();

//  post processing admin
    kipl::base::TImage<float,3> &result=m_Engine->GetResultImage();
    kipl::base::TImage<float,2> img(result.Dims());

    m_config.UserInformation.sDate = kipl::utilities::TimeStamp();
    memcpy(img.GetDataPtr(),result.GetLinePtr(0,result.Size(2)/2),img.Size()*sizeof(float));
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

//<LICENSE>

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QDesktopServices>
#include <QVersionNumber>

#include <KiplFrameworkException.h>
#include <ModuleException.h>
#include <utilities/TimeDate.h>
#include <math/image_statistics.h>
#include <base/thistogram.h>
#include <base/textractor.h>
#include <io/DirAnalyzer.h>
#include <datasetbase.h>
#include <strings/filenames.h>
#include <stltools/stlvecmath.h>

#include "kiptoolmainwindow.h"
#include "ui_kiptoolmainwindow.h"
#include "confighistorydialog.h"
#include "genericconversion.h"
#include "reslicerdialog.h"
#include "mergevolumesdialog.h"
#include "fileconversiondialog.h"
#include "processdialog.h"
#include "loadimagedialog.h"

#include "ImageIO.h"
using namespace std;

KipToolMainWindow::KipToolMainWindow(QApplication *app, QWidget *parent) :
    QMainWindow(parent),
    logger("KipToolMainWindow"),
    ui(new Ui::KipToolMainWindow),

    logdlg(new QtAddons::LoggingDialog(this)),
    button_toggleLoggerDlg(new QPushButton("Logger",this)),
    m_Engine(nullptr),
    m_OriginalHistogram(1024UL,"Original histogram"),
    m_config(QCoreApplication::applicationDirPath().toStdString()),
    m_ModuleConfigurator(&m_config),
    m_sFileName("noname.xml"),
    m_bRescaleViewers(false),
    m_bJustLoaded(false),
    m_eSlicePlane(kipl::base::ImagePlaneXY),
    m_QtApp(app),
    m_sApplicationPath(app->applicationDirPath().toStdString())
{
    ui->setupUi(this);
    //logger.AddLogTarget(*(ui->widget_logviewer));
    logger.AddLogTarget(*logdlg);

    ui->widget_moduleconfigurator->configure("kiptool",QDir::currentPath().toStdString(),&m_ModuleConfigurator);
    ui->widget_moduleconfigurator->SetApplicationObject(this);
    //    ui->widget_moduleconfigurator->configure("kiptool",QDir::currentPath().toStdString());
    ui->statusBar->addPermanentWidget(button_toggleLoggerDlg);

    QString configpath=QDir::homePath()+"/.imagingtools";
    QDir dir;
    kipl::strings::filenames::CheckPathSlashes(m_sApplicationPath,true);

    if (!dir.exists(configpath)) {
         dir.mkdir(configpath);
    }

    LoadDefaults();
    UpdateDialog();
    SetupCallbacks();

    ui->plotter_hprofile->hideLegend();
    ui->plotter_vprofile->hideLegend();

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
        m_config.setAppPath(m_sApplicationPath);
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
    m_config.mImageInformation.sSourcePath.clear();
    FileSet loadInfo;

    loadInfo.m_sFilemask = m_config.mImageInformation.sSourceFileMask;
    loadInfo.m_nFirst    = m_config.mImageInformation.nFirstFileIndex;
    loadInfo.m_nLast     = m_config.mImageInformation.nLastFileIndex;
    loadInfo.m_nStep     = m_config.mImageInformation.nStepFileIndex;
    loadInfo.m_bUseROI   = m_config.mImageInformation.bUseROI;
    loadInfo.m_Flip      = m_config.mImageInformation.eFlip;
    loadInfo.m_Rotate    = m_config.mImageInformation.eRotate;
    loadInfo.m_nRepeat   = m_config.mImageInformation.nRepeat;
    loadInfo.m_nStride   = m_config.mImageInformation.nStride;

    std::copy_n(m_config.mImageInformation.nROI, 4, loadInfo.m_ROI);
    ui->widget_loadForm->setReaderConfig(loadInfo);

    ui->edit_destinationpath->setText(QString::fromStdString(m_config.mOutImageInformation.sDestinationPath));
    ui->edit_destinationmask->setText(QString::fromStdString(m_config.mOutImageInformation.sDestinationFileMask));

    int idx=0;
    switch (m_config.mOutImageInformation.eResultImageType) {
        case kipl::io::TIFF8bits  : idx=1; break;
        case kipl::io::TIFF16bits : idx=2; break;
        case kipl::io::TIFFfloat  : idx=3; break;
        default: idx=0;
    }

    ui->combo_FileType->setCurrentIndex(idx);
    ui->widget_moduleconfigurator->SetModules(m_config.modules);
    ui->text_description->setText(QString(m_config.UserInformation.sComment.c_str()));

    ui->edit_samplename->setText(m_config.UserInformation.sSample.c_str());
    ui->edit_username->setText(m_config.UserInformation.sOperator.c_str());
    ui->edit_projectname->setText(m_config.UserInformation.sProjectNumber.c_str());
    ui->edit_instrument->setText(m_config.UserInformation.sInstrument.c_str());
}

void KipToolMainWindow::UpdateConfig()
{


    m_config.mOutImageInformation.sDestinationPath     = ui->edit_destinationpath->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_config.mOutImageInformation.sDestinationPath,true);
    m_config.mOutImageInformation.sDestinationFileMask = ui->edit_destinationmask->text().toStdString();

    FileSet readerInfo=ui->widget_loadForm->getReaderConfig();
    m_config.mImageInformation.sSourcePath.clear();
    kipl::strings::filenames::CheckPathSlashes(m_config.mImageInformation.sSourcePath,true);

    m_config.mImageInformation.sSourceFileMask = readerInfo.m_sFilemask;
    m_config.mImageInformation.nFirstFileIndex = static_cast<size_t>(readerInfo.m_nFirst);
    m_config.mImageInformation.nLastFileIndex  = static_cast<size_t>(readerInfo.m_nLast);
    m_config.mImageInformation.nStepFileIndex  = static_cast<size_t>(readerInfo.m_nStep);
    m_config.mImageInformation.nStride         = static_cast<size_t>(readerInfo.m_nStride);
    m_config.mImageInformation.nRepeat         = static_cast<size_t>(readerInfo.m_nRepeat);
    m_config.mImageInformation.eFlip           = readerInfo.m_Flip;
    m_config.mImageInformation.eRotate         = readerInfo.m_Rotate;

    m_config.mImageInformation.bUseROI = readerInfo.m_bUseROI;
    std::copy_n(readerInfo.m_ROI,4,m_config.mImageInformation.nROI);

    m_config.modules = ui->widget_moduleconfigurator->GetModules();
    switch (ui->combo_FileType->currentIndex())
    {
        case 0:
        {
            switch (m_OriginalImage.info.nBitsPerSample)
            {
                case 8  : m_config.mOutImageInformation.eResultImageType = kipl::io::TIFF8bits;  break;
                case 16 : m_config.mOutImageInformation.eResultImageType = kipl::io::TIFF16bits; break;
                case 32 : m_config.mOutImageInformation.eResultImageType = kipl::io::TIFFfloat;  break;
                default : m_config.mOutImageInformation.eResultImageType = kipl::io::TIFF16bits; break;
            }
            break;
        }
        case 1: m_config.mOutImageInformation.eResultImageType = kipl::io::TIFF8bits; break;
        case 2: m_config.mOutImageInformation.eResultImageType = kipl::io::TIFF16bits; break;
        case 3: m_config.mOutImageInformation.eResultImageType = kipl::io::TIFFfloat; break;
        case 4: m_config.mOutImageInformation.eResultImageType = kipl::io::TIFF16bitsMultiFrame; break;
    }

    m_config.UserInformation.sComment       = ui->text_description->toPlainText().toStdString();
    m_config.UserInformation.sInstrument    = ui->edit_instrument->text().toStdString();
    m_config.UserInformation.sOperator      = ui->edit_username->text().toStdString();
    m_config.UserInformation.sVersion       = kipl::strings::value2string(kipl::strings::VersionNumber("$Rev$"));
    m_config.UserInformation.sSample        = ui->edit_samplename->text().toStdString();
}

void KipToolMainWindow::SetupCallbacks()
{
    ui->widget_loadForm->setHideROI(false);
    ui->widget_loadForm->setHideMirrorRotate(false);

    QtAddons::uxROIWidget * roiw=ui->widget_loadForm->roiWidget();

    roiw->registerViewer(ui->imageviewer_original);
    roiw->setROIColor("green");
    roiw->setTitle("Crop region");
    roiw->updateViewer();
    roiw->setAutoHideROI(true);
    roiw->setAllowUpdateImageDims(false);

    // BUtton in status bar needs to be manually connected
    connect(button_toggleLoggerDlg,SIGNAL(clicked()),this,SLOT(button_toggleLoggerDlg_clicked()));

}

void KipToolMainWindow::UpdateHistogramView()
{
    std::map<std::string, kipl::containers::PlotData<float,size_t> >::iterator it;
    int idx=1;

    if (!m_HistogramList.empty()) {

        for (it=m_HistogramList.begin(); it!=m_HistogramList.end(); it++, idx++) {
            QString lbl;
            lbl.sprintf("Hist ",idx);
            ui->plotter_histogram->setCurveData(idx,it->second.GetX(),it->second.GetY(),static_cast<int>(it->second.Size()),lbl);
        }


    }
    else {
        ui->plotter_histogram->clearAllCurves();
    }
    ui->plotter_histogram->setCurveData(0,m_OriginalHistogram.GetX(), m_OriginalHistogram.GetY(),m_OriginalHistogram.Size(),QString::fromStdString(m_OriginalHistogram.name()));
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
                ui->plotter_plots->setCurveData(idx,plot_it->second.GetX(),plot_it->second.GetY(),plot_it->second.Size(),QString::fromStdString(plot_it->second.name()));
            }
        }
    }
}

void KipToolMainWindow::UpdateMatrixROI()
{
    logger(kipl::logging::Logger::LogMessage,"Update MatrixROI");
//    QRect rect;

//    rect.setCoords(ui->spin_crop0->value(),
//                   ui->spin_crop1->value(),
//                   ui->spin_crop2->value(),
//                   ui->spin_crop3->value());

//    ui->imageviewer_original->set_rectangle(rect,QColor("green"),0);
}

void KipToolMainWindow::on_button_loaddata_clicked()
{
    logger(kipl::logging::Logger::LogMessage,"Load image data");
    UpdateConfig();
    try {
        LoadImageDialog loaddlg(&m_Interactor,this);
        kipl::base::TImage<float,3> img;
        int res = loaddlg.exec(&m_config,&img);

        if (res == QDialog::Accepted)
        {
            m_OriginalImage=img;
            if (m_Engine!=nullptr) {
                delete m_Engine;
                m_Engine = nullptr;
            }

            ui->slider_images->setMinimum(0);
            ui->slider_images->setMaximum(static_cast<int>(m_OriginalImage.Size(2))-1);
            ui->slider_images->setValue(0);
            m_bJustLoaded = true;
            ui->combo_sliceplane->setCurrentIndex(0);
            on_combo_sliceplane_activated(0);
            on_slider_images_sliderMoved(0);
            // Todo: Show histogram...
            float *axis = new float[m_OriginalHistogram.Size()];
            size_t *bins = new size_t[m_OriginalHistogram.Size()];
            kipl::base::Histogram(m_OriginalImage.GetDataPtr(),m_OriginalImage.Size(),bins,m_OriginalHistogram.Size(),0,0,axis);

            medianFilter(bins,m_OriginalHistogram.Size(),5);
            m_OriginalHistogram.SetData(axis,bins,m_OriginalHistogram.Size());
            UpdateHistogramView();
            delete [] axis;
            delete [] bins;
        }
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

void KipToolMainWindow::on_button_savedata_clicked()
{
    logger(kipl::logging::Logger::LogMessage,"Save processed data");
    if (m_Engine!=nullptr) {
        UpdateConfig();
        m_Engine->SaveImage(&m_config.mOutImageInformation);
    }
    else {
        QMessageBox dlg;
        dlg.setText("There is not processed data to save");
        dlg.exec();
    }
}

void KipToolMainWindow::on_combo_plotselector_currentIndexChanged(int index)
{
    logger(kipl::logging::Logger::LogMessage,"plot selector index changed");
}

void KipToolMainWindow::on_slider_images_sliderMoved(int position)
{
    int maxslice;
    switch (m_eSlicePlane) {
    case kipl::base::ImagePlaneXY: maxslice = static_cast<int>(m_OriginalImage.Size(2)); break;
    case kipl::base::ImagePlaneXZ: maxslice = static_cast<int>(m_OriginalImage.Size(1)); break;
    case kipl::base::ImagePlaneYZ: maxslice = static_cast<int>(m_OriginalImage.Size(0)); break;
    }

    if ((m_OriginalImage.Size()!=0) && (position<maxslice) && (0<=position)) {
        QSignalBlocker blocker(ui->spinSliceIndex);

        ui->spinSliceIndex->setValue(position);
        float lo = 0.0f;
        float hi = 0.0f;
        if (m_bJustLoaded) {
            m_bJustLoaded = false;
            kipl::math::minmax(m_OriginalImage.GetDataPtr(),m_OriginalImage.Size(),&lo,&hi);
        }
        else
            ui->imageviewer_original->get_levels(&lo,&hi);



        m_SliceOriginal=kipl::base::ExtractSlice(m_OriginalImage,static_cast<size_t>(position),m_eSlicePlane);

        ui->imageviewer_original->set_image(m_SliceOriginal.GetDataPtr(),m_SliceOriginal.Dims(),lo,hi);
        if (m_Engine!=nullptr) {
            kipl::base::TImage<float,3> &result=m_Engine->GetResultImage();

            if ((result.Size(0)==m_OriginalImage.Size(0)) &&
                (result.Size(1)==m_OriginalImage.Size(1)) &&
                (result.Size(2)==m_OriginalImage.Size(2)))
            {
                m_SliceResult = kipl::base::ExtractSlice(result,position,m_eSlicePlane);
                if (m_bRescaleViewers) {
                    ui->imageviewer_processed->set_image(m_SliceResult.GetDataPtr(), m_SliceResult.Dims());
                    m_bRescaleViewers=false;
                }
                else {
                    ui->imageviewer_processed->get_levels(&lo,&hi);
                    ui->imageviewer_processed->set_image(m_SliceResult.GetDataPtr(), m_SliceResult.Dims(),lo,hi);
                }

                kipl::base::TImage<float,2> diff(m_SliceOriginal.Dims());
                float *pDiff=diff.GetDataPtr();
                float *pRes=m_SliceResult.GetDataPtr();
                float *pImg=m_SliceOriginal.GetDataPtr();

                for (size_t i=0; i<diff.Size(); i++) {
                        pDiff[i]=pRes[i]-pImg[i];
                }
                ui->imageviewer_difference->set_image(pDiff,diff.Dims());
            }
        }
        else {
            ui->imageviewer_processed->clear_viewer();
            ui->imageviewer_difference->clear_viewer();
        }
        on_slider_hprofile_sliderMoved(ui->slider_hprofile->value());
        on_slider_vprofile_sliderMoved(ui->slider_vprofile->value());
    }
}

void KipToolMainWindow::on_actionNew_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"New config requested");
    KiplProcessConfig cfg(QCoreApplication::applicationDirPath().toStdString());
    m_config = cfg;
    ui->imageviewer_original->clear_viewer();
    ui->imageviewer_processed->clear_viewer();
    ui->plotter_histogram->clearAllCurves();
    ui->plotter_plots->clearAllCurves();
    ui->imageviewer_difference->clear_viewer();
    UpdateDialog();
}

void KipToolMainWindow::on_actionOpen_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"open config");

    QString qfname;

    qfname=QFileDialog::getOpenFileName(this,"Select configuration file to open",qfname,"*.xml");

    if (!qfname.isEmpty()) {
        m_sFileName = qfname;
        LoadConfiguration(qfname);
    }
    else {
        logger(kipl::logging::Logger::LogMessage,"The configuration name was empty, no file saved.");

    }

}


void KipToolMainWindow::on_actionSave_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"save config");



    if (m_sFileName == "noname.xml") {
        on_actionSave_as_triggered();
    }
    else {
        UpdateConfig();
        SaveConfiguration(m_sFileName);
    }
}

void KipToolMainWindow::on_actionSave_as_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"Save config as");

    UpdateConfig();
    QString qfname=QDir::homePath()+"/"+m_sFileName;

    qfname=QFileDialog::getSaveFileName(this,"Select location to save the configuration",qfname,"*.xml");

    if (!qfname.isEmpty()) {
        m_sFileName = qfname;
        SaveConfiguration(qfname);
    }
    else {
        logger(kipl::logging::Logger::LogMessage,"The configuration name was empty, no file saved.");
        QMessageBox::warning(this,"No file name","You did not provide a file name to save the configuration file. The configuration was not saved.");
    }

}

void KipToolMainWindow::SaveConfiguration(QString qfname)
{
    std::string fname = qfname.toStdString();
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    std::ofstream cfgfile(fname.c_str());

    logger.message(std::string("Save config apppath ")+m_sApplicationPath);
    m_config.setAppPath(m_sApplicationPath);
    cfgfile<<m_config.WriteXML();
}

void  KipToolMainWindow::LoadConfiguration(QString qfname)
{
    std::string fname = qfname.toStdString();
    kipl::strings::filenames::CheckPathSlashes(fname,false);

    QString sError;
    bool bError=false;
    try {
        m_config.setAppPath(m_sApplicationPath);
        m_config.LoadConfigFile(fname,"kiplprocessing");
    }
    catch (ModuleException &e) {
        sError=QString::fromStdString(e.what());
        bError=true;
    }
    catch (kipl::base::KiplException &e) {
        sError=QString::fromStdString(e.what());
        bError=true;
    }
    catch (std::exception &e) {
        sError=QString::fromStdString(e.what());
        bError=true;
    }
    catch (...) {
        sError="Unsupported exception was collected while reading configuration file.";
        bError=true;
    }

    if (!bError)
        UpdateDialog();
    else {
        QMessageBox msg(this);

        msg.setText("Failed to open configuration file");
        msg.setDetailedText(sError);
        msg.addButton(QMessageBox::Ok);

        msg.exec();
    }
}

void KipToolMainWindow::on_actionQuit_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"Quit");
}

void KipToolMainWindow::on_actionStart_processing_triggered()
{
    logger(kipl::logging::Logger::LogMessage,"Start processing");

    UpdateConfig();
    m_config.setAppPath(QCoreApplication::applicationDirPath().toStdString());
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
        m_Engine=nullptr;
    }

    bool bBuildFailed=false;

    try {
        m_Engine=m_Factory.BuildEngine(m_config,&m_Interactor);
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
        dlg.setDetailedText(QString::fromStdString(msg.str()));
        dlg.exec();
        return;
    }

    logger(kipl::logging::Logger::LogMessage,"The process chain was built successfully.");

    bool bExecutionFailed=false;
    m_PlotList.clear();
    m_HistogramList.clear();

    ProcessDialog dlg(&m_Interactor,this);

    msg.str("");
    int res=0;
    try {
        res=dlg.exec(m_Engine,&m_OriginalImage);
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
        dlg.setText("The process chain execution failed");
        dlg.exec();
        return;
    }

    if (res==QDialog::Accepted) {
        logger(kipl::logging::Logger::LogMessage,"The process chain ended successfully");
        m_PlotList=m_Engine->GetPlots();
        m_HistogramList=m_Engine->GetHistograms();
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
    else {
        logger(kipl::logging::Logger::LogMessage,"The process chain was aborted.");

    }
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
    QMessageBox dlg;
    std::ostringstream msg;
    QVersionNumber ver;

    msg<<"KipTool "<<m_QtApp->applicationVersion().toStdString()<<"\nCompile date: "<<__DATE__<<" at "<<__TIME__<<std::endl;

    msg<<"Using \nQt version: "<<qVersion()<<"\n"
      <<"LibTIFF, zLib, fftw3, libcfitsio";

    dlg.setText(QString::fromStdString(msg.str()));

    dlg.exec();
}

void KipToolMainWindow::button_toggleLoggerDlg_clicked()
{
    if (logdlg->isHidden()) {

        logdlg->show();
    }
    else {
        logdlg->hide();
    }
}

void KipToolMainWindow::on_combo_sliceplane_activated(int index)
{
    std::ostringstream msg;
    m_eSlicePlane = static_cast<kipl::base::eImagePlanes>(1<<index);
    int maxslices=static_cast<int>(m_OriginalImage.Size(2-index));
    ui->slider_images->setMaximum(maxslices-1);
    ui->slider_images->setValue(maxslices/2);

    msg<<"Changed slice plane to "<<m_eSlicePlane<<" max slices="<<maxslices<<" "<<m_OriginalImage;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    switch (m_eSlicePlane) {
        case kipl::base::ImagePlaneXY :
            m_nSliceSizeX=m_OriginalImage.Size(0)-1;
            m_nSliceSizeY=m_OriginalImage.Size(1)-1;
        break;
        case kipl::base::ImagePlaneXZ :
            m_nSliceSizeX=m_OriginalImage.Size(0)-1;
            m_nSliceSizeY=m_OriginalImage.Size(2)-1;
        break;
        case kipl::base::ImagePlaneYZ :
            m_nSliceSizeX=m_OriginalImage.Size(1)-1;
            m_nSliceSizeY=m_OriginalImage.Size(2)-1;
        break;
    }

    ui->slider_hprofile->setMaximum(m_nSliceSizeY);
    ui->slider_hprofile->setValue(m_nSliceSizeY/2);

    ui->slider_vprofile->setMaximum(m_nSliceSizeX);
    ui->slider_vprofile->setValue(m_nSliceSizeX/2);

    on_slider_images_sliderMoved(maxslices/2);
    on_slider_hprofile_sliderMoved(m_nSliceSizeY/2);
    on_slider_vprofile_sliderMoved(m_nSliceSizeX/2);
}

void KipToolMainWindow::on_check_linkviewers_toggled(bool checked)
{
    QtAddons::ImageViewerWidget *v1=dynamic_cast<QtAddons::ImageViewerWidget *>(ui->imageviewer_original);
    QtAddons::ImageViewerWidget *v2=dynamic_cast<QtAddons::ImageViewerWidget *>(ui->imageviewer_processed);

    if (checked)
        v1->LinkImageViewer(v2);
    else
        v1->ClearLinkedImageViewers();
}

void KipToolMainWindow::on_tabWidget_plots_currentChanged(int index)
{
    if (index==3) {
        on_slider_hprofile_sliderMoved(ui->slider_hprofile->value());
        on_slider_vprofile_sliderMoved(ui->slider_vprofile->value());
    }
    else {
        ui->imageviewer_original->clear_plot(0);
        ui->imageviewer_original->clear_plot(1);
        if (m_Engine!=nullptr) {
            ui->imageviewer_processed->clear_plot(0);
            ui->imageviewer_processed->clear_plot(1);
        }
    }
}

void KipToolMainWindow::on_slider_hprofile_sliderMoved(int position)
{
    if (ui->tabWidget_plots->currentIndex()==3) {
        QVector<QPointF> cursor;

        cursor.append(QPointF(0.0,static_cast<double>(position)));
        cursor.append(QPointF(m_nSliceSizeX,position));
        ui->imageviewer_original->set_plot(cursor,QColor("red"),0);

        QVector<QPointF> data;
        float *pImg=m_SliceOriginal.GetLinePtr(static_cast<size_t>(position));
        for (size_t i=0; i<m_SliceOriginal.Size(0); i++)
            data.append(QPointF(i,static_cast<double>(pImg[i])));

        ui->plotter_hprofile->setCurveData(0,data,"Original");

        if (m_Engine!=nullptr) {
            ui->imageviewer_processed->set_plot(cursor,QColor("red"),0);
            data.clear();
            pImg=m_SliceResult.GetLinePtr(static_cast<size_t>(position));
            for (size_t i=0; i<m_SliceResult.Size(0); i++)
                data.append(QPointF(i,static_cast<double>(pImg[i])));

            ui->plotter_hprofile->setCurveData(1,data,"Processed");
        }
    }
}

void KipToolMainWindow::on_slider_vprofile_sliderMoved(int position)
{
    if (ui->tabWidget_plots->currentIndex()==3) {
        QVector<QPointF> cursor;
        cursor.append(QPointF(position,0));
        cursor.append(QPointF(position,m_nSliceSizeY));
        ui->imageviewer_original->set_plot(cursor,QColor("red"),1);

        QVector<QPointF> data;
        float *pImg=m_SliceOriginal.GetDataPtr()+position;
        size_t sx=m_SliceOriginal.Size(0);
        for (size_t i=0; i<m_SliceOriginal.Size(1); i++)
            data.append(QPointF(i,static_cast<double>(pImg[i*sx])));

        ui->plotter_vprofile->setCurveData(0,data,"Original");

        if (m_Engine!=nullptr) {
            ui->imageviewer_processed->set_plot(cursor,QColor("red"),1);
            data.clear();
            pImg=m_SliceResult.GetDataPtr()+position;
            for (size_t i=0; i<m_SliceResult.Size(1); i++)
                data.append(QPointF(i,static_cast<double>(pImg[i*sx])));

            ui->plotter_vprofile->setCurveData(1,data,"Processed");
        }
    }
}

void KipToolMainWindow::on_actionGeneric_file_conversion_triggered()
{
    GenericConversion dlg;

    dlg.exec();

}

void KipToolMainWindow::on_actionReslice_images_triggered()
{
    ReslicerDialog dlg;

    dlg.exec();
}

void KipToolMainWindow::on_actionMerge_volumes_triggered()
{
    MergeVolumesDialog dlg;

    dlg.exec();
}

void KipToolMainWindow::on_actionFile_conversion_triggered()
{
    FileConversionDialog dlg;

    dlg.exec();
}

void KipToolMainWindow::on_spinSliceIndex_valueChanged(int arg1)
{
    QSignalBlocker blocker(ui->slider_images);

    ui->slider_images->setValue(arg1);
    on_slider_images_sliderMoved(arg1);
}

void KipToolMainWindow::on_spinSliceIndex_editingFinished()
{
    on_spinSliceIndex_valueChanged(ui->spinSliceIndex->value());
}

void KipToolMainWindow::on_actionReport_a_bug_triggered()
{
    QUrl url=QUrl("https://github.com/neutronimaging/imagingsuite/issues");
    if (!QDesktopServices::openUrl(url)) {
        QMessageBox dlg;
        dlg.setText("MuhRec could not open your web browser with the link https://github.com/neutronimaging/tools/issues");

        dlg.exec();
    }
}

void KipToolMainWindow::on_actionRegister_for_news_letter_triggered()
{
    QUrl url=QUrl("http://www.imagingscience.ch/newsletter/");
    if (!QDesktopServices::openUrl(url)) {
        QMessageBox dlg;
        dlg.setText("KipTool could not open your web browser with the link http://www.imagingscience.ch/newsletter/");
        dlg.exec();
    }
}

void KipToolMainWindow::on_actionUser_manual_triggered()
{
    QUrl url=QUrl("https://github.com/neutronimaging/imagingsuite/wiki/User-manual-KipTool");
    if (!QDesktopServices::openUrl(url)) {
        QMessageBox dlg;
        dlg.setText("MuhRec could not open your web browser with the link https://github.com/neutronimaging/imagingsuite/wiki/User-manual-KipTool");
        dlg.exec();
    }
}

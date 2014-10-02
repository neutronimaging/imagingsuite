#include <sstream>

#include <QFileDialog>
#include <QMessageBox>
#include "muhrecmainwindow.h"
#include "ui_muhrecmainwindow.h"
#include "configuregeometrydialog.h"
#include "findskiplistdialog.h"
#include "recondialog.h"
#include "viewgeometrylistdialog.h"

#include <base/timage.h>
#include <math/mathfunctions.h>
#include <base/thistogram.h>
#include <strings/string2array.h>
#include <strings/filenames.h>
#include <io/DirAnalyzer.h>

#include <BackProjectorBase.h>
#include <ReconHelpers.h>
#include <ReconException.h>
#include <ProjectionReader.h>

#include <ModuleException.h>
#include <ParameterHandling.h>

#include <set>
#include <sstream>
#include <string>


MuhRecMainWindow::MuhRecMainWindow(QApplication *app, QWidget *parent) :
    QMainWindow(parent),
    logger("MuhRec2MainWindow"),
    ui(new Ui::MuhRecMainWindow),
    m_QtApp(app),
    m_ModuleConfigurator(&m_Config),
    m_pEngine(NULL),
    m_nCurrentPage(0),
    m_nRequiredMemory(0),
    m_sApplicationPath(app->applicationDirPath().toStdString()),
    m_sConfigFilename("noname.xml"),
    m_bCurrentReconStored(true)
{
    std::ostringstream msg;
    kipl::strings::filenames::CheckPathSlashes(m_sApplicationPath,true);
    ui->setupUi(this);
    kipl::logging::Logger::AddLogTarget(*(ui->logviewer));
    logger(kipl::logging::Logger::LogMessage,"Enter c'tor");
    ui->projectionViewer->hold_annotations(true);
    std::string defaultmodules;
#ifdef Q_OS_WIN
        defaultmodules=m_sApplicationPath+"/StdBackProjectors.dll";
#else
    #ifdef Q_OS_MAC
        defaultmodules = m_sApplicationPath+"../Frameworks/libStdBackProjectors.dylib";
    #else
        defaultmodules = m_sApplicationPath+"/../Frameworks/libStdBackProjectors.so";
    #endif
#endif
    ui->ConfiguratorBackProj->Configure("muhrecbp",defaultmodules,m_sApplicationPath);

#ifdef Q_OS_WIN
        defaultmodules=m_sApplicationPath+"/StdPreprocModules.dll";
#else
    #ifdef Q_OS_MAC
        defaultmodules = m_sApplicationPath+"../Frameworks/libStdPreprocModules.dylib";
    #else
        defaultmodules = m_sApplicationPath+"/../Frameworks/libStdPreprocModules.so";
    #endif
#endif

    ui->moduleconfigurator->configure("muhrec",m_sApplicationPath,&m_ModuleConfigurator);
    ui->moduleconfigurator->SetDefaultModuleSource(defaultmodules);
    ui->moduleconfigurator->SetApplicationObject(this);

    LoadDefaults();
    UpdateDialog();
    ProjectionIndexChanged(0);
    ReconROIChanged(0);
        SetupCallBacks();

}

MuhRecMainWindow::~MuhRecMainWindow()
{
    delete ui;
}


void MuhRecMainWindow::SetupCallBacks()
{
    // Connecting buttons
    connect(ui->buttonProjectionPath,SIGNAL(clicked()),this,SLOT(BrowseProjectionPath()));
 //   connect(ui->buttonBrowseReference,SIGNAL(clicked()),this,SLOT(BrowseReferencePath()));
    connect(ui->buttonBrowseDestinationPath,SIGNAL(clicked()),this,SLOT(BrowseDestinationPath()));
    connect(ui->buttonTakePath,SIGNAL(clicked()),this,SLOT(TakeProjectionPath()));
    connect(ui->buttonPreview,SIGNAL(clicked()),this,SLOT(PreviewProjection()));
    connect(ui->buttonGetDoseROI,SIGNAL(clicked()),this,SLOT(GetDoseROI()));
    connect(ui->buttonGetMatrixROI,SIGNAL(clicked()),this,SLOT(GetMatrixROI()));
    connect(ui->buttonGetSkipList,SIGNAL(clicked()),this,SLOT(GetSkipList()));



    connect(ui->buttonSaveMatrix, SIGNAL(clicked()), this, SLOT(SaveMatrix()));

    // Dose roi size
    connect(ui->spinDoseROIx0,SIGNAL(valueChanged(int)),this,SLOT(DoseROIChanged(int)));
    connect(ui->spinDoseROIy0,SIGNAL(valueChanged(int)),this,SLOT(DoseROIChanged(int)));
    connect(ui->spinDoseROIx1,SIGNAL(valueChanged(int)),this,SLOT(DoseROIChanged(int)));
    connect(ui->spinDoseROIy1,SIGNAL(valueChanged(int)),this,SLOT(DoseROIChanged(int)));

    // Matrix roi size
    connect(ui->spinMatrixROI0,SIGNAL(valueChanged(int)),this,SLOT(MatrixROIChanged(int)));
    connect(ui->spinMatrixROI1,SIGNAL(valueChanged(int)),this,SLOT(MatrixROIChanged(int)));
    connect(ui->spinMatrixROI2,SIGNAL(valueChanged(int)),this,SLOT(MatrixROIChanged(int)));
    connect(ui->spinMatrixROI3,SIGNAL(valueChanged(int)),this,SLOT(MatrixROIChanged(int)));
    connect(ui->checkUseMatrixROI,SIGNAL(stateChanged(int)),this,SLOT(UseMatrixROI(int)));

    // Graylevels
    connect(ui->dspinGrayLow,SIGNAL(valueChanged(double)),this,SLOT(GrayLevelsChanged(double)));
    connect(ui->dspinGrayHigh,SIGNAL(valueChanged(double)),this,SLOT(GrayLevelsChanged(double)));

    // Display projections
    connect(ui->sliderProjections,SIGNAL(sliderMoved(int)),this,SLOT(PreviewProjection(int)));
  //  connect(ui->buttonPreview,SIGNAL(clicked()),this,SLOT(PreviewProjection()));
    connect(ui->spinFirstProjection,SIGNAL(valueChanged(int)),this,SLOT(ProjectionIndexChanged(int)));
    connect(ui->spinLastProjection,SIGNAL(valueChanged(int)),this,SLOT(ProjectionIndexChanged(int)));
    connect(ui->comboFlipProjection,SIGNAL(currentIndexChanged(int)),this,SLOT(PreviewProjection(int)));
    connect(ui->comboRotateProjection,SIGNAL(currentIndexChanged(int)),this,SLOT(PreviewProjection(int)));

    // Display slices
    connect(ui->sliderSlices,SIGNAL(sliderMoved(int)),this,SLOT(DisplaySlice(int)));

    // Recon ROI
    connect(ui->spinMarginLeft,SIGNAL(valueChanged(int)),this,SLOT(ReconROIChanged(int)));
    connect(ui->spinMarginRight,SIGNAL(valueChanged(int)),this,SLOT(ReconROIChanged(int)));
    connect(ui->spinSlicesFirst,SIGNAL(valueChanged(int)),this,SLOT(ReconROIChanged(int)));
    connect(ui->spinSlicesLast,SIGNAL(valueChanged(int)),this,SLOT(ReconROIChanged(int)));
    connect(ui->buttonGetReconROI,SIGNAL(clicked()),this,SLOT(GetReconROI()));

    connect(ui->dspinRotationCenter,SIGNAL(valueChanged(double)),this,SLOT(CenterOfRotationChanged(double)));
    connect(ui->dspinTiltAngle,SIGNAL(valueChanged(double)),this,SLOT(CenterOfRotationChanged(double)));
    connect(ui->dspinTiltPivot,SIGNAL(valueChanged(double)),this,SLOT(CenterOfRotationChanged(double)));
    connect(ui->checkCorrectTilt,SIGNAL(stateChanged(int)),this,SLOT(CenterOfRotationChanged(int)));
    connect(ui->buttonConfigGeometry,SIGNAL(clicked()),this,SLOT(ConfigureGeometry()));

    // Menus
    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(MenuFileNew()));
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(MenuFileOpen()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(MenuFileSave()));
    connect(ui->actionSave_As,SIGNAL(triggered()),this,SLOT(MenuFileSaveAs()));
    connect(ui->actionQuit,SIGNAL(triggered()),this,SLOT(MenuFileQuit()));
    connect(ui->actionStartReconstruction,SIGNAL(triggered()),this,SLOT(MenuReconstructStart()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(MenuHelpAbout()));

    connect(ui->actionStore_geometry,SIGNAL(triggered()),this,SLOT(StoreGeometrySetting()));
    connect(ui->actionView_geometry_list,SIGNAL(triggered()),this,SLOT(ViewGeometryList()));
    connect(ui->actionClear_list,SIGNAL(triggered()),this,SLOT(ClearGeometrySettings()));
}

void MuhRecMainWindow::BrowseProjectionPath()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select location of the projections",
                                      ui->editProjectionMask->text());
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

        ui->editProjectionMask->setText(QString::fromStdString(fi.m_sMask));
    }
}

void MuhRecMainWindow::on_buttonBrowseReference_clicked()
{
    std::string sPath, sFname;
    std::vector<std::string> exts;
    if (ui->editOpenBeamMask->text().isEmpty())
        kipl::strings::filenames::StripFileName(ui->editProjectionMask->text().toStdString(),sPath,sFname,exts);
    else
        kipl::strings::filenames::StripFileName(ui->editOpenBeamMask->text().toStdString(),sPath,sFname,exts);

//    QString projdir=QFileDialog::getOpenFileName(this,
//                                      "Select a dark current file",
//                                      QString::fromStdString(sPath));

    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select location of the open-beam projections",
                                      ui->editOpenBeamMask->text());

    if (!projdir.isEmpty()) {
        kipl::io::DirAnalyzer da;
        kipl::io::FileItem fi=da.GetFileMask(projdir.toStdString());

        ui->editOpenBeamMask->setText(QString::fromStdString(fi.m_sMask));
    }
}

void MuhRecMainWindow::BrowseDestinationPath()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location the reconstructed slices",
                                      ui->editDestPath->text());

    if (!projdir.isEmpty())
        ui->editDestPath->setText(projdir);
}

void MuhRecMainWindow::TakeProjectionPath()
{

    ui->editOpenBeamMask->setText(ui->editProjectionMask->text());
}

void MuhRecMainWindow::ProjectionIndexChanged(int x)
{
    ui->sliderProjections->setMaximum(ui->spinLastProjection->value());
    ui->sliderProjections->setMinimum(ui->spinFirstProjection->value());
    PreviewProjection();
}

void MuhRecMainWindow::PreviewProjection(int x)
{
    std::ostringstream msg;
    ProjectionReader reader;

    if (x<0) {
       int slice = ui->spinFirstProjection->value();
       ui->sliderProjections->setValue(slice);
    }

    msg.str("");
    try {
  //      std::string path=ui->editProjectionMask->text().toStdString();

        std::string fmask=ui->editProjectionMask->text().toStdString();
    //    kipl::strings::filenames::CheckPathSlashes(fmask,true);
        std::string name, ext;
        kipl::strings::filenames::MakeFileName(fmask,ui->sliderProjections->value(),name,ext,'#','0');

        if (QFile::exists(QString::fromStdString(name))) {

            m_PreviewImage=reader.Read("",fmask,static_cast<size_t>(ui->sliderProjections->value()),
                            static_cast<kipl::base::eImageFlip>(ui->comboFlipProjection->currentIndex()),
                            static_cast<kipl::base::eImageRotate>(ui->comboRotateProjection->currentIndex()),
                            static_cast<float>(ui->spinProjectionBinning->value()),NULL);

            float lo,hi;

            if (x < 0) {
                const size_t NHist=512;
                size_t hist[NHist];
                float axis[NHist];
                size_t nLo=0;
                size_t nHi=0;
                kipl::base::Histogram(m_PreviewImage.GetDataPtr(),m_PreviewImage.Size(),hist,NHist,0.0f,0.0f,axis);
                kipl::base::FindLimits(hist, NHist, 99.0f, &nLo, &nHi);
                lo=axis[nLo];
                hi=axis[nHi];

                ui->projectionViewer->set_image(m_PreviewImage.GetDataPtr(),m_PreviewImage.Dims(),lo,hi);
            }
            else {

                ui->projectionViewer->get_levels(&lo,&hi);
                ui->projectionViewer->set_image(m_PreviewImage.GetDataPtr(),m_PreviewImage.Dims(),lo,hi);
            }

            SetImageDimensionLimits(m_PreviewImage.Dims());
            UpdateMemoryUsage(m_Config.ProjectionInfo.roi);
        }
        else {
            QMessageBox mbox(this);
            msg.str("");
            msg<<"Could not load the image "<<name<<std::endl<<"the file does not exist.";
            logger(kipl::logging::Logger::LogError,msg.str());
            mbox.setText(QString::fromStdString(msg.str()));
            mbox.exec();
        }
    }
    catch (ReconException &e) {
        msg.str("");
        msg<<"Could not load the projection for preview: \n"<<e.what();
        logger(kipl::logging::Logger::LogMessage,msg.str());
        PreviewProjection( ui->spinFirstProjection->value());
    }
    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"Could not load the projection for preview: \n"<<e.what();
        logger(kipl::logging::Logger::LogMessage,msg.str());
        PreviewProjection( ui->spinFirstProjection->value());
    }
}

void MuhRecMainWindow::PreviewProjection()
{
    PreviewProjection(-1);
}

void MuhRecMainWindow::DisplaySlice(int x)
{
    if (m_pEngine==NULL)
        return;

    std::ostringstream msg;
    int nSelectedSlice=x;

    if (x<0) {
        nSelectedSlice=m_Config.MatrixInfo.nDims[2]/2;
        ui->sliderProjections->setValue(nSelectedSlice);
    }

    try {
        //kipl::base::TImage<float,2> slice=m_pEngine->GetSlice(nSelectedSlice-m_Config.ProjectionInfo.roi[1]);
        kipl::base::TImage<float,2> slice=m_pEngine->GetSlice(nSelectedSlice,m_eSlicePlane);
        ui->sliceViewer->set_image(slice.GetDataPtr(),slice.Dims(),
                                   static_cast<float>(ui->dspinGrayLow->value()),
                                   static_cast<float>(ui->dspinGrayHigh->value()));

    }
    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"Failed to display slice \n"<<e.what();
        logger(kipl::logging::Logger::LogMessage,msg.str());
    }

}

void MuhRecMainWindow::DisplaySlice()
{
    DisplaySlice(-1);
}
void MuhRecMainWindow::GetSkipList()
{
    UpdateConfig();

    FindSkipListDialog dlg;

    int res=dlg.exec(m_Config);

    if (res==QDialog::Accepted)
        ui->editProjectionSkipList->setText(dlg.getSkipList());

}

void MuhRecMainWindow::GetDoseROI()
{
    QRect rect=ui->projectionViewer->get_marked_roi();

    if (rect.width()*rect.height()!=0)
    {
        ui->spinDoseROIx0->blockSignals(true);
        ui->spinDoseROIy0->blockSignals(true);
        ui->spinDoseROIx1->blockSignals(true);
        ui->spinDoseROIy1->blockSignals(true);
        ui->spinDoseROIx0->setValue(rect.x());
        ui->spinDoseROIy0->setValue(rect.y());
        ui->spinDoseROIx1->setValue(rect.x()+rect.width());
        ui->spinDoseROIy1->setValue(rect.y()+rect.height());
        ui->spinDoseROIx0->blockSignals(false);
        ui->spinDoseROIy0->blockSignals(false);
        ui->spinDoseROIx1->blockSignals(false);
        ui->spinDoseROIy1->blockSignals(false);
        UpdateDoseROI();
    }
}

void MuhRecMainWindow::UpdateDoseROI()
{
    QRect rect;

    rect.setCoords(ui->spinDoseROIx0->value(),
                   ui->spinDoseROIy0->value(),
                   ui->spinDoseROIx1->value(),
                   ui->spinDoseROIy1->value());

    ui->projectionViewer->set_rectangle(rect,QColor("green").light(),0);
}

void MuhRecMainWindow::SetImageDimensionLimits(const size_t *const dims)
{
    ui->spinDoseROIx0->setMaximum(dims[0]-1);
    ui->spinDoseROIy0->setMaximum(dims[1]-1);
    ui->spinDoseROIx1->setMaximum(dims[0]-1);
    ui->spinDoseROIy1->setMaximum(dims[1]-1);

    ui->spinMarginLeft->setMaximum(dims[0]-1);
    ui->spinMarginRight->setMaximum(dims[0]-1);
    ui->spinSlicesFirst->setMaximum(dims[1]-1);
    ui->spinSlicesLast->setMaximum(dims[1]-1);
}

void MuhRecMainWindow::GetReconROI()
{
    QRect rect=ui->projectionViewer->get_marked_roi();

    if (rect.width()*rect.height()!=0)
    {
        ui->spinMarginLeft->blockSignals(true);
        ui->spinMarginRight->blockSignals(true);
        ui->spinSlicesFirst->blockSignals(true);
        ui->spinSlicesLast->blockSignals(true);
        ui->spinMarginLeft->setValue(rect.x());
        ui->spinSlicesFirst->setValue(rect.y());
        ui->spinMarginRight->setValue(rect.x()+rect.width());
        ui->spinSlicesLast->setValue(rect.y()+rect.height());
        ui->spinMarginLeft->blockSignals(false);
        ui->spinMarginRight->blockSignals(false);
        ui->spinSlicesFirst->blockSignals(false);
        ui->spinSlicesLast->blockSignals(false);
        ReconROIChanged(0);
    }
}

void MuhRecMainWindow::BinningChanged()
{

}

void MuhRecMainWindow::FlipChanged()
{

}

void MuhRecMainWindow::RotateChanged()
{

}

void MuhRecMainWindow::DoseROIChanged(int x)
{
    UpdateDoseROI();
}

void MuhRecMainWindow::ReconROIChanged(int x)
{
    QRect rect;
    size_t * dims=m_Config.ProjectionInfo.roi;

    rect.setCoords(dims[0]=ui->spinMarginLeft->value(),
                   dims[1]=ui->spinSlicesFirst->value(),
                   dims[2]=ui->spinMarginRight->value(),
                   dims[3]=ui->spinSlicesLast->value());

    ui->projectionViewer->set_rectangle(rect,QColor("yellow"),1);
    CenterOfRotationChanged();
    UpdateMemoryUsage(dims);
}

void MuhRecMainWindow::CenterOfRotationChanged(int x)
{
    CenterOfRotationChanged();
}

void  MuhRecMainWindow::CenterOfRotationChanged(double x)
{
    CenterOfRotationChanged();
}

void MuhRecMainWindow::CenterOfRotationChanged()
{
    double pos=ui->dspinRotationCenter->value()+ui->spinMarginLeft->value();
    QVector<QPointF> coords;
    coords.push_back(QPointF(pos,0));
    coords.push_back(QPointF(pos,m_PreviewImage.Size(1)));


    if (ui->checkCorrectTilt->checkState()) {
        double pivot=ui->dspinTiltPivot->value();
        double tantilt=tan(ui->dspinTiltAngle->value()*3.1415/180.0);
        coords[0].setX(coords[0].x()-tantilt*pivot);
        coords[1].setX(coords[1].x()+tantilt*(coords[1].y()-pivot));
    }

    ui->projectionViewer->set_plot(coords,QColor("red").light(),0);
}

void MuhRecMainWindow::ConfigureGeometry()
{
 ConfigureGeometryDialog dlg;

 UpdateConfig();

 int res=dlg.exec(m_Config);

 if (res==QDialog::Accepted) {
    dlg.GetConfig(m_Config);
    UpdateDialog();
    UpdateMemoryUsage(m_Config.ProjectionInfo.roi);
 }
}

void MuhRecMainWindow::StoreGeometrySetting()
{
    if (!m_bCurrentReconStored)
    {
        std::ostringstream msg;
        if (m_LastMidSlice.Size()!=0) {
            m_StoredReconList.push_back(std::make_pair(m_LastReconConfig,m_LastMidSlice));
            msg<<"Stored last recon config (list size "<<m_StoredReconList.size()<<")";
        }
        else
            msg<<"Data was not reconstructed, no geometry was stored";

        logger(kipl::logging::Logger::LogMessage,msg.str());
        m_bCurrentReconStored = true;
    }
}

void MuhRecMainWindow::ClearGeometrySettings()
{
    if (!m_StoredReconList.empty())
    {
        QMessageBox confirm_dlg;

        confirm_dlg.setStandardButtons(QMessageBox::Ok | QMessageBox::Abort);
        confirm_dlg.setDefaultButton(QMessageBox::Abort);
        confirm_dlg.setText("Do you want to clear the list with stored reconstruction settings?");
        confirm_dlg.setWindowTitle("Clear configuration list");

        int res=confirm_dlg.exec();

        if (res==QMessageBox::Ok) {
            logger(kipl::logging::Logger::LogMessage, "The list with stored configurations was cleared.");
            m_StoredReconList.clear();
            m_bCurrentReconStored = false;
        }
    }
}

void MuhRecMainWindow::ViewGeometryList()
{
    if (!m_StoredReconList.empty()) {
        ViewGeometryListDialog dlg;
        dlg.setList(m_StoredReconList);
        int res=dlg.exec();

        if (res==QDialog::Accepted) {
            if (dlg.changedConfigFields() & ConfigField_Tilt) {
                float center,tilt, pivot;
                dlg.getTilt(center, tilt, pivot);
                ui->dspinTiltAngle->setValue(tilt);
                ui->dspinTiltPivot->setValue(pivot);
                ui->dspinRotationCenter->setValue(center);
            }

            if (dlg.changedConfigFields() & ConfigField_ROI) {

                dlg.getROI(m_Config.MatrixInfo.roi);

                ui->spinMatrixROI0->setValue(m_Config.MatrixInfo.roi[0]);
                ui->spinMatrixROI1->setValue(m_Config.MatrixInfo.roi[1]);
                ui->spinMatrixROI2->setValue(m_Config.MatrixInfo.roi[2]);
                ui->spinMatrixROI3->setValue(m_Config.MatrixInfo.roi[3]);
            }
        }
    }
}

void MuhRecMainWindow::GrayLevelsChanged(double x)
{
    double low=ui->dspinGrayLow->value();
    double high=ui->dspinGrayHigh->value();
    ui->sliceViewer->set_levels(low,high);
    ui->plotHistogram->setPlotCursor(0,QtAddons::PlotCursor(low,QColor("green"),QtAddons::PlotCursor::Vertical));
    ui->plotHistogram->setPlotCursor(1,QtAddons::PlotCursor(high,QColor("green"),QtAddons::PlotCursor::Vertical));
}

void MuhRecMainWindow::GetMatrixROI()
{
    QRect rect=ui->sliceViewer->get_marked_roi();

    if (rect.width()*rect.height()!=0)
    {
        ui->spinMatrixROI0->blockSignals(true);
        ui->spinMatrixROI1->blockSignals(true);
        ui->spinMatrixROI2->blockSignals(true);
        ui->spinMatrixROI3->blockSignals(true);
        if (m_Config.MatrixInfo.bUseROI) {
            ui->spinMatrixROI0->setValue(rect.x()+m_Config.MatrixInfo.roi[0]);
            ui->spinMatrixROI1->setValue(rect.y()+m_Config.MatrixInfo.roi[1]);
            ui->spinMatrixROI2->setValue(rect.x()+rect.width()+m_Config.MatrixInfo.roi[0]);
            ui->spinMatrixROI3->setValue(rect.y()+rect.height()+m_Config.MatrixInfo.roi[1]);
        }
        else {
            ui->spinMatrixROI0->setValue(rect.x());
            ui->spinMatrixROI1->setValue(rect.y());
            ui->spinMatrixROI2->setValue(rect.x()+rect.width());
            ui->spinMatrixROI3->setValue(rect.y()+rect.height());
        }
        ui->spinMatrixROI0->blockSignals(false);
        ui->spinMatrixROI1->blockSignals(false);
        ui->spinMatrixROI2->blockSignals(false);
        ui->spinMatrixROI3->blockSignals(false);
        UpdateMatrixROI();
    }
}

void MuhRecMainWindow::MatrixROIChanged(int x)
{
    logger(kipl::logging::Logger::LogMessage,"MatrixROI changed");
    UpdateMatrixROI();
}

void MuhRecMainWindow::UpdateMatrixROI()
{
    logger(kipl::logging::Logger::LogMessage,"Update MatrixROI");
    QRect rect;

    rect.setCoords(ui->spinMatrixROI0->value(),
                   ui->spinMatrixROI1->value(),
                   ui->spinMatrixROI2->value(),
                   ui->spinMatrixROI3->value());

    ui->sliceViewer->set_rectangle(rect,QColor("green"),0);
}

void MuhRecMainWindow::UseMatrixROI(int x)
{
    if (x) {
        ui->spinMatrixROI0->show();
        ui->spinMatrixROI1->show();
        ui->spinMatrixROI2->show();
        ui->spinMatrixROI3->show();
        ui->buttonGetMatrixROI->show();
        ui->labelMX0->show();
        ui->labelMX1->show();
        ui->labelMX2->show();
        ui->labelMX3->show();
    }
    else
    {
        ui->spinMatrixROI0->hide();
        ui->spinMatrixROI1->hide();
        ui->spinMatrixROI2->hide();
        ui->spinMatrixROI3->hide();
        ui->buttonGetMatrixROI->hide();
        ui->labelMX0->hide();
        ui->labelMX1->hide();
        ui->labelMX2->hide();
        ui->labelMX3->hide();

    }
}

void MuhRecMainWindow::MenuFileNew()
{
    std::string defaultsname;
    #ifdef Q_OS_DARWIN
        defaultsname=m_sApplicationPath+"../Resources/defaults_mac.xml";
    #endif

    #ifdef Q_OS_WIN
         defaultsname="resources/defaults_windows.xml";
    #endif
    #ifdef Q_OS_LINUX
        defaultsname=m_sApplicationPath+"../resources/defaults_linux.xml";
    #endif

    std::ostringstream msg;

    kipl::strings::filenames::CheckPathSlashes(defaultsname,false);
    try {
        m_Config.LoadConfigFile(defaultsname.c_str(),"reconstructor");

    }
    catch (ReconException &e) {
        msg<<"Loading defaults failed :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
    }
    catch (ModuleException &e) {
        msg<<"Loading defaults failed :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Loading defaults failed :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
    }

    m_Config.ProjectionInfo.sPath              = QDir::homePath().toStdString();
    m_Config.ProjectionInfo.sReferencePath     = QDir::homePath().toStdString();
    m_Config.MatrixInfo.sDestinationPath       = QDir::homePath().toStdString();

//    size_t pos = 0;
//    std::string oldstr="@executable_path";
//    while((pos = m_Config.backprojector.m_sSharedObject.find(oldstr, pos)) != std::string::npos)
//    {
//       m_Config.backprojector.m_sSharedObject.replace(pos, oldstr.length(), m_sApplicationPath);
//       pos += m_sApplicationPath.length();
//    }

    logger(kipl::logging::Logger::LogMessage,m_Config.backprojector.m_sSharedObject);
    UpdateDialog();
    UpdateMemoryUsage(m_Config.ProjectionInfo.roi);
    m_sConfigFilename="noname.xml";
}

void MuhRecMainWindow::MenuFileOpen()
{
    std::ostringstream msg;
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open reconstruction configuration"),tr("configurations (*.xml)"));

    QMessageBox msgbox;

    msgbox.setWindowTitle(tr("Config file error"));
    msgbox.setText(tr("Failed to load the configuration file"));

    try {
        m_Config.LoadConfigFile(fileName.toStdString(),"reconstructor");
    }
    catch (ReconException & e) {
        msg<<"Failed to load the configuration file :\n"<<
             e.what();
        msgbox.setDetailedText(QString::fromStdString(msg.str()));
        msgbox.exec();
    }
    catch (ModuleException & e) {
        msg<<"Failed to load the configuration file :\n"<<
             e.what();
        msgbox.setDetailedText(QString::fromStdString(msg.str()));
        msgbox.exec();
    }
    catch (kipl::base::KiplException & e) {
        msg<<"Failed to load the configuration file :\n"<<
             e.what();
        msgbox.setDetailedText(QString::fromStdString(msg.str()));
        msgbox.exec();
    }
    catch (std::exception & e) {
        msg<<"Failed to load the configuration file :\n"<<
             e.what();
        msgbox.setDetailedText(QString::fromStdString(msg.str()));
        msgbox.exec();
    }

    UpdateDialog();

}

void MuhRecMainWindow::MenuFileSave()
{
    if (m_sConfigFilename=="noname.xml")
        MenuFileSaveAs();
    else {
        std::ofstream conffile(m_sConfigFilename.c_str());

        conffile<<m_Config.WriteXML();
    }
}

void MuhRecMainWindow::MenuFileSaveAs()
{
    QString fname=QFileDialog::getSaveFileName(this,"Save configuration file",QDir::homePath());

    m_sConfigFilename=fname.toStdString();
    std::ofstream conffile(m_sConfigFilename.c_str());

    conffile<<m_Config.WriteXML();
}

void MuhRecMainWindow::MenuFileQuit()
{
    m_QtApp->quit();
}

void MuhRecMainWindow::MenuHelpAbout()
{
    QMessageBox dlg;
    std::ostringstream msg;

    msg<<"MuhRec 3\nCompile date: "<<__DATE__<<" at "<<__TIME__<<std::endl;

    msg<<"Using \nQt version: 4.8.1\n"
      <<"LibTIFF, zLib, fftw3, libcfitsio";

    dlg.setText(QString::fromStdString(msg.str()));

    dlg.exec();
}

void MuhRecMainWindow::MenuReconstructStart()
{

    ui->tabMainControl->setCurrentIndex(4);
    ReconDialog dlg(&m_Interactor);
    bool bBuildFailure=false;

    ostringstream msg;
    ostringstream confpath;
    // Save current recon settings
    QDir dir;

    QString path=dir.homePath()+"/.imagingtools";

    logger(kipl::logging::Logger::LogMessage,path.toStdString());
    if (!dir.exists(path)) {
        dir.mkdir(path);
    }
    std::string sPath=path.toStdString();
    kipl::strings::filenames::CheckPathSlashes(sPath,true);
    confpath<<sPath<<"CurrentRecon.xml";
    try {
        UpdateConfig();
        ofstream of(confpath.str().c_str());
        if (!of.is_open()) {
            msg.str("");
            msg<<"Failed to open config file: "<<confpath.str()<<" for writing.";
            logger(kipl::logging::Logger::LogError,msg.str());
            return ;
        }

        of<<m_Config.WriteXML();
        of.flush();
    }
    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"Saving current config failed with exception: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return;
    }
    catch (std::exception &e) {
        msg.str("");
        msg<<"Saving current config failed with exception: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return;
    }


    if (m_pEngine!=NULL) {
        delete m_pEngine;
    }

    m_pEngine=NULL;

    m_Config.MatrixInfo.bAutomaticSerialize=false;
    if (m_Config.System.nMemory<m_nRequiredMemory) {
        msg.str("");
        msg<<"The requested matrix is larger ("<<m_nRequiredMemory<<"Mb) than the memorylimit ("
            <<m_Config.System.nMemory<<"Mb)\n\n"
            <<"Do you want to continue with reconstruction direct to disk?\n"
           <<"Current location is:\n"
          <<m_Config.MatrixInfo.sDestinationPath<<m_Config.MatrixInfo.sFileMask;
        QMessageBox largesize_dlg(this);
        largesize_dlg.setStandardButtons(QMessageBox::Ok | QMessageBox::Abort);
        largesize_dlg.setDefaultButton(QMessageBox::Ok);
        largesize_dlg.setText(QString::fromStdString(msg.str()));
        largesize_dlg.setWindowTitle("Matrix size warning");

        int res=largesize_dlg.exec();

        if (res!=QMessageBox::Ok) {
            return;
        }
        m_Config.MatrixInfo.bAutomaticSerialize=true;
    }
    else
        m_Config.MatrixInfo.bAutomaticSerialize=false;


    msg.str("");
    try {
        m_pEngine=m_Factory.BuildEngine(m_Config,&m_Interactor);
    }
    catch (std::exception &e) {
        msg<<"Reconstructor initialization failed with an STL exception: "<<endl
            <<e.what();
        bBuildFailure=true;
    }
    catch (ModuleException &e) {
        msg<<"Reconstructor initialization failed with a ModuleException: \n"
            <<e.what();
        bBuildFailure=true;
    }
    catch (ReconException &e) {
        msg<<"Reconstructor initialization failed with a recon exception: "<<endl
            <<e.what();
        bBuildFailure=true;
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Reconstructor initialization failed a Kipl exception: "<<endl
            <<e.what();
        bBuildFailure=true;
    }
    catch (...) {
        msg<<"Reconstructor initialization failed with an unknown exception";
        bBuildFailure=true;
    }

    if (bBuildFailure) {
        logger(kipl::logging::Logger::LogError,msg.str());
        QMessageBox error_dlg;
        error_dlg.setText("Failed to build reconstructor due to plugin exception. See log message for more information.");
        error_dlg.setDetailedText(QString::fromStdString(msg.str()));

        error_dlg.exec();

        if (m_pEngine!=NULL)
            delete m_pEngine;
        m_pEngine=NULL;

        return ;
    }

    msg.str("");
    bool bRunFailure=false;
    try {
        if (m_pEngine!=NULL) {
            int res=0;
            ui->tabMainControl->setCurrentIndex(3);
            res=dlg.exec(m_pEngine);

            if (res==QDialog::Accepted) {
                logger(kipl::logging::Logger::LogVerbose,"Finished with OK");

                // Store info about last recon
                m_LastReconConfig     = m_Config;
                m_bCurrentReconStored = false;
                size_t dims[3];
                m_pEngine->GetMatrixDims(dims);
                m_LastMidSlice = m_pEngine->GetSlice(dims[2]/2);

                // Prepare visualization
                if (m_Config.MatrixInfo.bAutomaticSerialize==false) {
                    PreviewProjection(); // Display the projection if it was not done already
                    ui->tabMainControl->setCurrentIndex(3);
                    if (ui->checkUseAutograyLevels->checkState()) {
                        const int nBins=256;
                        float x[nBins];
                        size_t y[nBins];
                        m_pEngine->GetHistogram(x,y,nBins);
                        ui->plotHistogram->setCurveData(0,x,y,nBins);

                        size_t lowlevel=0;
                        size_t highlevel=0;
                        kipl::base::FindLimits(y, nBins, 99.0, &lowlevel, &highlevel);
                        ui->dspinGrayLow->setValue(x[lowlevel]);
                        ui->dspinGrayHigh->setValue(x[highlevel]);

                        msg.str("");
                        msg<<kipl::math::Entropy(y+1,nBins-2);
                        ui->labelMatrixEntropy->setText(QString::fromStdString(msg.str()));
                    }

                    m_pEngine->GetMatrixDims(m_Config.MatrixInfo.nDims);
                    msg.str("");
                    msg<<"Reconstructed "<<m_Config.MatrixInfo.nDims[2]<<" slices";
                    logger(kipl::logging::Logger::LogMessage,msg.str());

//                    ui->sliderSlices->setRange(m_Config.ProjectionInfo.roi[1],m_Config.ProjectionInfo.roi[1]+m_Config.MatrixInfo.nDims[2]-1);
//                    ui->sliderSlices->setValue(m_Config.ProjectionInfo.roi[1]);
                    ui->sliderSlices->setRange(0,m_Config.MatrixInfo.nDims[2]-1);
                    ui->sliderSlices->setValue(m_Config.MatrixInfo.nDims[2]/2);
                    m_nSliceSizeX=m_Config.MatrixInfo.nDims[0];
                    m_nSliceSizeY=m_Config.MatrixInfo.nDims[1];
                    m_eSlicePlane=kipl::base::ImagePlaneXY;

                    msg.str("");
                    msg<<"Matrix display interval ["<<m_Config.MatrixInfo.fGrayInterval[0]<<", "<<m_Config.MatrixInfo.fGrayInterval[1]<<"]";
                    logger(kipl::logging::Logger::LogMessage,msg.str());

                    DisplaySlice();
                }
                else {
                    std::string fname=m_Config.MatrixInfo.sDestinationPath+"ReconConfig.xml";
                    kipl::strings::filenames::CheckPathSlashes(fname,false);
                    std::ofstream configfile(fname.c_str());
                    configfile<<m_Config.WriteXML();
                    configfile.close();

                    delete m_pEngine;
                    m_pEngine=NULL;
                }

            }
            if (res==QDialog::Rejected)
                logger(kipl::logging::Logger::LogVerbose,"Finished with Cancel");
        }
    }
    catch (std::exception &e) {
        msg<<"Reconstruction failed: "<<endl
            <<e.what();
        bRunFailure=true;
    }
    catch (ModuleException &e) {
        msg<<"Reconstruction failed with a module exception: \n"
            <<e.what();
        bRunFailure=true;
    }
    catch (ReconException &e) {
        msg<<"Reconstruction failed: "<<endl
            <<e.what();
        bRunFailure=true;
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Reconstruction failed: "<<endl
            <<e.what();
        bRunFailure=true;
    }

    catch (...) {
        msg<<"Reconstruction failed";
        bRunFailure=true;
    }

    if (bRunFailure) {
        logger(kipl::logging::Logger::LogError,msg.str());

        QMessageBox error_dlg;
        error_dlg.setText("Failed to run the reconstructor.");
        error_dlg.setDetailedText(QString::fromStdString(msg.str()));

        error_dlg.exec();
        if (m_pEngine!=NULL)
            delete m_pEngine;
        m_pEngine=NULL;

        return ;
    }
}

void MuhRecMainWindow::LoadDefaults()
{
    std::string defaultsname;
    QDir dir;
    QString currentname=dir.homePath()+"/.imagingtools/CurrentRecon.xml";

    bool bUseDefaults=true;
    if (dir.exists(currentname)) {
        defaultsname=currentname.toStdString();
        bUseDefaults=false;
    }
    else {
      //  m_QtApp->
    #ifdef Q_OS_WIN32
         defaultsname="resources/defaults_windows.xml";
    #else
        #ifdef Q_OS_LINUX
            defaultsname=m_sApplicationPath+"resources/defaults_linux.xml";
        #else
            #ifdef Q_OS_DARWIN
                defaultsname=m_sApplicationPath+"../Resources/defaults_mac.xml";
            #endif
        #endif
    #endif
        bUseDefaults=true;
    }
    std::ostringstream msg;

    kipl::strings::filenames::CheckPathSlashes(defaultsname,false);
    try {
        m_Config.LoadConfigFile(defaultsname.c_str(),"reconstructor");

    }
    catch (ReconException &e) {
        msg<<"Loading defaults failed :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
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
        m_Config.ProjectionInfo.sPath              = QDir::homePath().toStdString();
        m_Config.ProjectionInfo.sReferencePath     = QDir::homePath().toStdString();
        m_Config.MatrixInfo.sDestinationPath       = QDir::homePath().toStdString();

        std::list<ModuleConfig>::iterator it;

        std::string sSearchStr = "@executable_path";
        std::string sModulePath=m_QtApp->applicationDirPath().toStdString();
        size_t pos=0;
        for (it=m_Config.modules.begin(); it!=m_Config.modules.end(); it++) {
            pos=it->m_sSharedObject.find(sSearchStr);
            logger(kipl::logging::Logger::LogMessage,it->m_sSharedObject);
            if (pos!=std::string::npos)
                it->m_sSharedObject.replace(pos,sSearchStr.size(),sModulePath);
            logger(kipl::logging::Logger::LogMessage,it->m_sSharedObject);
        }
    }

    UpdateDialog();
    UpdateMemoryUsage(m_Config.ProjectionInfo.roi);
}

void MuhRecMainWindow::SaveMatrix()
{
    UpdateConfig();
    std::ostringstream msg;
    if (m_pEngine!=NULL) {
        try {
            m_pEngine->Serialize(&m_Config.MatrixInfo);

            std::string fname=m_Config.MatrixInfo.sDestinationPath+"ReconConfig.xml";
            kipl::strings::filenames::CheckPathSlashes(fname,false);
            std::ofstream configfile(fname.c_str());
            configfile<<m_Config.WriteXML();
            configfile.close();

//			int repdims[2]={595,842};
//			ReconReport report(repdims);

//			const int nBins=256;
//			float axis[nBins];
//			size_t hist[nBins];
//			engine->GetHistogram(axis,hist,nBins);
//			kipl::base::TImage<float,2> xy;
//			size_t dims[3];
//			engine->GetMatrixDims(dims);
//			xy=engine->GetSlice(dims[2]/2);
//			ostringstream reportname;
//			reportname<<config.MatrixInfo.sDestinationPath;
//			if (!config_filename.empty()) {
//				std::string path;
//				std::string name;
//				std::vector<std::string> extensions;
//				kipl::strings::filenames::StripFileName(config_filename,
//					path,name,extensions);
//				reportname<<name<<".pdf";
//			}
//			else
//				reportname<<"reconstruction_report.pdf";

//			report.CreateReport(reportname.str(),&config,&xy,&xy,&xy,hist,axis,nBins);
        }
        catch (ReconException &e) {
            msg<<"A recon exception occurred "<<e.what();
        }
        catch (kipl::base::KiplException &e) {
            msg<<"A kipl exception occurred "<<e.what();
        }
        catch (std::exception &e) {
            msg<<"A STL exception occurred "<<e.what();
        }
        catch (...) {
            msg<<"An unknown exception occurred ";
        }
        if (!msg.str().empty()) {
            QMessageBox msgdlg;

            msgdlg.setWindowTitle("Error");
            msgdlg.setText("Failed to save the reconstructed slices");
            msgdlg.setDetailedText(QString::fromStdString(msg.str()));
            msgdlg.exec();

            logger(kipl::logging::Logger::LogError,msg.str());
        }
    }
    else {
        logger(kipl::logging::Logger::LogWarning,"There is no matrix to save yet.");
        QMessageBox msgdlg;

        msgdlg.setWindowTitle("Error");
        msgdlg.setText("There is no matrix to save yet.");

        msgdlg.exec();
    }
}

void MuhRecMainWindow::UpdateMemoryUsage(size_t * roi)
{
    ostringstream msg;
    try  {
        ostringstream text;
        m_nRequiredMemory=0;
        double nMatrixMemory=0;
        double nBufferMemory=0;
        // Matrix size
        double length = abs(static_cast<ptrdiff_t>(roi[2])-static_cast<ptrdiff_t>(roi[0]));
        double height = abs(static_cast<ptrdiff_t>(roi[3])-static_cast<ptrdiff_t>(roi[1]));
        text.str("");

        nMatrixMemory = length*length*height*sizeof(float);
        // Memory for temp matrix
        double blocksize=GetIntParameter(m_Config.backprojector.parameters,"SliceBlock");
        blocksize=min(blocksize,height);
        nBufferMemory = length*length*blocksize*sizeof(float);
        // Projection buffer
        double projbuffersize=GetIntParameter(m_Config.backprojector.parameters,"ProjectionBufferSize");
        nBufferMemory += length*height*projbuffersize*sizeof(float);

        // Projection Data
        double nprojections=((double)m_Config.ProjectionInfo.nLastIndex-(double)m_Config.ProjectionInfo.nFirstIndex+1)/(double)m_Config.ProjectionInfo.nProjectionStep;
        nBufferMemory += length*height*nprojections*sizeof(float);

        nMatrixMemory/=1024*1024;
        nBufferMemory/=(1024*1024);
        text.str("");
        m_nRequiredMemory=static_cast<size_t>(nBufferMemory+nMatrixMemory);
        text<<"Memory usage: "<<m_nRequiredMemory
           <<"Mb (matrix: "<<ceil(nMatrixMemory)<<", buffers: "
           <<ceil(nBufferMemory)<<") system max "
           <<m_Config.System.nMemory<<"Mb";

        ui->statusBar->showMessage(QString::fromStdString(text.str()));
    }
    catch (ModuleException &e) {
        msg<<"Failed to compute the memory usage\n"<<e.what();
    }
    catch (ReconException &e) {
        msg<<"Failed to compute the memory usage\n"<<e.what();
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Failed to compute the memory usage\n"<<e.what();
    }
    catch (std::exception &e) {
        msg<<"Failed to compute the memory usage\n"<<e.what();
    }
    catch (...) {
        msg<<"Failed to compute the memory usage\n";
    }

    if (!msg.str().empty()) {
        QMessageBox error_dlg;
        error_dlg.setWindowTitle("Error");
        error_dlg.setText("Failed to build reconstructor due to plugin exception");
        error_dlg.setDetailedText(QString::fromStdString(msg.str()));
    }
}

void MuhRecMainWindow::UpdateDialog()
{
    std::ostringstream str;
 //   ui->editProjectionPath->setText(QString::fromStdString(m_Config.ProjectionInfo.sPath));
    ui->editProjectionMask->setText(QString::fromStdString(m_Config.ProjectionInfo.sFileMask));
    ui->spinFirstProjection->setValue(m_Config.ProjectionInfo.nFirstIndex);
    ui->spinLastProjection->setValue(m_Config.ProjectionInfo.nLastIndex);
    ui->spinProjectionStep->setValue(m_Config.ProjectionInfo.nProjectionStep);
    ui->comboProjectionStyle->setCurrentIndex(m_Config.ProjectionInfo.imagetype);
    ui->spinProjectionBinning->setValue(m_Config.ProjectionInfo.fBinning);
    ui->comboFlipProjection->setCurrentIndex(m_Config.ProjectionInfo.eFlip);
    ui->comboRotateProjection->setCurrentIndex(m_Config.ProjectionInfo.eRotate);
//    ui->editReferencePath->setText(QString::fromStdString(m_Config.ProjectionInfo.sReferencePath));
    ui->editOpenBeamMask->setText(QString::fromStdString(m_Config.ProjectionInfo.sOBFileMask));
    ui->spinFirstOpenBeam->setValue(m_Config.ProjectionInfo.nOBFirstIndex);
    ui->spinOpenBeamCount->setValue(m_Config.ProjectionInfo.nOBCount);
    ui->editDarkMask->setText(QString::fromStdString(m_Config.ProjectionInfo.sDCFileMask));
    ui->spinFirstDark->setValue(m_Config.ProjectionInfo.nDCFirstIndex);
    ui->spinDarkCount->setValue(m_Config.ProjectionInfo.nDCCount);
    ui->spinDoseROIx0->setValue(m_Config.ProjectionInfo.dose_roi[0]);
    ui->spinDoseROIy0->setValue(m_Config.ProjectionInfo.dose_roi[1]);
    ui->spinDoseROIx1->setValue(m_Config.ProjectionInfo.dose_roi[2]);
    ui->spinDoseROIy1->setValue(m_Config.ProjectionInfo.dose_roi[3]);
    ui->spinMarginLeft->setValue(m_Config.ProjectionInfo.roi[0]);
    ui->spinMarginRight->setValue(m_Config.ProjectionInfo.roi[2]);
    ui->spinSlicesFirst->setValue(m_Config.ProjectionInfo.roi[1]);
    ui->spinSlicesLast->setValue(m_Config.ProjectionInfo.roi[3]);
    ui->dspinRotationCenter->setValue(m_Config.ProjectionInfo.fCenter);
    ui->dspinAngleStart->setValue(m_Config.ProjectionInfo.fScanArc[0]);
    ui->dspinAngleStop->setValue(m_Config.ProjectionInfo.fScanArc[1]);
    ui->comboDataSequence->setCurrentIndex(m_Config.ProjectionInfo.scantype);
    ui->dspinResolution->setValue(m_Config.ProjectionInfo.fResolution[0]);
    ui->dspinTiltAngle->setValue(m_Config.ProjectionInfo.fTiltAngle);
    ui->dspinTiltPivot->setValue(m_Config.ProjectionInfo.fTiltPivotPosition);
    ui->checkCorrectTilt->setChecked(m_Config.ProjectionInfo.bCorrectTilt);
    ui->check_stitchprojections->setChecked(m_Config.ProjectionInfo.bTranslate);
    ui->moduleconfigurator->SetModules(m_Config.modules);
    ui->dspinRotateRecon->setValue(m_Config.MatrixInfo.fRotation);

    ui->dspinGrayLow->setValue(m_Config.MatrixInfo.fGrayInterval[0]);
    ui->dspinGrayHigh->setValue(m_Config.MatrixInfo.fGrayInterval[1]);

    ui->checkUseMatrixROI->setChecked(m_Config.MatrixInfo.bUseROI);
    UseMatrixROI(m_Config.MatrixInfo.bUseROI);
    ui->spinMatrixROI0->setValue(m_Config.MatrixInfo.roi[0]);
    ui->spinMatrixROI1->setValue(m_Config.MatrixInfo.roi[1]);
    ui->spinMatrixROI2->setValue(m_Config.MatrixInfo.roi[2]);
    ui->spinMatrixROI3->setValue(m_Config.MatrixInfo.roi[3]);

    ui->editDestPath->setText(QString::fromStdString(m_Config.MatrixInfo.sDestinationPath));
    ui->editSliceMask->setText(QString::fromStdString(m_Config.MatrixInfo.sFileMask));
    ui->comboDestFileType->setCurrentIndex(m_Config.MatrixInfo.FileType-2);
    // -2 to skip matlab types

    ui->editProjectName->setText(QString::fromStdString(m_Config.UserInformation.sProjectNumber));
    ui->editOperatorName->setText(QString::fromStdString(m_Config.UserInformation.sOperator));
    ui->editInstrumentName->setText(QString::fromStdString(m_Config.UserInformation.sInstrument));
    ui->editSampleDescription->setText(QString::fromStdString(m_Config.UserInformation.sSample));
    ui->editExperimentDescription->setText(QString::fromStdString(m_Config.UserInformation.sComment));

    str.str("");
    std::set<size_t>::iterator it;
    for (it=m_Config.ProjectionInfo.nlSkipList.begin(); it!=m_Config.ProjectionInfo.nlSkipList.end(); it++)
        str<<*it<<" ";
    ui->editProjectionSkipList->setText(QString::fromStdString(str.str()));
    ui->ConfiguratorBackProj->SetModule(m_Config.backprojector);
}

void MuhRecMainWindow::UpdateConfig()
{
    m_Config.ProjectionInfo.sPath="";
    m_Config.ProjectionInfo.sReferencePath="";

    m_Config.ProjectionInfo.sFileMask = ui->editProjectionMask->text().toStdString();
    //kipl::strings::filenames::CheckPathSlashes(m_Config.ProjectionInfo.sFileMask,true);
    m_Config.ProjectionInfo.nFirstIndex = ui->spinFirstProjection->value();
    m_Config.ProjectionInfo.nLastIndex = ui->spinLastProjection->value();
    m_Config.ProjectionInfo.nProjectionStep = ui->spinProjectionStep->value();
    m_Config.ProjectionInfo.imagetype = static_cast<ReconConfig::cProjections::eImageType>(ui->comboProjectionStyle->currentIndex());
    m_Config.ProjectionInfo.fBinning = ui->spinProjectionBinning->value();
    m_Config.ProjectionInfo.eFlip = static_cast<kipl::base::eImageFlip>(ui->comboFlipProjection->currentIndex());
    m_Config.ProjectionInfo.eRotate = static_cast<kipl::base::eImageRotate>(ui->comboRotateProjection->currentIndex());

//    m_Config.ProjectionInfo.sReferencePath = ui->editReferencePath->text().toStdString();
//    kipl::strings::filenames::CheckPathSlashes(m_Config.ProjectionInfo.sReferencePath,true);

    m_Config.ProjectionInfo.sOBFileMask = ui->editOpenBeamMask->text().toStdString();
    m_Config.ProjectionInfo.nOBFirstIndex = ui->spinFirstOpenBeam->value();
    m_Config.ProjectionInfo.nOBCount = ui->spinOpenBeamCount->value();
    m_Config.ProjectionInfo.sDCFileMask = ui->editDarkMask->text().toStdString();
    m_Config.ProjectionInfo.nDCFirstIndex = ui->spinFirstDark->value();
    m_Config.ProjectionInfo.nDCCount = ui->spinDarkCount->value();
    std::string str=ui->editProjectionSkipList->text().toStdString();
    if (!str.empty())
        kipl::strings::String2Set(str,m_Config.ProjectionInfo.nlSkipList);
    else
        m_Config.ProjectionInfo.nlSkipList.clear();

    m_Config.ProjectionInfo.dose_roi[0] = ui->spinDoseROIx0->value();
    m_Config.ProjectionInfo.dose_roi[1] = ui->spinDoseROIy0->value();
    m_Config.ProjectionInfo.dose_roi[2] = ui->spinDoseROIx1->value();
    m_Config.ProjectionInfo.dose_roi[3] = ui->spinDoseROIy1->value();

    m_Config.ProjectionInfo.roi[0] = ui->spinMarginLeft->value();
    m_Config.ProjectionInfo.roi[2] = ui->spinMarginRight->value();
    m_Config.ProjectionInfo.roi[1] = ui->spinSlicesFirst->value();
    m_Config.ProjectionInfo.roi[3] = ui->spinSlicesLast->value();

    m_Config.ProjectionInfo.fCenter = ui->dspinRotationCenter->value();
    m_Config.ProjectionInfo.fScanArc[0] = ui->dspinAngleStart->value();
    m_Config.ProjectionInfo.fScanArc[1] = ui->dspinAngleStop->value();
    m_Config.ProjectionInfo.scantype = static_cast<ReconConfig::cProjections::eScanType>(ui->comboDataSequence->currentIndex());
    m_Config.ProjectionInfo.fResolution[0] = ui->dspinResolution->value();
    m_Config.ProjectionInfo.fTiltAngle = ui->dspinTiltAngle->value();
    m_Config.ProjectionInfo.fTiltPivotPosition = ui->dspinTiltPivot->value();
    m_Config.ProjectionInfo.bCorrectTilt = ui->checkCorrectTilt->checkState();
    m_Config.ProjectionInfo.bTranslate = ui->check_stitchprojections->checkState();
    m_Config.modules = ui->moduleconfigurator->GetModules();
    m_Config.MatrixInfo.fRotation= ui->dspinRotateRecon->value();
    m_Config.MatrixInfo.fGrayInterval[0] = ui->dspinGrayLow->value();
    m_Config.MatrixInfo.fGrayInterval[1] = ui->dspinGrayHigh->value();
    m_Config.MatrixInfo.bUseROI = ui->checkUseMatrixROI->checkState();
    m_Config.MatrixInfo.roi[0] = ui->spinMatrixROI0->value();
    m_Config.MatrixInfo.roi[1] = ui->spinMatrixROI1->value();
    m_Config.MatrixInfo.roi[2] = ui->spinMatrixROI2->value();
    m_Config.MatrixInfo.roi[3] = ui->spinMatrixROI3->value();
    m_Config.MatrixInfo.sDestinationPath = ui->editDestPath->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_Config.MatrixInfo.sDestinationPath,true);
    m_Config.MatrixInfo.sFileMask = ui->editSliceMask->text().toStdString();
    m_Config.MatrixInfo.FileType = static_cast<kipl::io::eFileType>(ui->comboDestFileType->currentIndex()+2);
    // +2 to skip the matlab file types

    m_Config.UserInformation.sProjectNumber = ui->editProjectName->text().toStdString();
    m_Config.UserInformation.sOperator = ui->editOperatorName->text().toStdString();
    m_Config.UserInformation.sInstrument = ui->editInstrumentName->text().toStdString();
    m_Config.UserInformation.sSample = ui->editSampleDescription->text().toStdString();
    m_Config.UserInformation.sComment = ui->editExperimentDescription->toPlainText().toStdString();
    m_Config.backprojector = ui->ConfiguratorBackProj->GetModule();
}


void MuhRecMainWindow::on_buttonBrowseDC_clicked()
{
    std::string sPath, sFname;
    std::vector<std::string> exts;
    kipl::strings::filenames::StripFileName(ui->editProjectionMask->text().toStdString(),sPath,sFname,exts);
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select a dark current file",
                                      QString::fromStdString(sPath));
    if (!projdir.isEmpty()) {
        std::string pdir=projdir.toStdString();

        kipl::io::DirAnalyzer da;
        kipl::io::FileItem fi=da.GetFileMask(pdir);

        ui->editDarkMask->setText(QString::fromStdString(fi.m_sMask));
    }
}

void MuhRecMainWindow::on_buttonGetPathDC_clicked()
{
    ui->editDarkMask->setText(ui->editProjectionMask->text());
}

void MuhRecMainWindow::on_comboSlicePlane_activated(int index)
{
    std::ostringstream msg;
    m_eSlicePlane = static_cast<kipl::base::eImagePlanes>(1<<index);
    size_t dims[3];
    m_pEngine->GetMatrixDims(dims);
    int maxslices=static_cast<int>(dims[2-index]);
    ui->sliderSlices->setMaximum(maxslices-1);
    ui->sliderSlices->setValue(maxslices/2);

    msg<<"Changed slice plane to "<<m_eSlicePlane<<" max slices="<<maxslices;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    DisplaySlice(maxslices/2);

    switch (m_eSlicePlane) {
        case kipl::base::ImagePlaneXY :
            m_nSliceSizeX=dims[0]-1;
            m_nSliceSizeY=dims[1]-1;
        break;
        case kipl::base::ImagePlaneXZ :
            m_nSliceSizeX=dims[0]-1;
            m_nSliceSizeY=dims[2]-1;
        break;
        case kipl::base::ImagePlaneYZ :
            m_nSliceSizeX=dims[1]-1;
            m_nSliceSizeY=dims[2]-1;
        break;
    }
}

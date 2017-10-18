//<LICENSE>

#include <set>
#include <sstream>
#include <string>
#include <iomanip>

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDate>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>
#include <QSignalBlocker>

#include <base/timage.h>
#include <base/KiplException.h>
#include <math/mathfunctions.h>
#include <base/thistogram.h>
#include <strings/string2array.h>
#include <strings/filenames.h>
#include <io/DirAnalyzer.h>
#include <generators/Sine2D.h>

#include <BackProjectorModuleBase.h>
#include <ReconHelpers.h>
#include <ReconException.h>
#include <ProjectionReader.h>

#include <ModuleException.h>
#include <ParameterHandling.h>

#include "muhrecmainwindow.h"
#include "ui_muhrecmainwindow.h"
#include "configuregeometrydialog.h"
#include "findskiplistdialog.h"
#include "recondialog.h"
#include "viewgeometrylistdialog.h"
#include "preferencesdialog.h"
#include "dialogtoobig.h"
#include "piercingpointdialog.h"


MuhRecMainWindow::MuhRecMainWindow(QApplication *app, QWidget *parent) :
    QMainWindow(parent),
    logger("MuhRec3MainWindow"),
    ui(new Ui::MuhRecMainWindow),
    m_QtApp(app),
    m_ModuleConfigurator(&m_Config),
    m_pEngine(nullptr),
    m_nCurrentPage(0),
    m_nRequiredMemory(0),
    m_sApplicationPath(app->applicationDirPath().toStdString()),
    m_sHomePath(QDir::homePath().toStdString()),
    m_sConfigFilename("noname.xml"),
    m_bCurrentReconStored(true)
{
    std::ostringstream msg;
    ui->setupUi(this);
    kipl::logging::Logger::AddLogTarget(*(ui->logviewer));
    logger(kipl::logging::Logger::LogMessage,"Enter c'tor");

    kipl::strings::filenames::CheckPathSlashes(m_sApplicationPath,true);
    kipl::strings::filenames::CheckPathSlashes(m_sHomePath,true);

    m_sConfigPath = m_sHomePath + ".imagingtools";
    kipl::strings::filenames::CheckPathSlashes(m_sConfigPath,true);

    QDir dir;

    if (!dir.exists(QString::fromStdString(m_sConfigPath))) {
        dir.mkdir(QString::fromStdString(m_sConfigPath));
    }

    msg.str("");
    msg<<"ApplicationPath = "<<m_sApplicationPath<<std::endl
      <<"HomePath         = "<<m_sHomePath<<std::endl
      <<"ConfigPath       = "<<m_sConfigPath<<std::endl;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    ui->projectionViewer->hold_annotations(true);
    std::string defaultmodules;
#ifdef Q_OS_WIN
        defaultmodules=m_sApplicationPath+"\\StdBackProjectors.dll";
#else
    #ifdef Q_OS_MAC
        defaultmodules = m_sApplicationPath+"../Frameworks/libStdBackProjectors.dylib";
    #else
        defaultmodules = m_sApplicationPath+"../Frameworks/libStdBackProjectors.so";
    #endif
#endif
    ui->ConfiguratorBackProj->Configure("muhrecbp",defaultmodules,m_sApplicationPath);

#ifdef Q_OS_WIN
        defaultmodules=m_sApplicationPath+"\\StdPreprocModules.dll";
#else
    #ifdef Q_OS_MAC
        defaultmodules = m_sApplicationPath+"../Frameworks/libStdPreprocModules.dylib";
    #else
        defaultmodules = m_sApplicationPath+"../Frameworks/libStdPreprocModules.so";
    #endif
#endif

    ui->moduleconfigurator->configure("muhrec",m_sApplicationPath,&m_ModuleConfigurator);
    ui->moduleconfigurator->SetDefaultModuleSource(defaultmodules);
    ui->moduleconfigurator->SetApplicationObject(this);

    LoadDefaults(true);
    UpdateDialog();
    ProjectionIndexChanged(0);
    ProjROIChanged(0);
    SlicesChanged(0);
    SetupCallBacks();

    if (ui->checkCBCT->isChecked()) SlicesCBCTChanged(0);

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

    // Proj ROI
    connect(ui->spinProjROIx0,SIGNAL(valueChanged(int)),this,SLOT(ProjROIChanged(int)));
    connect(ui->spinProjROIx1,SIGNAL(valueChanged(int)),this,SLOT(ProjROIChanged(int)));
    connect(ui->spinProjROIy0,SIGNAL(valueChanged(int)),this,SLOT(ProjROIChanged(int)));
    connect(ui->spinProjROIy1,SIGNAL(valueChanged(int)),this,SLOT(ProjROIChanged(int)));
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
    ostringstream msg;
    QString oldmask=ui->editProjectionMask->text();
    QString newmask;

    QString projdir=QFileDialog::getOpenFileName(this,
                                      tr("Select location of the projections"),
                                      ui->editProjectionMask->text());


    if (!projdir.isEmpty()) {

        kipl::io::DirAnalyzer da;
        kipl::io::FileItem fi = da.GetFileMask(projdir.toStdString());

        if (fi.m_sExt!="hdf") {

            std::string pdir=projdir.toStdString();

            kipl::io::DirAnalyzer da;
            fi=da.GetFileMask(pdir);
            newmask=QString::fromStdString(fi.m_sMask);
            ui->editProjectionMask->setText(newmask);
            int c=0;
            int f=0;
            int l=0;

            if ((fi.m_sExt=="txt") || (fi.m_sExt=="csv")) {
                da.AnalyzeFileList(fi.m_sMask,c);
                f=1;
                l=c;
            }
            else
                da.AnalyzeMatchingNames(fi.m_sMask,c,f,l);

            msg<<"Found "<<c<<" files for mask "<<fi.m_sMask<<" in the interval "<<f<<" to "<<l;
            logger(logger.LogMessage,msg.str());

            QSignalBlocker spinFirst(ui->spinFirstProjection);
            ui->spinFirstProjection->setValue(f);

            QSignalBlocker spinLast(ui->spinLastProjection);
            ui->spinLastProjection->setValue(l);


            if (oldmask!=newmask)
            {
                PreviewProjection(-1);
                ui->projectionViewer->clear_rectangle(-1);
                ui->projectionViewer->clear_plot(-1);
            }

        }
        else {

            ui->editProjectionMask->setText(projdir);

             double angles[2];
             size_t Nofimgs[2];
             ProjectionReader reader;
             reader.GetNexusInfo(projdir.toStdString(),Nofimgs,angles);

             ui->spinFirstProjection->setValue(static_cast<int>(Nofimgs[0]));
             ui->spinLastProjection->setValue(static_cast<int>(Nofimgs[1]));
             ui->dspinAngleStart->setValue(angles[0]);
             ui->dspinAngleStop->setValue(angles[1]);

             if (oldmask!=ui->editProjectionMask->text())
             {
                 PreviewProjection(-1);
                 ui->projectionViewer->clear_rectangle(-1);
                 ui->projectionViewer->clear_plot(-1);
             }
        }

    }



}

void MuhRecMainWindow::on_buttonBrowseReference_clicked()
{
    std::ostringstream msg;
    std::string sPath, sFname;
    std::vector<std::string> exts;
    if (ui->editOpenBeamMask->text().isEmpty())
        kipl::strings::filenames::StripFileName(ui->editProjectionMask->text().toStdString(),sPath,sFname,exts);
    else
        kipl::strings::filenames::StripFileName(ui->editOpenBeamMask->text().toStdString(),sPath,sFname,exts);

    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select location of the open-beam projections",
                                      ui->editOpenBeamMask->text());

    if (!projdir.isEmpty()) {
        kipl::io::DirAnalyzer da;
        kipl::io::FileItem fi=da.GetFileMask(projdir.toStdString());


        if (fi.m_sExt=="hdf"){
            ui->editOpenBeamMask->setText(projdir);
            ProjectionReader reader;
            size_t Nofimgs[2];
            reader.GetNexusInfo(projdir.toStdString(),Nofimgs, NULL);
            ui->spinFirstOpenBeam->setValue(static_cast<int>(Nofimgs[0]));
            ui->spinOpenBeamCount->setValue(static_cast<int>(Nofimgs[1]+1));
        }
        else {
            ui->editOpenBeamMask->setText(QString::fromStdString(fi.m_sMask));

            int c=0;
            int f=0;
            int l=0;

            da.AnalyzeMatchingNames(fi.m_sMask,c,f,l);

            msg<<"Found "<<c<<" files for mask "<<fi.m_sMask<<" in the interval "<<f<<" to "<<l;
            logger(logger.LogMessage,msg.str());

            ui->spinFirstOpenBeam->setValue(f);
            ui->spinOpenBeamCount->setValue(c);
        }
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

void MuhRecMainWindow::ProjectionIndexChanged(int UNUSED(x))
{
    int first=ui->spinFirstProjection->value();
    int last=ui->spinLastProjection->value();

    if (first<last) {
        ui->sliderProjections->setMaximum(last);
        ui->sliderProjections->setMinimum(first);

        PreviewProjection();
    }
}

void MuhRecMainWindow::PreviewProjection(int x)
{
    QSignalBlocker sliderSignal(ui->sliderProjections);

    std::ostringstream msg;
    ProjectionReader reader;


    if (x<=0) {
       int slice = ui->spinFirstProjection->value();
       ui->sliderProjections->setValue(slice);
    }

    msg.str("");
    msg<<"Preview slider info: max="
      <<ui->sliderProjections->maximum()<<", current="
     << ui->sliderProjections->value();

    logger(logger.LogMessage,msg.str());

    try {
        UpdateConfig();
        std::string fmask=ui->editProjectionMask->text().toStdString();

        std::string name, ext;
        size_t found;
        int position=ui->sliderProjections->value();

        std::map<float,ProjectionInfo> fileList;
        BuildFileList(&m_Config,&fileList);
        if (fileList.size()<position) // Workaround for bad BuildFileList implementation
        {
            logger(logger.LogWarning, "Projection slider out of list range.");
            return;
        }

        auto it=fileList.begin();
        if (position<=ui->sliderProjections->maximum())
            std::advance(it,position-1);
        else {
            logger(logger.LogError,"Slider out of range");
            return;
        }

        name=it->second.name;

        if (QFile::exists(QString::fromStdString(name))) {
            int sliderval=ui->sliderProjections->value();
            m_PreviewImage=reader.Read(name,
                            static_cast<kipl::base::eImageFlip>(ui->comboFlipProjection->currentIndex()),
                            static_cast<kipl::base::eImageRotate>(ui->comboRotateProjection->currentIndex()),
                            static_cast<float>(ui->spinProjectionBinning->value()),NULL);

            if (QFile::exists(QString::fromStdString(fmask)) || QFile::exists(QString::fromStdString(name))) {

                int sliderval=ui->sliderProjections->value();

                found = fmask.find("hdf");
                //           size_t zdims[2]={1,1};
                //            m_PreviewImage.Resize(zdims);

                try {
                if (found!=std::string::npos )
                {
                    m_PreviewImage=reader.ReadNexus(fmask,static_cast<size_t>(ui->sliderProjections->value()),
                                    static_cast<kipl::base::eImageFlip>(ui->comboFlipProjection->currentIndex()),
                                    static_cast<kipl::base::eImageRotate>(ui->comboRotateProjection->currentIndex()),
                                    static_cast<float>(ui->spinProjectionBinning->value()),NULL);


                }
                else
                {
                    m_PreviewImage=reader.Read("",fmask,static_cast<size_t>(ui->sliderProjections->value()),
                                    static_cast<kipl::base::eImageFlip>(ui->comboFlipProjection->currentIndex()),
                                    static_cast<kipl::base::eImageRotate>(ui->comboRotateProjection->currentIndex()),
                                    static_cast<float>(ui->spinProjectionBinning->value()),NULL);
                }

                if ( m_PreviewImage.Size()==0)
                { // this happens in case an empty image is returned by ReadNexus

                  QMessageBox mbox(this);
                  msg.str("");
                  msg<<"KiplException: Nexus format not supported\n";
                  logger(kipl::logging::Logger::LogError,msg.str());
                  mbox.setText(QString::fromStdString(msg.str()));
                  mbox.exec();
                }
            }
                catch(ReconException &e){
                    msg<<"ReconException: Reading file failed\n"<<e.what();

                    logger(kipl::logging::Logger::LogError,msg.str());
                    throw ReconException(msg.str(),__FILE__,__LINE__);

                }
                catch(kipl::base::KiplException &e){
                    msg<<"KiplException: Reading file failed\n"<<e.what();

                    logger(kipl::logging::Logger::LogError,msg.str());
                    throw ReconException(msg.str(),__FILE__,__LINE__);
                }

                float lo,hi;

                if (m_PreviewImage.Size()==1) {
                    logger(logger.LogWarning,"Could not read preview");
                    return ;
                }
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
                msg.str("");
                msg<<sliderval<<" ("<<std::fixed<<std::setprecision(2)<<sliderval * (ui->dspinAngleStop->value()-ui->dspinAngleStart->value())/
                     (ui->spinLastProjection->value()-ui->spinFirstProjection->value())<<" deg)";

                ui->label_projindex->setText(QString::fromStdString(msg.str()));

                SetImageDimensionLimits(m_PreviewImage.Dims());
                UpdateMemoryUsage(m_Config.ProjectionInfo.roi);
            }
            else {
                msg.str("");
                msg<<"Could not load the image "<<name<<std::endl<<"the file does not exist.";
                logger(kipl::logging::Logger::LogError,msg.str());
                throw ReconException(msg.str(),__FILE__,__LINE__);
            }
        }

    }
    catch (ReconException &e) {
        QMessageBox mbox(this);
        msg.str("");
        msg<<"Could not load the projection for preview: \n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        mbox.setText(QString::fromStdString(msg.str()));
        mbox.exec();

    }
    catch (kipl::base::KiplException &e) {
        QMessageBox mbox(this);
        msg.str("");
        msg<<"Could not load the projection for preview: \n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        mbox.setText(QString::fromStdString(msg.str()));
        mbox.exec();
    }

}

void MuhRecMainWindow::PreviewProjection()
{
    PreviewProjection(-1);
}

void MuhRecMainWindow::DisplaySlice(int x)
{
    QSignalBlocker blockSlider(ui->sliderSlices);

    if (m_pEngine==nullptr)
        return;

    std::ostringstream msg;
    int nSelectedSlice=x;

    if (x<0) {
        nSelectedSlice=m_Config.MatrixInfo.nDims[2]/2;
        ui->sliderProjections->setValue(nSelectedSlice);
    }

    try {
        kipl::base::TImage<float,2> slice=m_pEngine->GetSlice(nSelectedSlice,m_eSlicePlane);
        ui->sliceViewer->set_image(slice.GetDataPtr(),slice.Dims(),
                                   static_cast<float>(ui->dspinGrayLow->value()),
                                   static_cast<float>(ui->dspinGrayHigh->value()));
        msg.str("");
        if (ui->checkCBCT->isChecked()){
            msg<<x<<" ("<<x+ui->spinSubVolumeSizeZ0->value()<<")";
        }
        else {
             msg<<x<<" ("<<x+m_Config.ProjectionInfo.roi[1]<<")";
        }
        ui->label_sliceindex->setText(QString::fromStdString(msg.str()));

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
    ui->spinDoseROIx0->setMaximum(static_cast<int>(dims[0])-1);
    ui->spinDoseROIy0->setMaximum(static_cast<int>(dims[1])-1);
    ui->spinDoseROIx1->setMaximum(static_cast<int>(dims[0])-1);
    ui->spinDoseROIy1->setMaximum(static_cast<int>(dims[1])-1);

    ui->spinProjROIx0->setMaximum(static_cast<int>(dims[0])-1);
    ui->spinProjROIx1->setMaximum(static_cast<int>(dims[0])-1);
    ui->spinProjROIy0->setMaximum(static_cast<int>(dims[1])-1);
    ui->spinProjROIy1->setMaximum(static_cast<int>(dims[1])-1);

    ui->spinSlicesFirst->setMaximum(static_cast<int>(dims[1])-1);
    ui->spinSlicesLast->setMaximum(static_cast<int>(dims[1])-1);
}

void MuhRecMainWindow::GetReconROI()
{
    QRect rect=ui->projectionViewer->get_marked_roi();

    if (rect.width()*rect.height()!=0)
    {
        QSignalBlocker(ui->spinProjROIx0);
        QSignalBlocker(ui->spinProjROIx1);
        QSignalBlocker(ui->spinProjROIy0);
        QSignalBlocker(ui->spinProjROIy1);

        ui->spinProjROIx0->setValue(rect.x());
        ui->spinProjROIy0->setValue(rect.y());
        ui->spinProjROIx1->setValue(rect.x()+rect.width());
        ui->spinProjROIy1->setValue(rect.y()+rect.height());

//        ui->spinSlicesFirst->setMinimum(m_Config.ProjectionInfo.projection_roi[1]);
//        ui->spinSlicesLast->setMinimum(m_Config.ProjectionInfo.projection_roi[1]);

//        ui->spinSlicesFirst->setMaximum(m_Config.ProjectionInfo.projection_roi[3]);
//        ui->spinSlicesLast->setMaximum(m_Config.ProjectionInfo.projection_roi[3]);

        ProjROIChanged(0);
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

void MuhRecMainWindow::DoseROIChanged(int UNUSED(x))
{
    UpdateDoseROI();
}

void MuhRecMainWindow::ProjROIChanged(int UNUSED(x))
{
    QRect rect;
    size_t * dims=m_Config.ProjectionInfo.roi;

    rect.setCoords(dims[0]=ui->spinProjROIx0->value(),
                   dims[1]=ui->spinProjROIy0->value(),
                   dims[2]=ui->spinProjROIx1->value(),
                   dims[3]=ui->spinProjROIy1->value());

    rect=rect.normalized();

    ui->projectionViewer->set_rectangle(rect,QColor("yellow"),1);
    ui->spinSlicesFirst->setMinimum(dims[1]);
    ui->spinSlicesFirst->setMaximum(dims[3]);

    ui->spinSlicesLast->setMinimum(dims[1]);
    ui->spinSlicesLast->setMaximum(dims[3]);

    SlicesChanged(0);
    CenterOfRotationChanged();
    UpdateMemoryUsage(dims);

    if (ui->checkCBCT->isChecked()){
        ComputeVolumeSize(); // update the size of the output volume
        ui->spinSlicesFirst->setValue(ui->spinProjROIy0->value());
        ui->spinSlicesLast->setValue(ui->spinProjROIy1->value());
    }
}

void MuhRecMainWindow::CenterOfRotationChanged(int UNUSED(x))
{
    CenterOfRotationChanged();
}

void  MuhRecMainWindow::CenterOfRotationChanged(double UNUSED(x))
{
    CenterOfRotationChanged();
}

void MuhRecMainWindow::CenterOfRotationChanged()
{
    double pos=ui->dspinRotationCenter->value()+ui->spinProjROIx0->value();
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

    if (res==QDialog::Accepted)
    {
        dlg.GetConfig(m_Config);
        UpdateDialog();
        UpdateMemoryUsage(m_Config.ProjectionInfo.roi);
    }
}

void MuhRecMainWindow::StoreGeometrySetting()
{
    if (!m_bCurrentReconStored)
    {
        UpdateConfig();
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

        if (res==QMessageBox::Ok)
        {
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

void MuhRecMainWindow::GrayLevelsChanged(double UNUSED(x))
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

    if (m_Config.ProjectionInfo.beamgeometry == m_Config.ProjectionInfo.BeamGeometry_Cone){
        ComputeVolumeSize(); // updated volume size
        std::ostringstream msg;
        msg.str("");
        msg<<"Volume size update: "<<m_Config.MatrixInfo.nDims[0]<<" "<<m_Config.MatrixInfo.nDims[1]<<" "<<m_Config.MatrixInfo.nDims[2];
        logger(logger.LogMessage,msg.str());
    }
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
    LoadDefaults(false);
}

void MuhRecMainWindow::LoadDefaults(bool checkCurrent)
{
    std::ostringstream msg;

    QDir dir;

    std::string defaultsname=m_sHomePath+".imagingtools/CurrentRecon.xml";
    kipl::strings::filenames::CheckPathSlashes(defaultsname,false);


    std::string sModulePath=m_sApplicationPath;
    kipl::strings::filenames::CheckPathSlashes(sModulePath,true);

    msg.str("");
    msg<<"default name is "<<defaultsname<<" it "<<(dir.exists(QString::fromStdString(defaultsname))==true ? "exists" : "doesn't exist")<<" and should "<< (checkCurrent ? " " : "not ")<<"be used";
    logger(logger.LogMessage,msg.str());
    bool bUseDefaults=true;
    if ((checkCurrent==true) && // do we check for previous recons?
            (dir.exists(QString::fromStdString(defaultsname))==true)) { // is there a previous recon?
        bUseDefaults=false;
    }
    else {
#ifdef Q_OS_DARWIN
        defaultsname=m_sApplicationPath+"../Resources/defaults_mac.xml";
        sModulePath+="..";
#endif

#ifdef Q_OS_WIN
         defaultsname=m_sApplicationPath+"resources/defaults_windows.xml";
#endif

#ifdef Q_OS_LINUX
        defaultsname=m_sApplicationPath+"../resources/defaults_linux.xml";
        sModulePath+="..";
#endif
        bUseDefaults=true;
    }

    kipl::strings::filenames::CheckPathSlashes(defaultsname,false);
    msg.str("");
    msg<<"Looking for defaults at "<<defaultsname;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    msg.str("");
    try {
        m_Config.LoadConfigFile(defaultsname.c_str(),"reconstructor");
        msg.str("");
        msg<<m_Config.WriteXML();
        logger(logger.LogMessage,msg.str());
    }
    catch (ReconException &e) {
        msg.str("");
        msg<<"Loading defaults failed :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
    }
    catch (ModuleException &e) {
        msg.str("");
        msg<<"Loading defaults failed :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
    }
    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"Loading defaults failed :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
    }

    if (bUseDefaults) {
        m_Config.ProjectionInfo.sPath              = m_sHomePath;
        m_Config.ProjectionInfo.sReferencePath     = m_sHomePath;
        m_Config.MatrixInfo.sDestinationPath       = m_sHomePath;

        std::list<ModuleConfig>::iterator it;

        std::string sSearchStr = "@executable_path";

        // Replace template path by module path for pre processing
        size_t pos=0;

        logger(logger.LogMessage,"Updating path of preprocessing modules");
        for (it=m_Config.modules.begin(); it!=m_Config.modules.end(); it++) {
            pos=it->m_sSharedObject.find(sSearchStr);

            if (pos!=std::string::npos)
                it->m_sSharedObject.replace(pos,sSearchStr.size(),sModulePath);

            logger(kipl::logging::Logger::LogMessage,it->m_sSharedObject);
        }

        logger(logger.LogMessage,"Updating path of preprocessing modules");
        pos=m_Config.backprojector.m_sSharedObject.find(sSearchStr);
        if (pos!=std::string::npos)
            m_Config.backprojector.m_sSharedObject.replace(pos,sSearchStr.size(),sModulePath);

        logger(kipl::logging::Logger::LogMessage,m_Config.backprojector.m_sSharedObject);

        size_t dims[2]={100,100};
        kipl::base::TImage<float,2> img=kipl::generators::Sine2D::SineRings(dims,2.0f);
        ui->projectionViewer->set_image(img.GetDataPtr(),img.Dims());
        ui->sliceViewer->set_image(img.GetDataPtr(),img.Dims());
    }

    UpdateDialog();
    UpdateMemoryUsage(m_Config.ProjectionInfo.roi);
    m_sConfigFilename=m_sHomePath+"noname.xml";
}

void MuhRecMainWindow::MenuFileOpen()
{
    std::ostringstream msg;
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open reconstruction configuration"),tr("*.xml"));

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
        UpdateConfig();
        std::ofstream conffile(m_sConfigFilename.c_str());

        conffile<<m_Config.WriteXML();
    }
}

void MuhRecMainWindow::MenuFileSaveAs()
{
    QString fname=QFileDialog::getSaveFileName(this,"Save configuration file",QDir::homePath());

    UpdateConfig();
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

    msg<<"Using \nQt version: "<<qVersion()<<"\n"
      <<"LibTIFF, zLib, fftw3, libcfitsio";

    dlg.setText(QString::fromStdString(msg.str()));

    dlg.exec();
}

void MuhRecMainWindow::saveCurrentRecon()
{
    ostringstream confpath;
    ostringstream msg;
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
        logger(kipl::logging::Logger::LogMessage,"Saved current recon config");
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


}

void MuhRecMainWindow::MenuReconstructStart()
{
    ui->tabMainControl->setCurrentIndex(4);

    ostringstream msg;
    m_Config.MatrixInfo.bAutomaticSerialize=false;
    if (m_Config.System.nMemory<m_nRequiredMemory) {

        DialogTooBig largesize_dlg;

        largesize_dlg.SetFields(ui->editDestPath->text(),
                                ui->editSliceMask->text(),
                                false,
                                ui->spinSlicesFirst->value(),
                                ui->spinSlicesLast->value(),
                                ui->spinProjROIy0->value(),ui->spinProjROIy1->value());
        int res=largesize_dlg.exec();

        if (res!=QDialog::Accepted) {
            logger(logger.LogMessage,"Reconstruction was aborted");
            return;
        }
        m_Config.MatrixInfo.sDestinationPath = largesize_dlg.GetPath().toStdString();
        kipl::strings::filenames::CheckPathSlashes(m_Config.MatrixInfo.sDestinationPath,true);
        m_Config.MatrixInfo.sFileMask        = largesize_dlg.GetMask().toStdString();
        ui->editDestPath->setText(QString::fromStdString(m_Config.MatrixInfo.sDestinationPath));
        ui->editSliceMask->setText(QString::fromStdString(m_Config.MatrixInfo.sFileMask));

        msg.str("");
        msg<<"Reconstructing direct to folder "<<m_Config.MatrixInfo.sDestinationPath
          <<" using the mask "<< m_Config.MatrixInfo.sFileMask;
        logger(logger.LogMessage,msg.str());

        m_Config.MatrixInfo.bAutomaticSerialize=true;
    }
    else
        m_Config.MatrixInfo.bAutomaticSerialize=false;

    saveCurrentRecon();

    ExecuteReconstruction();
}

void MuhRecMainWindow::ExecuteReconstruction()
{
    std::ostringstream msg;
    std::list<string> freelist;
    bool bBuildFailure=false;

    freelist.push_back("grayinterval");
    freelist.push_back("center");
    freelist.push_back("operator");
    freelist.push_back("instrument");
    freelist.push_back("projectnumber");
    freelist.push_back("sample");
    freelist.push_back("comment");
    freelist.push_back("rotation");
    freelist.push_back("tiltangle");
    freelist.push_back("tiltpivot");
    freelist.push_back("correcttilt");

    freelist.push_back("usematrixroi");
    freelist.push_back("matrixroi");

    bool bRerunBackproj=!m_Config.ConfigChanged(m_LastReconConfig,freelist);
    msg.str(""); msg<<"Config has "<<(bRerunBackproj ? "not" : "")<<" changed";
    logger(kipl::logging::Logger::LogMessage,msg.str());

    if ( bRerunBackproj==false || m_pEngine==nullptr || m_Config.MatrixInfo.bAutomaticSerialize==true) {
        bRerunBackproj=false; // Just in case if other cases outruled re-run

        logger(kipl::logging::Logger::LogMessage,"Preparing for full recon");
        msg.str("");
        try {
            if (m_pEngine!=nullptr)
            {
                delete m_pEngine;
            }

            m_pEngine=nullptr;

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

            if (m_pEngine!=nullptr)
                delete m_pEngine;
            m_pEngine=nullptr;

            return ;
        }
    }
    else {
        logger(kipl::logging::Logger::LogMessage,"Preparing for back proj only");
        if (m_pEngine!=nullptr)
            m_pEngine->SetConfig(m_Config); // Set new recon parameters for the backprojector
        else {
            logger(logger.LogError,"No engine allocated");
            return;
        }
        bRerunBackproj=true;
    }


    msg.str("");

    ReconDialog dlg(&m_Interactor);
    bool bRunFailure=false;
    try {
        if (m_pEngine!=nullptr) {
            int res=0;
            ui->tabMainControl->setCurrentIndex(3);
            res=dlg.exec(m_pEngine,bRerunBackproj);

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

                    ui->sliderSlices->setRange(0,static_cast<int>(m_Config.MatrixInfo.nDims[2])-1);
                    ui->sliderSlices->setValue(static_cast<int>(m_Config.MatrixInfo.nDims[2]/2));
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
                    m_pEngine=nullptr;
                }

            }
            if (res==QDialog::Rejected)
            {
                logger(kipl::logging::Logger::LogVerbose,"Finished with Cancel");
                if (m_pEngine!=nullptr)
                {
                    delete m_pEngine;
                    m_pEngine = nullptr;
                }
            }
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
        if (m_pEngine!=nullptr)
            delete m_pEngine;
        m_pEngine=nullptr;

        return ;
    }
}

void MuhRecMainWindow::SaveMatrix()
{
    UpdateConfig();
    std::ostringstream msg;
    if (m_pEngine!=nullptr) {
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
        double height = 0;
        if (ui->checkCBCT->isChecked())
            height = abs(ui->spinSubVolumeSizeZ0->value()-ui->spinSubVolumeSizeZ1->value()+1);
        else
            height = abs(static_cast<ptrdiff_t>(roi[3])-static_cast<ptrdiff_t>(roi[1]));
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
//        m_nRequiredMemory=static_cast<size_t>(nMatrixMemory);
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
    QSignalBlocker blockFirstProjection(ui->spinFirstProjection);
    QSignalBlocker blockLastProjection(ui->spinLastProjection);
    QSignalBlocker blockFlipProjection(ui->comboFlipProjection);
    QSignalBlocker blockRotateProjection(ui->comboRotateProjection);

    ui->editProjectionMask->setText(QString::fromStdString(m_Config.ProjectionInfo.sFileMask));
    ui->editOpenBeamMask->setText(QString::fromStdString(m_Config.ProjectionInfo.sOBFileMask));
    ui->editDarkMask->setText(QString::fromStdString(m_Config.ProjectionInfo.sDCFileMask));

    ui->spinFirstProjection->setValue(static_cast<int>(m_Config.ProjectionInfo.nFirstIndex));
    ui->spinLastProjection->setValue(static_cast<int>(m_Config.ProjectionInfo.nLastIndex));
    ui->spinProjectionStep->setValue(static_cast<int>(m_Config.ProjectionInfo.nProjectionStep));
    ui->comboProjectionStyle->setCurrentIndex(m_Config.ProjectionInfo.imagetype);
    ui->spinProjectionBinning->setValue(m_Config.ProjectionInfo.fBinning);
    ui->comboFlipProjection->setCurrentIndex(m_Config.ProjectionInfo.eFlip);
    ui->comboRotateProjection->setCurrentIndex(m_Config.ProjectionInfo.eRotate);


    ui->spinFirstOpenBeam->setValue(static_cast<int>(m_Config.ProjectionInfo.nOBFirstIndex));
    ui->spinOpenBeamCount->setValue(static_cast<int>(m_Config.ProjectionInfo.nOBCount));


    ui->spinFirstDark->setValue(static_cast<int>(m_Config.ProjectionInfo.nDCFirstIndex));
    ui->spinDarkCount->setValue(static_cast<int>(m_Config.ProjectionInfo.nDCCount));

    ui->spinDoseROIx0->setValue(static_cast<int>(m_Config.ProjectionInfo.dose_roi[0]));
    ui->spinDoseROIy0->setValue(static_cast<int>(m_Config.ProjectionInfo.dose_roi[1]));
    ui->spinDoseROIx1->setValue(static_cast<int>(m_Config.ProjectionInfo.dose_roi[2]));
    ui->spinDoseROIy1->setValue(static_cast<int>(m_Config.ProjectionInfo.dose_roi[3]));

    QSignalBlocker blockSlicesFirst(ui->spinSlicesFirst);
    QSignalBlocker blockSlicesLast(ui->spinSlicesLast);
    ui->spinSlicesFirst->blockSignals(true);
    ui->spinSlicesLast->blockSignals(true);

    ui->spinProjROIx0->setValue(static_cast<int>(m_Config.ProjectionInfo.projection_roi[0]));
    ui->spinProjROIy0->setValue(static_cast<int>(m_Config.ProjectionInfo.projection_roi[1]));
    ui->spinProjROIx1->setValue(static_cast<int>(m_Config.ProjectionInfo.projection_roi[2]));
    ui->spinProjROIy1->setValue(static_cast<int>(m_Config.ProjectionInfo.projection_roi[3]));

    ui->spinSlicesFirst->setValue(static_cast<int>(m_Config.ProjectionInfo.roi[1]));
    ui->spinSlicesLast->setValue(static_cast<int>(m_Config.ProjectionInfo.roi[3]));
    ui->spinSlicesFirst->blockSignals(false);
    ui->spinSlicesLast->blockSignals(false);

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
    ui->spinMatrixROI0->setValue(static_cast<int>(m_Config.MatrixInfo.roi[0]));
    ui->spinMatrixROI1->setValue(static_cast<int>(m_Config.MatrixInfo.roi[1]));
    ui->spinMatrixROI2->setValue(static_cast<int>(m_Config.MatrixInfo.roi[2]));
    ui->spinMatrixROI3->setValue(static_cast<int>(m_Config.MatrixInfo.roi[3]));

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

    ui->dspinSDD->setValue(m_Config.ProjectionInfo.fSDD);
    ui->dspinSOD->setValue(m_Config.ProjectionInfo.fSOD);

    ui->dspinPiercPointX->setValue(m_Config.ProjectionInfo.fpPoint[0]);
    ui->dspinPiercPointY->setValue(m_Config.ProjectionInfo.fpPoint[1]);

    ui->comboDirRotation->setCurrentIndex(m_Config.ProjectionInfo.eDirection);

    if(m_Config.ProjectionInfo.beamgeometry == m_Config.ProjectionInfo.BeamGeometry_Cone) {
        ui->checkCBCT->setChecked(true);
//        ComputeVolumeSizeSpacing();
    }

//    if (ui->checkCBCT->isChecked()) {
//        m_Config.ProjectionInfo.beamgeometry = m_Config.ProjectionInfo.BeamGeometry_Cone;
//        ComputeVolumeSizeSpacing();
//        ui->TabGeometry->setTabEnabled(1,true);
//        ui->spinSlicesFirst->setValue(ui->spinProjROIy0->value());
//        ui->spinSlicesLast->setValue(ui->spinProjROIy1->value());
//        ui->spinSlicesFirst->hide();
//        ui->spinSlicesLast->hide();
//        ui->label_50->hide();
//        ui->label_51->hide();
//        ui->label_30->hide();
//    }
//    else {
//        m_Config.ProjectionInfo.beamgeometry = m_Config.ProjectionInfo.BeamGeometry_Parallel;
//        ui->TabGeometry->setTabEnabled(1,false);
//        ui->spinSlicesFirst->show();
//        ui->spinSlicesLast->show();
//        ui->label_50->show();
//        ui->label_51->show();
//        ui->label_30->show();
//    }


        ui->spinSubVolumeSizeZ0->setValue(static_cast<int>(m_Config.ProjectionInfo.projection_roi[3] - m_Config.MatrixInfo.voi[5]));
        ui->spinSubVolumeSizeZ1->setValue(static_cast<int>(m_Config.ProjectionInfo.projection_roi[3] - m_Config.MatrixInfo.voi[4]));

    ProjROIChanged(0);
    SlicesChanged(0);
    UpdateDoseROI();
}

void MuhRecMainWindow::UpdateConfig()
{
    m_Config.ProjectionInfo.sPath="";
    m_Config.ProjectionInfo.sReferencePath="";

    m_Config.ProjectionInfo.sFileMask = ui->editProjectionMask->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_Config.ProjectionInfo.sFileMask,false);
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
    kipl::strings::filenames::CheckPathSlashes(m_Config.ProjectionInfo.sOBFileMask,false);

    m_Config.ProjectionInfo.nOBFirstIndex = ui->spinFirstOpenBeam->value();
    m_Config.ProjectionInfo.nOBCount = ui->spinOpenBeamCount->value();
    m_Config.ProjectionInfo.sDCFileMask = ui->editDarkMask->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_Config.ProjectionInfo.sDCFileMask,false);

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

    m_Config.ProjectionInfo.projection_roi[0] = ui->spinProjROIx0->value();
    m_Config.ProjectionInfo.projection_roi[1] = ui->spinProjROIy0->value();
    m_Config.ProjectionInfo.projection_roi[2] = ui->spinProjROIx1->value();
    m_Config.ProjectionInfo.projection_roi[3] = ui->spinProjROIy1->value();

    m_Config.ProjectionInfo.roi[0] = ui->spinProjROIx0->value();
    m_Config.ProjectionInfo.roi[2] = ui->spinProjROIx1->value();
    m_Config.ProjectionInfo.roi[1] = ui->spinSlicesFirst->value();
    m_Config.ProjectionInfo.roi[3] = ui->spinSlicesLast->value();

    m_Config.ProjectionInfo.fCenter = ui->dspinRotationCenter->value();
    m_Config.ProjectionInfo.fScanArc[0] = ui->dspinAngleStart->value();
    m_Config.ProjectionInfo.fScanArc[1] = ui->dspinAngleStop->value();
    m_Config.ProjectionInfo.scantype = static_cast<ReconConfig::cProjections::eScanType>(ui->comboDataSequence->currentIndex());
    m_Config.ProjectionInfo.fResolution[0] = m_Config.ProjectionInfo.fResolution[1] = ui->dspinResolution->value();
    m_Config.ProjectionInfo.fTiltAngle = ui->dspinTiltAngle->value();
    m_Config.ProjectionInfo.fTiltPivotPosition = ui->dspinTiltPivot->value();
    m_Config.ProjectionInfo.bCorrectTilt = ui->checkCorrectTilt->checkState();
    m_Config.ProjectionInfo.bTranslate = ui->check_stitchprojections->checkState();

    m_Config.ProjectionInfo.fSDD = ui->dspinSDD->value();
    m_Config.ProjectionInfo.fSOD = ui->dspinSOD->value();

    m_Config.ProjectionInfo.fpPoint[0] = ui->dspinPiercPointX->value();
    m_Config.ProjectionInfo.fpPoint[1] = ui->dspinPiercPointY->value();

    if (ui->checkCBCT->isChecked()) {
        m_Config.ProjectionInfo.beamgeometry = m_Config.ProjectionInfo.BeamGeometry_Cone;
        ComputeVolumeSizeSpacing();
        m_Config.ProjectionInfo.roi[1] = m_Config.ProjectionInfo.projection_roi[1];
        m_Config.ProjectionInfo.roi[3] = m_Config.ProjectionInfo.projection_roi[3];
        ui->TabGeometry->setTabEnabled(1,true);
        ui->spinSlicesFirst->setValue(ui->spinProjROIy0->value());
        ui->spinSlicesLast->setValue(ui->spinProjROIy1->value());
        ui->spinSlicesFirst->hide();
        ui->spinSlicesLast->hide();
        ui->label_50->hide();
        ui->label_51->hide();
        ui->label_30->hide();
        ui->checkUseMatrixROI->hide();

        SlicesCBCTChanged(0);
    }
    else {
        m_Config.ProjectionInfo.beamgeometry= m_Config.ProjectionInfo.BeamGeometry_Parallel;
        m_Config.ProjectionInfo.roi[1] = ui->spinSlicesFirst->value();
        m_Config.ProjectionInfo.roi[3] = ui->spinSlicesLast->value();
        ui->TabGeometry->setTabEnabled(1,false);
        ui->spinSlicesFirst->show();
        ui->spinSlicesLast->show();
        ui->label_50->show();
        ui->label_51->show();
        ui->label_30->show();
        ui->checkUseMatrixROI->show();

        SlicesChanged(0);
    }



        m_Config.MatrixInfo.voi[0] = 0;
        m_Config.MatrixInfo.voi[1] = (m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0]);
        m_Config.MatrixInfo.voi[2] = 0;
        m_Config.MatrixInfo.voi[3] = (m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0]);
        m_Config.MatrixInfo.voi[4] = m_Config.ProjectionInfo.roi[3]-ui->spinSubVolumeSizeZ1->value();
        m_Config.MatrixInfo.voi[5] = m_Config.ProjectionInfo.roi[3]-ui->spinSubVolumeSizeZ0->value();


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

    m_Config.MatrixInfo.FileType = static_cast<kipl::io::eFileType>(ui->comboDestFileType->currentIndex()+2);
    m_Config.MatrixInfo.sFileMask = ui->editSliceMask->text().toStdString();

    // Validity test of the slice file mask
    if (m_Config.MatrixInfo.sFileMask.find_last_of('.')==std::string::npos) {
        logger(logger.LogWarning,"Destination file mask is missing a file extension. Adding .tif");
        m_Config.MatrixInfo.sFileMask.append(".tif");
    }

    ptrdiff_t pos=m_Config.MatrixInfo.sFileMask.find_last_of('.');
    if (m_Config.MatrixInfo.sFileMask.find('#')==std::string::npos) {
        logger(logger.LogWarning,"Destination file mask is missing an index mask. Adding '_####'' before file extension");
        m_Config.MatrixInfo.sFileMask.insert(pos,"_####");
    }
    ui->editSliceMask->setText(QString::fromStdString(m_Config.MatrixInfo.sFileMask));
    // +2 to skip the matlab file types

    m_Config.UserInformation.sProjectNumber = ui->editProjectName->text().toStdString();
    m_Config.UserInformation.sOperator = ui->editOperatorName->text().toStdString();
    m_Config.UserInformation.sInstrument = ui->editInstrumentName->text().toStdString();
    m_Config.UserInformation.sSample = ui->editSampleDescription->text().toStdString();
    m_Config.UserInformation.sComment = ui->editExperimentDescription->toPlainText().toStdString();
    m_Config.backprojector = ui->ConfiguratorBackProj->GetModule();

    m_Config.ProjectionInfo.fSDD = ui->dspinSDD->value();
    m_Config.ProjectionInfo.fSOD = ui->dspinSOD->value();
    m_Config.ProjectionInfo.fpPoint[0] = ui->dspinPiercPointX->value();
    m_Config.ProjectionInfo.fpPoint[1] = ui->dspinPiercPointY->value();
    m_Config.ProjectionInfo.eDirection = static_cast<kipl::base::eRotationDirection>(ui->comboDirRotation->currentIndex());

    m_Config.SanityCheck();
}


void MuhRecMainWindow::on_buttonBrowseDC_clicked()
{
    std::ostringstream msg;
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

        if (fi.m_sExt=="hdf"){
            ui->editDarkMask->setText(projdir);
            ProjectionReader reader;
            size_t Nofimgs[2];
            reader.GetNexusInfo(projdir.toStdString(),Nofimgs, NULL);
            ui->spinFirstDark->setValue(Nofimgs[0]);
            ui->spinDarkCount->setValue(Nofimgs[1]+1);
        }
        else {
            ui->editDarkMask->setText(QString::fromStdString(fi.m_sMask));

            int c=0;
            int f=0;
            int l=0;

            da.AnalyzeMatchingNames(fi.m_sMask,c,f,l);

            msg<<"Found "<<c<<" files for mask "<<fi.m_sMask<<" in the interval "<<f<<" to "<<l;
            logger(logger.LogMessage,msg.str());

            ui->spinFirstDark->setValue(f);
            ui->spinDarkCount->setValue(c);
        }
    }
}

void MuhRecMainWindow::on_buttonGetPathDC_clicked()
{
//    std::ostringstream msg;
//    kipl::io::DirAnalyzer da;
//    kipl::io::FileItem fi=da.GetFileMask(ui->editProjectionMask->text().toStdString());

//    if (fi.m_sExt=="hdf"){
//        ui->editDarkMask->setText(ui->editProjectionMask->text());
//    }
//    else {
//        std::string darkname=fi.m_sPath+"dc_*."+fi.m_sExt;
//        std::vector<std::string> flist=da.GetDirList(darkname);
//        if (flist.size()==0) {
//            darkname=fi.m_sPath+"dark_*."+fi.m_sExt;
//            flist=da.GetDirList(darkname);
//        }

//        if (flist.size()!=0) {
//            int c=0;
//            int f=0;
//            int l=0;
//            fi=da.GetFileMask(flist.front());
//            da.AnalyzeMatchingNames(fi.m_sMask,c,f,l);

//            msg<<"Found "<<c<<" files for mask "<<fi.m_sMask<<" in the interval "<<f<<" to "<<l;
//            logger(logger.LogMessage,msg.str());

//            ui->editDarkMask->setText(QString::fromStdString(fi.m_sMask));
//            ui->spinFirstDark->setValue(f);
//            ui->spinDarkCount->setValue(c);
//        }
//        else {
//            ui->editDarkMask->setText(ui->editProjectionMask->text());
//        }
//    }


    ui->editDarkMask->setText(ui->editProjectionMask->text());
}

void MuhRecMainWindow::on_comboSlicePlane_activated(int index)
{
    std::ostringstream msg;
    m_eSlicePlane = static_cast<kipl::base::eImagePlanes>(1<<index);
    size_t dims[3];
    if (m_pEngine!=nullptr)
    {
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
    else {
        logger(logger.LogWarning,"No engine allocated");
    }
}

void MuhRecMainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog dlg;
    dlg.m_MemoryLimit = static_cast<int>(m_Config.System.nMemory);
    dlg.m_LogLevel    = m_Config.System.eLogLevel;


    int res=dlg.exec();

    if (res==dlg.Accepted) {

        m_Config.System.nMemory   = dlg.m_MemoryLimit;
        m_Config.System.eLogLevel = dlg.m_LogLevel;
    }

}

void MuhRecMainWindow::on_actionReconstruct_to_disk_triggered()
{
    if (reconstructToDisk()) {
      saveCurrentRecon();
      ExecuteReconstruction();
    }
}

bool MuhRecMainWindow::reconstructToDisk()
{
    std::ostringstream msg;
    logger(logger.LogMessage,"Starting reconstruct to disk");

    DialogTooBig largesize_dlg;
    UpdateConfig();

    logger(logger.LogMessage,msg.str());
    if (m_Config.ProjectionInfo.beamgeometry==m_Config.ProjectionInfo.BeamGeometry_Parallel)
    {
        largesize_dlg.SetFields(ui->editDestPath->text(),
                                ui->editSliceMask->text(),
                                false,
                                m_Config.ProjectionInfo.roi[1], m_Config.ProjectionInfo.roi[3],
                                m_Config.ProjectionInfo.projection_roi[1],m_Config.ProjectionInfo.projection_roi[3]);
    }
    else if (m_Config.ProjectionInfo.beamgeometry==m_Config.ProjectionInfo.BeamGeometry_Cone)
    {
        largesize_dlg.SetFields(ui->editDestPath->text(),
                                ui->editSliceMask->text(),
                                false,
                                ui->spinSubVolumeSizeZ0->value(), ui->spinSubVolumeSizeZ1->value(),
                                m_Config.ProjectionInfo.projection_roi[1],m_Config.ProjectionInfo.projection_roi[3]);
    }

    int res=largesize_dlg.exec();

    if (res!=QDialog::Accepted) {
        logger(logger.LogMessage,"Reconstruction was aborted");
        return false;
    }

    m_Config.MatrixInfo.sDestinationPath = largesize_dlg.GetPath().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_Config.MatrixInfo.sDestinationPath,true);
    m_Config.MatrixInfo.sFileMask        = largesize_dlg.GetMask().toStdString();
    ui->editDestPath->setText(QString::fromStdString(m_Config.MatrixInfo.sDestinationPath));
    ui->editSliceMask->setText(QString::fromStdString(m_Config.MatrixInfo.sFileMask));

    if (m_Config.ProjectionInfo.beamgeometry==m_Config.ProjectionInfo.BeamGeometry_Parallel)
    {
        ui->spinSlicesFirst->setValue(largesize_dlg.GetFirst());
        ui->spinSlicesLast->setValue(largesize_dlg.GetLast());
    }
    else if (m_Config.ProjectionInfo.beamgeometry==m_Config.ProjectionInfo.BeamGeometry_Cone)
    {
        ui->spinSubVolumeSizeZ0->setValue(largesize_dlg.GetFirst());
        ui->spinSubVolumeSizeZ1->setValue(largesize_dlg.GetLast());
    }

    UpdateConfig();

    msg.str("");
    msg<<"Reconstructing slices "<<m_Config.ProjectionInfo.roi[1]<<" to "<<m_Config.ProjectionInfo.roi[3]<<"direct to folder "<<m_Config.MatrixInfo.sDestinationPath
      <<" using the mask "<< m_Config.MatrixInfo.sFileMask;
    logger(logger.LogMessage,msg.str());

    m_Config.MatrixInfo.bAutomaticSerialize=true;

    return true;
}

void MuhRecMainWindow::on_spinSlicesFirst_valueChanged(int arg1)
{
    SlicesChanged(arg1);
}

void MuhRecMainWindow::on_spinSlicesLast_valueChanged(int arg1)
{
    SlicesChanged(arg1);
}

void MuhRecMainWindow::SlicesChanged(int arg1)
{
    QRect rect;
    size_t * dims=m_Config.ProjectionInfo.roi;

    rect.setCoords(dims[0]=ui->spinProjROIx0->value(),
                   dims[1]=ui->spinSlicesFirst->value(),
                   dims[2]=ui->spinProjROIx1->value(),
                   dims[3]=ui->spinSlicesLast->value());

    ui->projectionViewer->set_rectangle(rect,QColor("lightblue"),2);

    UpdateMemoryUsage(dims);
}

void MuhRecMainWindow::on_actionRemove_CurrentRecon_xml_triggered()
{
    ostringstream confpath;
    ostringstream msg;
    // Save current recon settings

    QDir dir;
    QString filename=dir.homePath()+"/.imagingtools/CurrentRecon.xml";

    std::string name=filename.toStdString();
    kipl::strings::filenames::CheckPathSlashes(name,false);
    filename=QString::fromStdString(name);

    if (dir.exists(filename)) {
        QDate date=QDate::currentDate();
        QString destname=filename+"."+date.toString("yyyyMMdd");
        dir.rename(filename,destname);
        msg.str("");
        msg<<"Moved "<<filename.toStdString()<<" to "<<destname.toStdString();
        logger(logger.LogMessage,msg.str());
    }
    else {
        msg.str("");
        msg<<"Couldn't find "<<filename.toStdString();
        logger(logger.LogMessage,msg.str());
    }

}

void MuhRecMainWindow::on_actionReport_a_bug_triggered()
{
    QUrl url=QUrl("https://github.com/neutronimaging/imagingsuite/issues");
    if (!QDesktopServices::openUrl(url)) {
 //       QMessageBox dlg("Could not open repository","MuhRec could not open your web browser with the link https://github.com/neutronimaging/tools/issues");
    }
}

void MuhRecMainWindow::on_checkCBCT_clicked(bool checked)
{

    if (checked) {
        m_Config.ProjectionInfo.beamgeometry = m_Config.ProjectionInfo.BeamGeometry_Cone;
        ComputeVolumeSizeSpacing();

        std::ostringstream msg;
        msg.str("");
        msg<<"Volume for CBCT: "<<m_Config.MatrixInfo.nDims[0]<<" "<<m_Config.MatrixInfo.nDims[1]<<" "<<m_Config.MatrixInfo.nDims[2]
          <<" voxels with isotropic spacing: "<< m_Config.MatrixInfo.fVoxelSize[0];
        logger(logger.LogMessage,msg.str());

        ui->TabGeometry->setTabEnabled(1,true);
        ui->spinSlicesFirst->setValue(ui->spinProjROIy0->value());
        ui->spinSlicesLast->setValue(ui->spinProjROIy1->value());
        ui->spinSlicesFirst->hide();
        ui->spinSlicesLast->hide();
        ui->label_50->hide();
        ui->label_51->hide();
        ui->label_30->hide();
        ui->checkUseMatrixROI->hide();

        SlicesCBCTChanged(0);

    } else {
        m_Config.ProjectionInfo.beamgeometry = m_Config.ProjectionInfo.BeamGeometry_Parallel;
        ui->TabGeometry->setTabEnabled(1,false);
        ui->spinSlicesFirst->show();
        ui->spinSlicesLast->show();
        ui->label_50->show();
        ui->label_51->show();
        ui->label_30->show();
        ui->checkUseMatrixROI->show();

        SlicesChanged(0);
    }
}

void MuhRecMainWindow::ComputeVolumeSizeSpacing()
{
    ComputeVolumeSize();
    ComputeVoxelSpacing();
}

void MuhRecMainWindow::ComputeVolumeSize()
{
    float magn = m_Config.ProjectionInfo.fSDD/m_Config.ProjectionInfo.fSOD;
    // compute volume dimensions
    m_Config.MatrixInfo.nDims[0] = (m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0])*m_Config.ProjectionInfo.fResolution[0]/magn/m_Config.MatrixInfo.fVoxelSize[0];
    m_Config.MatrixInfo.nDims[1] = m_Config.MatrixInfo.nDims[0];
    m_Config.MatrixInfo.nDims[2] = (m_Config.ProjectionInfo.roi[3]-m_Config.ProjectionInfo.roi[1])*m_Config.ProjectionInfo.fResolution[0]/magn/m_Config.MatrixInfo.fVoxelSize[0];
}

void MuhRecMainWindow::ComputeVoxelSpacing()
{
    float magn = m_Config.ProjectionInfo.fSDD/m_Config.ProjectionInfo.fSOD;
    m_Config.MatrixInfo.fVoxelSize[0] = m_Config.MatrixInfo.fVoxelSize[1] = m_Config.MatrixInfo.fVoxelSize[2] = m_Config.ProjectionInfo.fResolution[0]/magn;
}



void MuhRecMainWindow::on_checkCBCT_stateChanged(int arg1)
{
    // probably this one is not needed

        if (ui->checkCBCT->isChecked()) {
            m_Config.ProjectionInfo.beamgeometry = m_Config.ProjectionInfo.BeamGeometry_Cone;
            ComputeVolumeSizeSpacing();
            ui->TabGeometry->setTabEnabled(1,true);
            ui->spinSlicesFirst->setValue(ui->spinProjROIy0->value());
            ui->spinSlicesLast->setValue(ui->spinProjROIy1->value());
            ui->spinSlicesFirst->hide();
            ui->spinSlicesLast->hide();
            ui->label_50->hide();
            ui->label_51->hide();
            ui->label_30->hide();

        }
        else {
            m_Config.ProjectionInfo.beamgeometry = m_Config.ProjectionInfo.BeamGeometry_Parallel;
            ui->TabGeometry->setTabEnabled(1,false);
            ui->spinSlicesFirst->show();
            ui->spinSlicesLast->show();
            ui->label_50->show();
            ui->label_51->show();
            ui->label_30->show();
        }

}

void MuhRecMainWindow::on_buttonGetPP_clicked()
{
    PiercingPointDialog dlg;
    UpdateConfig();

    int res=dlg.exec(m_Config);

    if (res==QDialog::Accepted) {
        pair<float,float> position=dlg.getPosition();

        m_Config.ProjectionInfo.fpPoint[0]=position.first;
        m_Config.ProjectionInfo.fpPoint[1]=position.second;
        UpdateDialog();
    }

}

void MuhRecMainWindow::on_dspinResolution_valueChanged(double arg1)
{
    if (m_Config.ProjectionInfo.beamgeometry == m_Config.ProjectionInfo.BeamGeometry_Cone){
        m_Config.ProjectionInfo.fResolution[0]=m_Config.ProjectionInfo.fResolution[1]= ui->dspinResolution->value();
        ComputeVoxelSpacing();
        std::ostringstream msg;
        msg.str("");
        msg<<"Voxel spacing for CBCT updated: "<< m_Config.MatrixInfo.fVoxelSize[0]<< " =" << m_Config.ProjectionInfo.fResolution[0] << "/magnification";
        logger(logger.LogMessage,msg.str());
    }
}



void MuhRecMainWindow::on_spinSubVolumeSizeZ0_valueChanged(int arg1)
{
    SlicesCBCTChanged(arg1);
}

void MuhRecMainWindow::on_spinSubVolumeSizeZ1_valueChanged(int arg1)
{
    SlicesCBCTChanged(arg1);
}

void MuhRecMainWindow::SlicesCBCTChanged(int x)
{
    QRect rect;

    rect.setCoords(ui->spinProjROIx0->value(),
                   ui->spinSubVolumeSizeZ0->value(),
                   ui->spinProjROIx1->value(),
                   ui->spinSubVolumeSizeZ1->value());

    ui->projectionViewer->set_rectangle(rect,QColor("lightblue"),2);
}

void MuhRecMainWindow::on_actionRegister_for_news_letter_triggered()
{
    QUrl url=QUrl("http://www.imagingscience.ch/newsletter/");
    if (!QDesktopServices::openUrl(url)) {
        QMessageBox dlg;
        dlg.setText("MuhRec could not open your web browser with the link http://www.imagingscience.ch/newsletter/");
        dlg.exec();
    }

}

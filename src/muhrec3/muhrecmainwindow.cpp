#include <sstream>

#include <QFileDialog>
#include "muhrecmainwindow.h"
#include "ui_muhrecmainwindow.h"

#include <base/timage.h>
#include <math/mathfunctions.h>
#include <base/thistogram.h>
#include <strings/string2array.h>
#include <strings/filenames.h>

#include <BackProjectorBase.h>
#include <ReconHelpers.h>
#include <ReconException.h>
#include <ProjectionReader.h>

#include <ModuleException.h>
#include <ParameterHandling.h>

#include <set>
#include <sstream>
#include <string>


MuhRecMainWindow::MuhRecMainWindow(std::string application_path, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MuhRecMainWindow),
    logger("MuhRec2MainWindow"),
    m_pEngine(NULL),
    m_nCurrentPage(0),
    m_sApplicationPath(application_path),
    m_sConfigFilename("noname.xml")
{
    std::ostringstream msg;
    ui->setupUi(this);
    kipl::logging::Logger::AddLogTarget(*(ui->logviewer));
    logger(kipl::logging::Logger::LogMessage,"Enter c'tor");

    msg<<"Application path:"<<m_sApplicationPath;
    logger(kipl::logging::Logger::LogMessage,msg.str());
    ui->projectionViewer->hold_annotations(true);
    UpdateDialog();
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
    connect(ui->buttonBrowseReference,SIGNAL(clicked()),this,SLOT(BrowseReferencePath()));
    connect(ui->buttonBrowseDestinationPath,SIGNAL(clicked()),this,SLOT(BrowseDestinationPath()));
    connect(ui->buttonTakePath,SIGNAL(clicked()),this,SLOT(TakeProjectionPath()));
    connect(ui->buttonPreview,SIGNAL(clicked()),this,SLOT(PreviewProjection()));
    connect(ui->buttonGetDoseROI,SIGNAL(clicked()),this,SLOT(GetDoseROI()));
    connect(ui->buttonGetMatrixROI,SIGNAL(clicked()),this,SLOT(GetMatrixROI()));

    connect(ui->buttonSaveMatrix, SIGNAL(clicked()), this, SLOT(SaveMatrix()));
    connect(ui->buttonConfigGeometry, SIGNAL(clicked()), this,SLOT(ConfigureGeometry()));

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
    connect(ui->buttonPreview,SIGNAL(clicked()),this,SLOT(PreviewProjection()));
    connect(ui->spinFirstProjection,SIGNAL(valueChanged(int)),this,SLOT(ProjectionIndexChanged(int)));
    connect(ui->spinLastProjection,SIGNAL(valueChanged(int)),this,SLOT(ProjectionIndexChanged(int)));
    connect(ui->comboFlipProjection,SIGNAL(currentIndexChanged(int)),this,SLOT(PreviewProjection(int)));
    connect(ui->comboRotateProjection,SIGNAL(currentIndexChanged(int)),this,SLOT(PreviewProjection(int)));

/*
    void BinningChanged();
    void FlipChanged();
    void RotateChanged();
    void CenterOfRotationChanged();
    void MatrixROIChanged();
*/
}

void MuhRecMainWindow::BrowseProjectionPath()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location of the projections",
                                      ui->editProjectionPath->text());

    if (!projdir.isEmpty())
        ui->editProjectionPath->setText(projdir);
}

void MuhRecMainWindow::BrowseReferencePath()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location of the reference projections",
                                      ui->editReferencePath->text());

    if (!projdir.isEmpty())
        ui->editReferencePath->setText(projdir);
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
    ui->editReferencePath->setText(ui->editProjectionPath->text());
}

void MuhRecMainWindow::ProjectionIndexChanged(int x)
{
    ui->sliderProjections->setMaximum(ui->spinLastProjection->value());
    ui->sliderProjections->setMinimum(ui->spinFirstProjection->value());
    PreviewProjection();
}

void MuhRecMainWindow::PreviewProjection(int x)
{
    PreviewProjection();
}

void MuhRecMainWindow::PreviewProjection()
{
    std::ostringstream msg;
    ProjectionReader reader;
    kipl::base::TImage<float,2> img;
    try {
        std::string path=ui->editProjectionPath->text().toStdString();
        kipl::strings::filenames::CheckPathSlashes(path,true);
        std::string fmask=ui->editProjectionMask->text().toStdString();

        msg.str("");
        msg<<"Path: "<<path<<", fmask: "<<fmask;
        logger(kipl::logging::Logger::LogMessage,msg.str());
        img=reader.Read(path,fmask,static_cast<size_t>(ui->sliderProjections->value()),
                        static_cast<kipl::base::eImageFlip>(ui->comboFlipProjection->currentIndex()),
                        static_cast<kipl::base::eImageRotate>(ui->comboRotateProjection->currentIndex()),
                        static_cast<float>(ui->spinProjectionBinning->value()),NULL);

        ui->projectionViewer->set_image(img.GetDataPtr(),img.Dims());
    }
    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"Could not load the projection for preview: \n"<<e.what();
        logger(kipl::logging::Logger::LogMessage,msg.str());
    }
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

    ui->projectionViewer->set_rectangle(rect,QColor("green"),0);
}

void MuhRecMainWindow::BinningChanged()
{}

void MuhRecMainWindow::FlipChanged()
{}

void MuhRecMainWindow::RotateChanged()
{}

void MuhRecMainWindow::DoseROIChanged(int x)
{
    UpdateDoseROI();
}

void MuhRecMainWindow::ReconROIChanged(int x)
{}

void MuhRecMainWindow::CenterOfRotationChanged()
{}

void MuhRecMainWindow::ConfigureGeometry()
{}

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
        ui->spinMatrixROI0->setValue(rect.x());
        ui->spinMatrixROI1->setValue(rect.y());
        ui->spinMatrixROI2->setValue(rect.x()+rect.width());
        ui->spinMatrixROI3->setValue(rect.x()+rect.height());
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

void MuhRecMainWindow::SaveMatrix()
{}

void MuhRecMainWindow::UpdateDialog()
{
    std::ostringstream str;
    ui->editProjectionPath->setText(QString::fromStdString(m_Config.ProjectionInfo.sPath));
    ui->editProjectionMask->setText(QString::fromStdString(m_Config.ProjectionInfo.sFileMask));
    ui->spinFirstProjection->setValue(m_Config.ProjectionInfo.nFirstIndex);
    ui->spinLastProjection->setValue(m_Config.ProjectionInfo.nLastIndex);
    ui->spinProjectionStep->setValue(m_Config.ProjectionInfo.nProjectionStep);
    ui->comboProjectionStyle->setCurrentIndex(m_Config.ProjectionInfo.imagetype);
    ui->spinProjectionBinning->setValue(m_Config.ProjectionInfo.fBinning);
    ui->comboFlipProjection->setCurrentIndex(m_Config.ProjectionInfo.eFlip);
    ui->comboRotateProjection->setCurrentIndex(m_Config.ProjectionInfo.eRotate);
    ui->editReferencePath->setText(QString::fromStdString(m_Config.ProjectionInfo.sReferencePath));
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
    ui->comboDestFileType->setCurrentIndex(m_Config.MatrixInfo.FileType);

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
}

void MuhRecMainWindow::UpdateConfig()
{
    m_Config.ProjectionInfo.sPath     = ui->editProjectionPath->text().toStdString();
    m_Config.ProjectionInfo.sFileMask = ui->editProjectionMask->text().toStdString();
    m_Config.ProjectionInfo.nFirstIndex = ui->spinFirstProjection->value();
    m_Config.ProjectionInfo.nLastIndex = ui->spinLastProjection->value();
    m_Config.ProjectionInfo.nProjectionStep = ui->spinProjectionStep->value();
    m_Config.ProjectionInfo.imagetype = static_cast<ReconConfig::cProjections::eImageType>(ui->comboProjectionStyle->currentIndex());
    m_Config.ProjectionInfo.fBinning = ui->spinProjectionBinning->value();
    m_Config.ProjectionInfo.eFlip = static_cast<kipl::base::eImageFlip>(ui->comboFlipProjection->currentIndex());
    m_Config.ProjectionInfo.eRotate = static_cast<kipl::base::eImageRotate>(ui->comboRotateProjection->currentIndex());

    m_Config.ProjectionInfo.sReferencePath = ui->editReferencePath->text().toStdString();
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
    m_Config.MatrixInfo.sFileMask = ui->editSliceMask->text().toStdString();
    m_Config.MatrixInfo.FileType = static_cast<kipl::io::eFileType>(ui->comboDestFileType->currentIndex());

    m_Config.UserInformation.sProjectNumber = ui->editProjectName->text().toStdString();
    m_Config.UserInformation.sOperator = ui->editOperatorName->text().toStdString();
    m_Config.UserInformation.sInstrument = ui->editInstrumentName->text().toStdString();
    m_Config.UserInformation.sSample = ui->editSampleDescription->text().toStdString();
    m_Config.UserInformation.sComment = ui->editExperimentDescription->toPlainText().toStdString();

}

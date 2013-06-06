#include <sstream>

#include "muhrecmainwindow.h"
#include "ui_muhrecmainwindow.h"
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
    UpdateDialog();

}

MuhRecMainWindow::~MuhRecMainWindow()
{
    delete ui;
}

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

    str.str("");
    std::set<size_t>::iterator it;
    for (it=m_Config.ProjectionInfo.nlSkipList.begin(); it!=m_Config.ProjectionInfo.nlSkipList.end(); it++)
        str<<*it<<" ";
    ui->editProjectionSkipList->setText(QString::fromStdString(str.str()));
}

void MuhRecMainWindow::UpdateConfig()
{}

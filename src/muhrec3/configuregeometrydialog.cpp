#include "configuregeometrydialog.h"
#include "ui_configuregeometrydialog.h"

#include <QMessageBox>

#include <ProjectionReader.h>
#include <ReconException.h>
#include <ReconHelpers.h>
#include <base/KiplException.h>
#include <strings/filenames.h>
#include <filters/medianfilter.h>

#include <exception>


ConfigureGeometryDialog::ConfigureGeometryDialog(QWidget *parent) :
    QDialog(parent),
    logger("ConfigGeometryDialog"),
    ui(new Ui::ConfigureGeometryDialog)
{
    ui->setupUi(this);
    connect(ui->buttonFindCenter,SIGNAL(clicked()),SLOT(FindCenter()));
    connect(ui->checkUseTilt,SIGNAL(toggled(bool)),SLOT(UseTilt(bool)));
}

ConfigureGeometryDialog::~ConfigureGeometryDialog()
{
    delete ui;
}

int ConfigureGeometryDialog::exec(ReconConfig &config)
{
    m_Config=config;
    int res=LoadImages();

    if (res<0)
        return QDialog::Rejected;

    ui->viewerProjection->set_image(m_Proj0Deg.GetDataPtr(),m_Proj0Deg.Dims(),0,1.3);

    UpdateDialog();

    return QDialog::exec();
}

void ConfigureGeometryDialog::GetConfig(ReconConfig & config)
{
    UpdateConfig();
    config=m_Config;
}

void ConfigureGeometryDialog::FindCenter()
{
    UpdateConfig();
}

void ConfigureGeometryDialog::UseTilt(bool x)
{
    if (x) {
        ui->dspinTiltAngle->show();
        ui->dspinTiltPivot->show();
        ui->labelTiltAngle->show();
        ui->labelTiltPivot->show();
    }
    else {
        ui->dspinTiltAngle->hide();
        ui->dspinTiltPivot->hide();
        ui->labelTiltAngle->hide();
        ui->labelTiltPivot->hide();
    }
}

int ConfigureGeometryDialog::LoadImages()
{
    kipl::base::TImage<float,2> img;
    std::ostringstream msg;

    ProjectionReader reader;

    msg.str("");
    QMessageBox loaderror_dlg;

    loaderror_dlg.setWindowTitle("Error");
    loaderror_dlg.setText("Failed to load images.");

    m_Proj0Deg=kipl::base::TImage<float,2>();
    m_Proj180Deg=kipl::base::TImage<float,2>();
    m_ProjOB=kipl::base::TImage<float,2>();
    m_ProjCumulative=kipl::base::TImage<float,2>();

    // Load references
    try {
        if (m_Config.ProjectionInfo.nOBCount)
            m_ProjOB=reader.Read(m_Config.ProjectionInfo.sReferencePath,
                    m_Config.ProjectionInfo.sOBFileMask,
                    1,
                    m_Config.ProjectionInfo.eFlip,
                    m_Config.ProjectionInfo.eRotate,
                    m_Config.ProjectionInfo.fBinning,
                    NULL);
    }
    catch (ReconException & e) {
        msg<<"Failed to load the open beam image (ReconException): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (kipl::base::KiplException & e) {
        msg<<"Failed to load the open beam image (kipl): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (std::exception & e) {
        msg<<"Failed to load the open beam image (STL): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (...) {
        msg<<"Failed to load the open beam image (unknown exception): ";
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }

    std::map<float,ProjectionInfo> projlist;
    BuildFileList(&m_Config,&projlist);
    std::map<float,ProjectionInfo>::iterator it,marked;

    size_t filtdims[]={3,3};
    kipl::filters::TMedianFilter<float,2> medfilt(filtdims);

    try {
        marked=projlist.begin();
        float diff=abs(marked->first);
        for (it=projlist.begin(); it!=projlist.end(); it++) {
            if (abs(it->first)<diff) {
                marked=it;
                diff=abs(marked->first);
            }
        }
        msg.str("");
        msg<<"Found first projection at "<<marked->second.angle<<", name: "<<marked->second.name<<", weight="<<marked->second.weight;
        logger(kipl::logging::Logger::LogMessage,msg.str());

        m_Proj0Deg=reader.Read(marked->second.
                name,
                m_Config.ProjectionInfo.eFlip,
                m_Config.ProjectionInfo.eRotate,
                m_Config.ProjectionInfo.fBinning,
                NULL);

        if (m_Proj0Deg.Size()==m_ProjOB.Size()) {
            float *pProj=m_Proj0Deg.GetDataPtr();
            float *pOB=m_ProjOB.GetDataPtr();

            for (size_t i=0; i<m_Proj0Deg.Size(); i++) {
                pProj[i]=pProj[i]/pOB[i];
                pProj[i]= pProj[i]<=0.0f ? 0.0f : -log(pProj[i]);
            }
        }
        else {
            logger(kipl::logging::Logger::LogWarning,"Open beam image does not have the same size as the projection");
        }
        m_Proj0Deg=medfilt(m_Proj0Deg);

    }
    catch (ReconException & e) {
        msg<<"Failed to load the first projection (ReconException): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (kipl::base::KiplException &e)
    {
        msg.str("");
        msg<<"Failed loading projection at 0 degrees: \n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (std::exception & e)
    {
        msg.str("");
        msg<<"Failed loading projection at 0 degrees:\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }

    try {
        marked=projlist.begin();
        float diff=abs(180.0f-marked->first);
        for (it=projlist.begin(); it!=projlist.end(); it++) {
            if (abs(180.0f-it->first)<diff) {
                marked=it;
                diff=abs(180.0f-marked->first);
            }
        }
        msg.str("");
        msg<<"Found opposite projection at "<<marked->second.angle<<", name: "<<marked->second.name<<", weight="<<marked->second.weight;
        logger(kipl::logging::Logger::LogMessage,msg.str());

        m_Proj180Deg=reader.Read(marked->second.name,
                m_Config.ProjectionInfo.eFlip,
                m_Config.ProjectionInfo.eRotate,
                m_Config.ProjectionInfo.fBinning,
                NULL);

        if (m_Proj180Deg.Size()==m_ProjOB.Size()) {
            float *pProj=m_Proj180Deg.GetDataPtr();
            float *pOB=m_ProjOB.GetDataPtr();
            for (size_t i=0; i<m_Proj180Deg.Size(); i++) {
                pProj[i]=pProj[i]/pOB[i];
                pProj[i]= pProj[i]<=0.0f ? 0.0f : -log(pProj[i]);
            }
        }
        else {
            logger(kipl::logging::Logger::LogWarning,"Open beam image does not have the same size as the projection");
        }

        m_Proj180Deg=medfilt(m_Proj180Deg);
    }
    catch (ReconException & e) {
        msg<<"Failed to load projection 180 degrees (ReconException): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (kipl::base::KiplException &e)
    {
        msg.str("");
        msg<<"Failed loading projection at 180 degrees:\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (std::exception & e)
    {
        msg.str("");
        msg<<"Failed loading projection at 180 degrees:\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }

    return 0;
}

void ConfigureGeometryDialog::UpdateConfig()
{
    m_Config.ProjectionInfo.bCorrectTilt = ui->checkUseTilt->isChecked();
    m_Config.ProjectionInfo.roi[0]       = ui->spinMarginLeft->value();
    m_Config.ProjectionInfo.roi[1]       = ui->spinSliceFirst->value();
    m_Config.ProjectionInfo.roi[2]       = ui->spinMarginRight->value();
    m_Config.ProjectionInfo.roi[3]       = ui->spinMarginLeft->value();
    m_Config.ProjectionInfo.fScanArc[0]  = ui->dspinAngleFirst->value();
    m_Config.ProjectionInfo.fScanArc[1]  = ui->dspinAngleLast->value();
    m_Config.ProjectionInfo.fCenter      = ui->dspinCenterRotation->value();
    m_Config.ProjectionInfo.fTiltAngle   = ui->dspinTiltAngle->value();
    m_Config.ProjectionInfo.fTiltPivotPosition = ui->dspinTiltPivot->value();

}

void ConfigureGeometryDialog::UpdateDialog()
{
    ui->checkUseTilt->setChecked(m_Config.ProjectionInfo.bCorrectTilt);
    ui->spinMarginLeft->setValue(m_Config.ProjectionInfo.roi[0]);
    ui->spinSliceFirst->setValue(m_Config.ProjectionInfo.roi[1]);
    ui->spinMarginRight->setValue(m_Config.ProjectionInfo.roi[2]);
    ui->spinMarginLeft->setValue(m_Config.ProjectionInfo.roi[3]);
    ui->dspinAngleFirst->setValue(m_Config.ProjectionInfo.fScanArc[0]);
    ui->dspinAngleLast->setValue(m_Config.ProjectionInfo.fScanArc[1]);
    ui->dspinCenterRotation->setValue(m_Config.ProjectionInfo.fCenter);
    ui->dspinTiltAngle->setValue(m_Config.ProjectionInfo.fTiltAngle);
    ui->dspinTiltPivot->setValue(m_Config.ProjectionInfo.fTiltPivotPosition);
}

